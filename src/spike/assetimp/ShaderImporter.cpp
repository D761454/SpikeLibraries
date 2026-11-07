#include "spike/assetimp/ShaderImporter.h"

#include <fstream>
#include <filesystem>
#include <ios>
#include <sstream>
#include <string>
#include <utility>

#include "spike/debugl/debug.hpp"
#include "spike/error.hpp"
#include "spike/warning.h"
#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

#include "spike/assetimp/aicollection.h"
#include "spike/assetimp/aishader.h"
#include "spike/assetimp/BaseImporter.h"

DISABLE_WARNING_UNUSED_VARIABLE

namespace {
	const assimp::ShaderType_ get_stage_type(string& str) {
		if (str == "VERTEX") {
			return assimp::ShaderType_::ShaderType_vertex;
		}
		else if (str == "FRAGMENT") {
			return assimp::ShaderType_::ShaderType_fragment;
		}
		else if (str == "GEOMETRY") {
			return assimp::ShaderType_::ShaderType_geometry;
		}
		else if (str == "TESS_CONTROL") {
			return assimp::ShaderType_::ShaderType_tesc;
		}
		else if (str == "TESS_EVAL") {
			return assimp::ShaderType_::ShaderType_tese;
		}
		else if (str == "COMPUTE") {
			return assimp::ShaderType_::ShaderType_compute;
		}
		else if (str == "MESH") {
			return assimp::ShaderType_::ShaderType_mesh;
		}
		else if (str == "TASK") {
			return assimp::ShaderType_::ShaderType_task;
		}
		return assimp::ShaderType_::ShaderType_unknown;
	}
}

assimp::ShaderImporter::ShaderImporter() : BaseImporter(), sourcePath() {
	extensions = { ".glsl", ".vert", ".geom", ".frag", ".comp", ".task", ".tesc", ".tese", ".mesh" };
}

assimp::ShaderImporter::~ShaderImporter() {}

errlib::Error assimp::ShaderImporter::InternReadFile(const string& file, Collection* scene) {
	ShaderProgram sp;

	// parse over file, grabbing each stage and adding to program

	// Extract version + common code
	// Scan for "// STAGE:" markers
	// Extract each stage section

	string content;

	std::ifstream ifs;
	ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		ifs.open(file, std::ios::in);
		std::stringstream stream;
		stream << ifs.rdbuf();
		ifs.close();
		content = stream.str();
	}
	catch (std::ifstream::failure e) {
		Debug::LogError("Could not read file at path: {}", file);
		return errlib::Error(errlib::ErrorCode_notFound, "Could not read shader file: " + file);
	}

	sp.source = content;

	string stageStartMarker = "// STAGE:";
	string::size_type pos = 0;

	while ((pos = content.find(stageStartMarker, pos)) != string::npos) {
		pos += stageStartMarker.size();

		usize p1 = content.find_first_not_of(" ", pos);
		if (p1 == string::npos) continue;
		usize p2 = content.find_first_of("\n", p1);

		string type = content.substr(p1, p2 - p1);

		ShaderStage stage;
		stage.type = get_stage_type(type);

		sp.shaders.push_back(stage);
	}

	scene->shaders.push_back(std::move(sp));
	std::filesystem::path fp(file);
	sp.name = fp.filename().string();
	scene->name = fp.filename().stem().string();
	sourcePath = std::filesystem::absolute(file).string();

	return errlib::Error{};
}