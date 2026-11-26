/*
----------------------------------------------------------------------

BSD 3-Clause License

Copyright (c) 2026, Cayden Andrews

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------
*/

#include "spike/assetimp/ShaderImporter.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <filesystem>
#include <ios>
#include <sstream>
#include <string>
#include <utility>

#include "spike/error.hpp"
#include "spike/warning.h"
#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

#include "spike/assetimp/aicollection.h"
#include "spike/assetimp/aishader.h"
#include "spike/assetimp/BaseImporter.h"

namespace {
	const assimp::ShaderType_ get_stage_type(string& str) {
		if (str == "VERTEX")
			return assimp::ShaderType_::ShaderType_vertex;
		else if (str == "FRAGMENT")
			return assimp::ShaderType_::ShaderType_fragment;
		else if (str == "GEOMETRY")
			return assimp::ShaderType_::ShaderType_geometry;
		else if (str == "TESS_CONTROL")
			return assimp::ShaderType_::ShaderType_tesc;
		else if (str == "TESS_EVAL")
			return assimp::ShaderType_::ShaderType_tese;
		else if (str == "COMPUTE")
			return assimp::ShaderType_::ShaderType_compute;
		else if (str == "MESH")
			return assimp::ShaderType_::ShaderType_mesh;
		else if (str == "TASK")
			return assimp::ShaderType_::ShaderType_task;
		return assimp::ShaderType_::ShaderType_unknown;
	}
}

assimp::ShaderImporter::ShaderImporter() : BaseImporter(), sourcePath() {
	extensions = { ".glsl", ".vert", ".geom", ".frag", ".comp", ".task", ".tesc", ".tese", ".mesh" };
}

assimp::ShaderImporter::~ShaderImporter() {}

errlib::Error assimp::ShaderImporter::InternReadFile(const string& file, Collection* scene) {
	std::ifstream ifs(file, std::ios::in | std::ios::binary);
	if (!ifs.is_open()) {
		return errlib::Error::FromCode(errlib::ErrorCode_missingFile, "Could not open shader file: " + file);
	}

	std::ostringstream stream;
	stream << ifs.rdbuf();
	if (ifs.bad()) {
		return errlib::Error::FromCode(errlib::ErrorCode_IOerror, "Failed while reading shader file: " + file);
	}

	ShaderProgram sp;
	sp.source = stream.str();
	if (sp.source.empty()) {
		return errlib::Error::FromCode(errlib::ErrorCode_invalidAsset, "Shader source is empty: " + file);
	}

	const string stageStartMarker = "// STAGE:";
	string::size_type pos = 0;
	while ((pos = sp.source.find(stageStartMarker, pos)) != string::npos) {
		pos += stageStartMarker.size();
		const usize start = sp.source.find_first_not_of(" \t", pos);
		if (start == string::npos) {
			return errlib::Error::FromCode(errlib::ErrorCode_invalidAsset, "Shader stage marker has no stage name.");
		}
		const usize end = sp.source.find_first_of("\r\n", start);

		string type = sp.source.substr(start, end == string::npos ? string::npos : end - start);
		const usize last = type.find_last_not_of(" \t");
		if (last != string::npos) type.resize(last + 1);
		ShaderStage stage{ get_stage_type(type) };
		if (stage.type == ShaderType_unknown) {
			return errlib::Error::FromCode(errlib::ErrorCode_invalidAsset, "Unsupported shader stage: " + type);
		}
		sp.shaders.push_back(stage);
		pos = end == string::npos ? sp.source.size() : end;
	}

	const std::filesystem::path path(file);
	if (sp.shaders.empty()) { // no markers found
		string extension = path.extension().string();
		std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
		ShaderType_ stage = ShaderType_unknown;
		if (extension == ".vert") stage = ShaderType_vertex;
		else if (extension == ".frag") stage = ShaderType_fragment;
		else if (extension == ".geom") stage = ShaderType_geometry;
		else if (extension == ".tesc") stage = ShaderType_tesc;
		else if (extension == ".tese") stage = ShaderType_tese;
		else if (extension == ".comp") stage = ShaderType_compute;
		else if (extension == ".mesh") stage = ShaderType_mesh;
		else if (extension == ".task") stage = ShaderType_task;
		if (stage == ShaderType_unknown) {
			return errlib::Error::FromCode(errlib::ErrorCode_invalidAsset, "Shader source does not identify a shader stage.");
		}
		sp.shaders.push_back(ShaderStage{ stage });
	}

	sp.name = path.filename().string();
	scene->shaders.push_back(std::move(sp));
	scene->name = path.filename().stem().string();
	sourcePath = std::filesystem::absolute(path).string();
	return errlib::Error{};
}
