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
#include <cassert>
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
#include <exception>
#include <algorithm>
#include <cstdlib>
#include <istream>

#include "spike/error.hpp"
#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

#include "spike/assetimp/PostProcessing.h"
#include "spike/assetimp/aicollection.h"
#include "spike/assetimp/aivector2.h"
#include "spike/assetimp/aivector3.h"
#include "spike/assetimp/aivector4.h"
#include "spike/assetimp/aimesh.h"
#include "spike/assetimp/BaseImporter.h"

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
	template<> struct hash<ObjTuple> {
		size_t operator()(const ObjTuple& t) const {
			return hash<u32>()(t.posIdx) ^ (hash<u32>()(t.tcIdx) << 1) ^ (hash<u32>()(t.normIdx) << 2);
		}
	};
}

assimp::ObjImporter::ObjImporter() : BaseImporter(), sourcePath() {
	extensions = { ".obj" };
}

assimp::ObjImporter::~ObjImporter() {}

namespace {
	inline f32 dot(const assimp::vector3& lhs, const assimp::vector3& rhs) {
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	inline assimp::vector3 cross(const assimp::vector3& lhs, const assimp::vector3& rhs) {
		return {
			lhs.y * rhs.z - lhs.z * rhs.y,
			lhs.z * rhs.x - lhs.x * rhs.z,
			lhs.x * rhs.y - lhs.y * rhs.x
		};
	}

	inline assimp::vector3 normalize(const assimp::vector3& v) {
		f32 lenSq = dot(v, v);
		if (lenSq > 1e-8f) {
			f32 invLen = 1.0f / std::sqrt(lenSq);
			return v * invLen;
		}
		return { 0.0f, 0.0f, 0.0f };
	}
}

static bool parse_obj_index(const string& text, usize count, u32& index, bool optional) {
	if (text.empty()) {
		index = kMaxU32;
		return optional;
	}

	try {
		usize parsed = 0;
		const ll raw = std::stoll(text, &parsed);
		if (parsed != text.size() || raw == 0) return false;
		const ll resolved = raw > 0 ? raw - 1 : static_cast<ll>(count) + raw;
		if (resolved < 0 || static_cast<ull>(resolved) >= count || resolved >= kMaxU32) return false;
		index = static_cast<u32>(resolved);
		return true;
	}
	catch (const std::exception&) {
		return false;
	}
}

static bool parse_face_tuple(const string& tuple, ObjTuple& face, usize positionCount, usize uvCount, usize normalCount) {
	std::array<string, 3> fields;
	usize field = 0;
	usize start = 0;
	for (usize i = 0; i <= tuple.size(); ++i) {
		if (i == tuple.size() || tuple[i] == '/') {
			if (field >= fields.size()) return false;
			fields[field++] = tuple.substr(start, i - start);
			start = i + 1;
		}
	}
	return field >= 1
		&& parse_obj_index(fields[0], positionCount, face.posIdx, false)
		&& parse_obj_index(fields[1], uvCount, face.tcIdx, true)
		&& parse_obj_index(fields[2], normalCount, face.normIdx, true);
}

static bool parse_face(const std::vector<string>& verts, std::vector<ObjTuple>& vertices, usize positionCount, usize uvCount, usize normalCount) {
	if (verts.size() < 3) return false;
	for (const auto& vert : verts) {
		ObjTuple face;
		if (!parse_face_tuple(vert, face, positionCount, uvCount, normalCount)) return false;
		vertices.emplace_back(face);
	}
	return true;
}

/*Generates raw data buffers and sub-mesh offsets (pre validation)*/
static errlib::Error raw_import(const string& file,
	std::vector<assimp::vector3>& positionBuffer,
	std::vector<assimp::vector3>& normalBuffer,
	std::vector<assimp::vector2>& uvBuffer,
	std::vector<assimp::vector4>& tangentBuffer,
	std::vector<Poly>& faceBuffer,
	assimp::Mesh& model)
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
		auto finishSubMesh = [&]() {
			if (model.subMeshes.size() > 0) {
				const auto& last = model.subMeshes[model.subMeshes.size() - 1];
				sm.indexCount = static_cast<u32>(faceBuffer.size()) - (last.indexCount + last.indexOffset);
			}
			else sm.indexCount = static_cast<u32>(faceBuffer.size());

			model.subMeshes.push_back(std::move(sm));
			sm = assimp::SubMesh();
			sm.indexOffset = static_cast<u32>(faceBuffer.size());
		};

		umap<string, u32> materialMap;

