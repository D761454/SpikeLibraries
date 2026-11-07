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

#include "spike/assetimp/ObjImporter.h"

#include <functional>
#include <sstream>
#include <array>
#include <cmath>
#include <deque>
#include <filesystem>
#include <forward_list>
#include <ios>
#include <list>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <system_error>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <fstream>

#include "spike/debugl/debug.hpp"
#include "spike/error.hpp"
#include "spike/warning.h"
#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"
#include "spike/sml/vector2.h"
#include "spike/sml/vector3.h"
#include "spike/sml/vector4.h"

#include "spike/assetimp/PostProcessing.h"
#include "spike/assetimp/aicollection.h"
#include "spike/assetimp/aimesh.h"
#include "spike/assetimp/BaseImporter.h"

DISABLE_WARNING_UNUSED_VARIABLE

namespace {
	struct ObjTuple {
		u32 posIdx;
		u32 normIdx;
		u32 tcIdx;

		bool operator==(const ObjTuple& other) const {
			return posIdx == other.posIdx && tcIdx == other.tcIdx && normIdx == other.normIdx;
		}
	};

	struct Poly {
		std::vector<ObjTuple> indices;
	};
}

namespace std {
	template<> struct std::hash<ObjTuple> {
		size_t operator()(const ObjTuple& t) const {
			return std::hash<u32>()(t.posIdx) ^ (std::hash<u32>()(t.tcIdx) << 1) ^ (std::hash<u32>()(t.normIdx) << 2);
		}
	};
}

assimp::ObjImporter::ObjImporter() : BaseImporter(), sourcePath() {
	extensions = { ".obj" };
}

assimp::ObjImporter::~ObjImporter() {}