		for (string tmp; std::getline(fileStream, tmp);) {
			const usize comment = tmp.find('#');
			if (comment != string::npos) tmp.resize(comment);
			std::istringstream iss(tmp);
			string pref;
			iss >> pref;
			if (pref.empty()) continue;

			if (pref == "v") {
				f32 x = 0, y = 0, z = 0;
				if (!(iss >> x >> y >> z) || !std::isfinite(x) || !std::isfinite(y) || !std::isfinite(z))
					return errlib::Error::FromCode(errlib::ErrorCode_invalidAsset, "Invalid position record in " + file);
				positionBuffer.emplace_back(assimp::vector3(x, y, z));
			}
			else if (pref == "vn") {
				f32 x = 0, y = 0, z = 0;
				if (!(iss >> x >> y >> z) || !std::isfinite(x) || !std::isfinite(y) || !std::isfinite(z))
					return errlib::Error::FromCode(errlib::ErrorCode_invalidAsset, "Invalid normal record in " + file);
				normalBuffer.emplace_back(assimp::vector3(x, y, z));
			}
			else if (pref == "vt") {
				f32 u = 0, v = 0;
				if (!(iss >> u >> v) || !std::isfinite(u) || !std::isfinite(v))
					return errlib::Error::FromCode(errlib::ErrorCode_invalidAsset, "Invalid texture-coordinate record in " + file);
				uvBuffer.emplace_back(assimp::vector2(u, v));
			}
			else if (pref == "f") {
				std::vector<string> face;
				string tuple;
				while (iss >> tuple) face.push_back(tuple);
				Poly p;
				if (!parse_face(face, p.indices, positionBuffer.size(), uvBuffer.size(), normalBuffer.size()))
					return errlib::Error::FromCode(errlib::ErrorCode_invalidAsset, "Invalid or out-of-range face in " + file);
				faceBuffer.push_back(std::move(p));
			}
			else if (pref == "o") {
				finishSubMesh();
				string name;
				std::getline(iss >> std::ws, name);
				if (model.name.empty()) model.name = name;
			}
			else if (pref == "usemtl") {
				string material;
				if (!(iss >> material))
					return errlib::Error::FromCode(errlib::ErrorCode_invalidAsset, "Missing material name in " + file);
				finishSubMesh();
				if (auto it = materialMap.find(material); it != materialMap.end()) {
					sm.materialIdx = static_cast<u8>(it->second);
				}
				else {
					if (materialMap.size() >= kMaxU8)
						return errlib::Error::FromCode(errlib::ErrorCode_outOfBounds, "OBJ exceeds the supported material count");
					const u32 materialIndex = static_cast<u32>(materialMap.size());
					materialMap.emplace(material, materialIndex);
					sm.materialIdx = static_cast<u8>(materialIndex);
				}
			}
		}
		model.materialCount = static_cast<u8>(materialMap.size());
		fileStream.close();

		finishSubMesh();
		if (positionBuffer.empty() || faceBuffer.empty())
			return errlib::Error::FromCode(errlib::ErrorCode_invalidAsset, "OBJ contains no mesh faces: " + file);

		positionBuffer.shrink_to_fit();
		normalBuffer.shrink_to_fit();
		uvBuffer.shrink_to_fit();
		tangentBuffer.shrink_to_fit();
		faceBuffer.shrink_to_fit();
	}
	catch (std::ifstream::failure e) {
		return errlib::Error::FromCode(errlib::ErrorCode_IOerror, "Error reading file at path: " + file + " Error: " + e.what());
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

static void generate_normals(std::vector<Poly>& faces, const std::vector<assimp::vector3>& positions, std::vector<assimp::vector3>& normals) {
	for (auto& face : faces) {
		if (face.indices.size() < 3 || std::all_of(face.indices.begin(), face.indices.end(), [](const ObjTuple& vertex) { return vertex.normIdx != kMaxU32; })) continue;
		const assimp::vector3& a = positions[face.indices[0].posIdx];
		const assimp::vector3& b = positions[face.indices[1].posIdx];
		const assimp::vector3& c = positions[face.indices[2].posIdx];
		const assimp::vector3 normal = normalize(cross(b - a, c - a));
		const u32 normalIndex = static_cast<u32>(normals.size());
		normals.push_back(normal);
		for (auto& vertex : face.indices) {
			if (vertex.normIdx == kMaxU32) vertex.normIdx = normalIndex;
		}
	}
}

static void generate_missing_uvs(std::vector<Poly>& faces, const std::vector<assimp::vector3>& positions, std::vector<assimp::vector2>& uvs) {
	for (auto& face : faces) {
		if (face.indices.size() < 3 || std::all_of(face.indices.begin(), face.indices.end(), [](const ObjTuple& vertex) { return vertex.tcIdx != kMaxU32; })) continue;
		const assimp::vector3& a = positions[face.indices[0].posIdx];
		const assimp::vector3& b = positions[face.indices[1].posIdx];
		const assimp::vector3& c = positions[face.indices[2].posIdx];
		const assimp::vector3 normal = cross(b - a, c - a);
		const f32 absX = std::abs(normal.x), absY = std::abs(normal.y), absZ = std::abs(normal.z);
		for (auto& vertex : face.indices) {
			if (vertex.tcIdx != kMaxU32) continue;
			const assimp::vector3& position = positions[vertex.posIdx];
			const assimp::vector2 uv = absX >= absY && absX >= absZ ? assimp::vector2(position.y, position.z)
				: absY >= absZ ? assimp::vector2(position.x, position.z) : assimp::vector2(position.x, position.y);
			vertex.tcIdx = static_cast<u32>(uvs.size());
			uvs.push_back(uv);
		}
	}
}

template<typename T, typename U>
void generate_buffer(
	std::vector<assimp::vector3>& positionBuffer,
	std::vector<assimp::vector3>& normalBuffer,
	std::vector<assimp::vector2>& uvBuffer,
	std::vector<assimp::vector4>& tangentBuffer,
	std::vector<Poly>& faceBuffer,
	assimp::Mesh& model, std::vector<T>& vertexBuffer, std::vector<U>& indexBuffer)
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
						tup.normIdx == kMaxU32 ? assimp::vector3() : normalBuffer[tup.normIdx],
						tup.tcIdx == kMaxU32 ? assimp::vector2() : uvBuffer[tup.tcIdx]);
				}
			}
		}
		subMesh.indexCount = nidc;
	}
}

template<typename T>
inline void generate_tangents(assimp::Mesh& model, std::vector<T>& idxBuff) {
	std::vector<assimp::vector3> tan1(model.vertices48.size()), tan2(model.vertices48.size());

	for (usize i = 0; i + 2 < idxBuff.size(); i += 3) {
		const usize i1 = idxBuff[i];
		const usize i2 = idxBuff[i + 1];
		const usize i3 = idxBuff[i + 2];
		assert(i1 < model.vertices48.size() && i2 < model.vertices48.size() && i3 < model.vertices48.size());
		if (i1 >= model.vertices48.size() || i2 >= model.vertices48.size() || i3 >= model.vertices48.size()) return;

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

		assimp::vector3 sdir = { // tangent dir
			(t2 * x1 - t1 * x2) * r,
			(t2 * y1 - t1 * y2) * r,
			(t2 * z1 - t1 * z2) * r
		};

		assimp::vector3 tdir = { // bitangent dir
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

	for (usize i = 0; i < model.vertices48.size(); ++i) {
		const assimp::vector3& norm = model.vertices48[i].normal;
		const assimp::vector3& tangent = tan1[i];
		const assimp::vector3& bitangent = tan2[i];

		const assimp::vector3 tangentXYZ = normalize(tangent - norm * dot(norm, tangent));
		const f32 handedness = dot(cross(norm, tangent), bitangent) < 0.f ? -1.f : 1.f;
		model.vertices48[i].tangent = { tangentXYZ.x, tangentXYZ.y, tangentXYZ.z, handedness };
	}
}

template<typename T>
inline void flip_winding_order(std::vector<T>& indexBuffer) {
	for (usize i = 0; i < indexBuffer.size(); i += 3) {
		std::swap(indexBuffer[i + 1], indexBuffer[i + 2]);
	}
}

errlib::Error assimp::ObjImporter::InternReadFile(const string& file, Collection* scene) {
	std::vector<assimp::vector3> tmpPos;
	std::vector<assimp::vector3> tmpNorm;
	std::vector<assimp::vector2> tmpUV;
	std::vector<assimp::vector4> tmpTang;
	std::vector<Poly> faces;

	Mesh model;

	if (errlib::Error e = raw_import(file, tmpPos, tmpNorm, tmpUV, tmpTang, faces, model); !e.ok()) {
		return e;
	}

	const bool generateTangents = (ppFlags & PostProcessingFlags_importTangents) || (ppFlags & PostProcessingFlags_pbrImport);
	const bool index32 = tmpPos.size() > static_cast<usize>(kMaxU16);

	if ((ppFlags & PostProcessingFlags_triangulateFaces) || generateTangents || (ppFlags & PostProcessingFlags_flipWindingOrder))
		validate_faces(faces);
	if ((ppFlags & PostProcessingFlags_generateMissingNormals) || generateTangents)
		generate_normals(faces, tmpPos, tmpNorm);
	if ((ppFlags & PostProcessingFlags_generateMissingUVs) || generateTangents)
		generate_missing_uvs(faces, tmpPos, tmpUV);

	if (ppFlags & PostProcessingFlags_flipUVs) {
		for (auto& uv : tmpUV) uv.y *= -1;
	}

	if (model.name.empty()) model.name = std::filesystem::path(file).filename().stem().string();

	if (generateTangents) {
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