namespace {
	inline f32 dot(const sml::vector3& lhs, const sml::vector3& rhs) {
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	inline sml::vector3 cross(const sml::vector3& lhs, const sml::vector3& rhs) {
		return {
			lhs.y * rhs.z - lhs.z * rhs.y,
			lhs.z * rhs.x - lhs.x * rhs.z,
			lhs.x * rhs.y - lhs.y * rhs.x
		};
	}

	inline sml::vector3 normalize(const sml::vector3& v) {
		f32 lenSq = dot(v, v);
		if (lenSq > 1e-8f) {
			f32 invLen = 1.0f / std::sqrt(lenSq);
			return v * invLen;
		}
		return { 0.0f, 0.0f, 0.0f };
	}
}

static bool parse_face_tuple(const string& tuple, ObjTuple& face) {
	std::istringstream iss(tuple);
	string p;

	std::getline(iss, p, '/');
	if (p.empty()) return false;
	face.posIdx = std::stoi(p) - 1;

	if (std::getline(iss, p, '/') && !p.empty()) face.tcIdx = std::stoi(p) - 1;
	else face.tcIdx = kMaxU32;
	if (std::getline(iss, p, '/') && !p.empty()) face.normIdx = std::stoi(p) - 1;
	else face.normIdx = kMaxU32;
	return true;
}

static bool parse_face(const std::vector<string>& verts, std::vector<ObjTuple>& vertices) {
	if (verts.size() < 3) return false;
	for (const auto& vert : verts) {
		ObjTuple fv;
		if (!parse_face_tuple(vert, fv)) return false;
		vertices.emplace_back(fv);
	}
	return true;
}

/*Generates raw data buffers and sub-mesh offsets (pre validation)*/
static errlib::Error raw_import(const string& file,
	std::vector<sml::vector3>& positionBuffer,
	std::vector<sml::vector3>& normalBuffer,
	std::vector<sml::vector2>& uvBuffer,
	std::vector<sml::vector4>& tangentBuffer,
	std::vector<Poly>& faceBuffer,
	assimp::Model& model)
{
	std::ifstream fileStream;
	fileStream.exceptions(std::ifstream::badbit);
	try {
		fileStream.open(file, std::ios::in | std::ios::binary);
		if (!fileStream.is_open()) throw std::ifstream::failure("Could not find file at path", { 1, std::iostream_category() });

		positionBuffer.reserve(100);
		normalBuffer.reserve(100);
		uvBuffer.reserve(100);
		tangentBuffer.reserve(100);
		faceBuffer.reserve(100);

		assimp::SubMesh sm;

		umap<string, u32> materialMap;

		for (string tmp; std::getline(fileStream, tmp);) {
			std::istringstream iss(tmp);
			string pref;
			iss >> pref;

			if (pref == "v") {
				f32 x, y, z;
				iss >> x >> y >> z;
				positionBuffer.emplace_back(sml::vector3(x, y, z));
			}
			else if (pref == "vn") {
				f32 x, y, z;
				iss >> x >> y >> z;
				normalBuffer.emplace_back(sml::vector3(x, y, z));
			}
			else if (pref == "vt") {
				f32 u, v;
				iss >> u >> v;
				uvBuffer.emplace_back(sml::vector2(u, v));
			}
			else if (pref == "f") {
				std::vector<string> face;
				string tuple;
				while (iss >> tuple) face.push_back(tuple);
				Poly p;
				if (!parse_face(face, p.indices)) return errlib::Error(errlib::ErrorCode_invalidArgument, file);
				faceBuffer.push_back(std::move(p));
				sm.indexCount++; // face count atm
			}
			else if (pref == "o") {
				if (sm.indexCount > 0) {
					u32 offset = sm.indexOffset + sm.indexCount;
					model.subMeshes.push_back(std::move(sm));
					sm = assimp::SubMesh();
					sm.indexOffset = offset;
				}
				string name;
				iss >> name;

				if (model.name.empty()) {
					model.name = name;
				}
			}
			else if (pref == "usemtl") {
				string mat;
				iss >> mat;
				if (auto it = materialMap.find(mat); it != materialMap.end()) {
					sm.materialIdx = materialMap[mat];
				}
				else {
					sm.materialIdx = static_cast<u8>(materialMap.size());
					materialMap.insert(std::pair<string, u32>(mat, static_cast<u32>(materialMap.size())));
					model.materialCount++;
				}
			}
		}
		fileStream.close();

		model.subMeshes.push_back(std::move(sm));

		positionBuffer.shrink_to_fit();
		normalBuffer.shrink_to_fit();
		uvBuffer.shrink_to_fit();
		tangentBuffer.shrink_to_fit();
		faceBuffer.shrink_to_fit();
	}
	catch (std::ifstream::failure e) {
		Debug::LogError("Error reading file at path: {} \nError: {}, Code: {}: {}", file, e.what(), e.code().value(), e.code().message());
		return errlib::Error(errlib::ErrorCode_IOerror, file);
	}

	return errlib::Error{};
}

/*modify face to ensure it is made of triangles*/
static void validate_faces(std::vector<Poly>& faces) {
	const usize faceCount = faces.size();
	for (usize i = 0; i < faceCount; i++) {
		Poly& face = faces[i];
		const usize oldSize = face.indices.size();
		if (oldSize == 3) continue;

		const usize additions = (oldSize - 3) * 2;
		std::vector<ObjTuple> tmp;
		tmp.reserve(oldSize + additions);
		ObjTuple zero = face.indices[0];

		for (usize j = 0; j < oldSize; j++) {
			if (j > 2) {
				tmp.emplace_back(zero);
				tmp.emplace_back(face.indices[j - 1]);
			}
			tmp.emplace_back(face.indices[j]);
		}

		face.indices.swap(tmp);
	}
}

/*very basic fixer, parses missing data from nearby vertices in same face struct*/
static void generate_normals(std::vector<Poly>& faces) {
	for (auto& face : faces) {
		u32 idx = kMaxU32;
		for (u32 i = 0; i < face.indices.size(); i++) {
			idx = face.indices[i].normIdx;
			if (idx != kMaxU32) {
				for (auto& tup : face.indices) {
					tup.normIdx = idx;
				}
				break;
			}
		}
	}
}

template<typename T, typename U>
void generate_buffer(
	std::vector<sml::vector3>& positionBuffer,
	std::vector<sml::vector3>& normalBuffer,
	std::vector<sml::vector2>& uvBuffer,
	std::vector<sml::vector4>& tangentBuffer,
	std::vector<Poly>& faceBuffer,
	assimp::Model& model, std::vector<T>& vertexBuffer, std::vector<U>& indexBuffer)
{
	umap<ObjTuple, U> tupleToIndex;

	vertexBuffer.reserve(positionBuffer.size());
	for (auto& subMesh : model.subMeshes) {
		u32 nidc = 0;
		const u32 faceEnd = subMesh.indexOffset + subMesh.indexCount;
		for (u32 i = subMesh.indexOffset; i < faceEnd; i++) {
			Poly& face = faceBuffer[i];
			nidc += static_cast<u32>(face.indices.size());

			for (auto& tup : face.indices) {
				if (auto it = tupleToIndex.find(tup); it != tupleToIndex.end()) {
					indexBuffer.emplace_back(static_cast<U>(it->second));
				}
				else {
					U s = static_cast<U>(tupleToIndex.size());
					tupleToIndex.emplace(tup, s);
					indexBuffer.emplace_back(s);
					vertexBuffer.emplace_back(positionBuffer[tup.posIdx],
						tup.normIdx == kMaxU32 ? sml::vector3() : normalBuffer[tup.normIdx],
						tup.tcIdx == kMaxU32 ? sml::vector2() : uvBuffer[tup.tcIdx]);
				}
			}
		}
		subMesh.indexCount = nidc;
	}
}

template<typename T>
inline void generate_tangents(assimp::Model& model, std::vector<T>& idxBuff) {
	std::vector<sml::vector3> tan1, tan2;
	tan1.resize(model.vertices48.size()); tan2.resize(model.vertices48.size());

	for (u32 i = 0; i < static_cast<u32>(idxBuff.size()); i += 3) {
		T i1 = idxBuff[i];
		T i2 = idxBuff[i + 1];
		T i3 = idxBuff[i + 2];

		assimp::VertexP3N3T2T4& v1 = model.vertices48[i1];
		assimp::VertexP3N3T2T4& v2 = model.vertices48[i2];
		assimp::VertexP3N3T2T4& v3 = model.vertices48[i3];

		f32 x1 = v2.position.x - v1.position.x;
		f32 x2 = v3.position.x - v1.position.x;
		f32 y1 = v2.position.y - v1.position.y;
		f32 y2 = v3.position.y - v1.position.y;
		f32 z1 = v2.position.z - v1.position.z;
		f32 z2 = v3.position.z - v1.position.z;

		f32 s1 = v2.texCoord.x - v1.texCoord.x;
		f32 s2 = v3.texCoord.x - v1.texCoord.x;
		f32 t1 = v2.texCoord.y - v1.texCoord.y;
		f32 t2 = v3.texCoord.y - v1.texCoord.y;

		f32 r = (s1 * t2 - s2 * t1);
		r = (std::abs(r) < 1e-8f) ? 1.0f : (1.0f / r);

		sml::vector3 sdir = { // tangent dir
			(t2 * x1 - t1 * x2) * r,
			(t2 * y1 - t1 * y2) * r,
			(t2 * z1 - t1 * z2) * r
		};

		sml::vector3 tdir = { // bitangent dir
			(s1 * x2 - s2 * x1) * r,
			(s1 * y2 - s2 * y1) * r,
			(s1 * z2 - s2 * z1) * r
		};

		tan1[i1] += sdir;
		tan1[i2] += sdir;
		tan1[i3] += sdir;

		tan2[i1] += tdir;
		tan2[i2] += tdir;
		tan2[i3] += tdir;
	}

	for (u32 i = 0; i < static_cast<u32>(idxBuff.size()); ++i) {
		const sml::vector3& norm = model.vertices48[i].normal;
		const sml::vector3& t = tan1[i];
		const sml::vector3& b = tan2[i];

		sml::vector3 tanXYZ = normalize(t - norm * dot(norm, t)); // Gram-Schmidt orthogonalize
		f32 h = (dot(cross(norm, t), b) < 0.f) ? -1.f : 1.f;
		model.vertices48[i].tangent.Set(tanXYZ.x, tanXYZ.y, tanXYZ.z, h);
	}
}

template<typename T>
inline void flip_winding_order(std::vector<T>& indexBuffer) {
	for (u32 i = 0; i < static_cast<u32>(indexBuffer.size()); i += 3) {
		std::swap(indexBuffer[i + 1], indexBuffer[i + 2]);
	}
}

errlib::Error assimp::ObjImporter::InternReadFile(const string& file, Collection* scene) {
	std::vector<sml::vector3> tmpPos;
	std::vector<sml::vector3> tmpNorm;
	std::vector<sml::vector2> tmpUV;
	std::vector<sml::vector4> tmpTang;
	std::vector<Poly> faces;

	Model model;

	if (errlib::Error e = raw_import(file, tmpPos, tmpNorm, tmpUV, tmpTang, faces, model); !e.ok()) {
		return e;
	}

	const bool index32 = tmpPos.size() > static_cast<usize>(kMaxU16);

	if (ppFlags & PostProcessingFlags_triangulateFaces) validate_faces(faces);
	if (ppFlags & PostProcessingFlags_generateMissingNormals) generate_normals(faces);

	if (ppFlags & PostProcessingFlags_flipUVs) {
		for (auto& uv : tmpUV) uv.y *= -1;
	}

	if (ppFlags & PostProcessingFlags_importTangents || ppFlags & PostProcessingFlags_pbrImport) {
		if (index32) generate_buffer(tmpPos, tmpNorm, tmpUV, tmpTang, faces, model, model.vertices48, model.indexBuffer32);
		else generate_buffer(tmpPos, tmpNorm, tmpUV, tmpTang, faces, model, model.vertices48, model.indexBuffer16);
	}
	else {
		if (index32) generate_buffer(tmpPos, tmpNorm, tmpUV, tmpTang, faces, model, model.vertices32, model.indexBuffer32);
		else generate_buffer(tmpPos, tmpNorm, tmpUV, tmpTang, faces, model, model.vertices32, model.indexBuffer16);
	}

	if (ppFlags & PostProcessingFlags_importTangents || ppFlags & PostProcessingFlags_pbrImport) {
		if (index32) generate_tangents(model, model.indexBuffer32);
		else generate_tangents(model, model.indexBuffer16);
	}

	if (ppFlags & PostProcessingFlags_flipWindingOrder) {
		if (index32) flip_winding_order(model.indexBuffer32);
		else flip_winding_order(model.indexBuffer16);
	}

	for (usize i = 1; i < model.subMeshes.size(); i++) {
		model.subMeshes[i].indexOffset = model.subMeshes[i - 1].indexOffset + model.subMeshes[i - 1].indexCount;
	}
	for (auto& subMesh : model.subMeshes) {
		if (ppFlags & PostProcessingFlags_importTangents || ppFlags & PostProcessingFlags_pbrImport) {
			if (index32) calculate_submesh_bounds(subMesh, model.vertices48, model.indexBuffer32);
			else calculate_submesh_bounds(subMesh, model.vertices48, model.indexBuffer16);
		}
		else {
			if (index32) calculate_submesh_bounds(subMesh, model.vertices32, model.indexBuffer32);
			else calculate_submesh_bounds(subMesh, model.vertices32, model.indexBuffer16);
		}
	}

	scene->models.push_back(std::move(model));

	scene->name = std::filesystem::path(file).filename().stem().string();
	sourcePath = std::filesystem::absolute(file).string();

	return errlib::Error{};
}