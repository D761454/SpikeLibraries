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

#if defined(_USING_FBX)

#include "spike/assetimp/FbxImporter.h"

#include <string.h>
#include <exception>
#include <filesystem>
#include <memory>
#include <utility>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <cstdlib>

#include "spike/warning.h"
#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"
#include "spike/error.hpp"

DISABLE_WARNING_PUSH
DISABLE_WARNING_FBX
#include <fbxsdk.h>
#include <fbxsdk/core/fbxmanager.h>
#include <fbxsdk/core/fbxobject.h>
#include <fbxsdk/core/math/fbxvector4.h>
#include <fbxsdk/fileio/fbximporter.h>
#include <fbxsdk/fileio/fbxiosettings.h>
#include <fbxsdk/fileio/fbxiosettingspath.h>
#include <fbxsdk/scene/fbxscene.h>
#include <fbxsdk/scene/geometry/fbxgeometry.h>
#include <fbxsdk/scene/geometry/fbxlayer.h>
#include <fbxsdk/scene/geometry/fbxmesh.h>
#include <fbxsdk/scene/geometry/fbxnode.h>
#include <fbxsdk/scene/geometry/fbxnodeattribute.h>
#include <fbxsdk/utils/fbxgeometryconverter.h>
DISABLE_WARNING_POP

#include "spike/assetimp/aivector2.h"
#include "spike/assetimp/aivector3.h"
#include "spike/assetimp/aivector4.h"
#include "spike/assetimp/aimesh.h"
#include "spike/assetimp/BaseImporter.h"
#include "spike/assetimp/PostProcessing.h"
#include "spike/assetimp/aicollection.h"

namespace {
	struct FbxManagerDeleter {
		void operator()(fbxsdk::FbxManager* manager) const noexcept {
			if (manager) manager->Destroy();
		}
	};

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

assimp::FbxImporter::FbxImporter() : BaseImporter(), sourcePath() {
	extensions = { ".fbx" };
}

assimp::FbxImporter::~FbxImporter() {}

namespace {
	struct Poly {
		std::vector<u32> indices;
		u32 material;

		Poly() noexcept : indices(), material(0) {}
		Poly(const Poly& other) { *this = other; }
		~Poly() {}

		Poly& operator=(const Poly& other) {
			if (&other == this) return *this;

			if (other.indices.size() > 0) {
				indices.resize(other.indices.size());
				std::memcpy(indices.data(), other.indices.data(), other.indices.size() * sizeof(u32));
			}
			else indices.clear();

			return *this;
		}

		bool operator==(const Poly& other) const {
			if (indices.size() != other.indices.size()) return false;
			for (u32 i = 0; i < this->indices.size(); ++i) {
				if (indices[i] != other.indices[i]) return false;
			}

			return true;
		}

		bool operator!=(const Poly& other) const {
			return !(*this == other);
		}
	};

	umap<assimp::VertexP3N3T2, u32, assimp::VertexHash> vertexCache;
}

static void read_normal(FbxMesh* inMesh, i32 ctrlPtIdx, i32 vertCounter, assimp::vector3& outNormal) {
	if (inMesh->GetElementNormalCount() < 1) throw std::exception("Invalid Normal Number");

	FbxGeometryElementNormal* vertNormal = inMesh->GetElementNormal(0);
	switch (vertNormal->GetMappingMode()) {
	case FbxGeometryElement::eByControlPoint:
		switch (vertNormal->GetReferenceMode()) {
		case FbxGeometryElement::eDirect: {
			outNormal.x = static_cast<f32>(vertNormal->GetDirectArray().GetAt(ctrlPtIdx).mData[0]);
			outNormal.y = static_cast<f32>(vertNormal->GetDirectArray().GetAt(ctrlPtIdx).mData[1]);
			outNormal.z = static_cast<f32>(vertNormal->GetDirectArray().GetAt(ctrlPtIdx).mData[2]);
		} break;
		case FbxGeometryElement::eIndexToDirect: {
			i32 index = vertNormal->GetIndexArray().GetAt(ctrlPtIdx);
			outNormal.x = static_cast<f32>(vertNormal->GetDirectArray().GetAt(index).mData[0]);
			outNormal.y = static_cast<f32>(vertNormal->GetDirectArray().GetAt(index).mData[1]);
			outNormal.z = static_cast<f32>(vertNormal->GetDirectArray().GetAt(index).mData[2]);
		} break;
		default:
			throw std::exception("Invalid Reference");
		} break;
	case FbxGeometryElement::eByPolygonVertex:
		switch (vertNormal->GetReferenceMode()) {
		case FbxGeometryElement::eDirect: {
			outNormal.x = static_cast<f32>(vertNormal->GetDirectArray().GetAt(vertCounter).mData[0]);
			outNormal.y = static_cast<f32>(vertNormal->GetDirectArray().GetAt(vertCounter).mData[1]);
			outNormal.z = static_cast<f32>(vertNormal->GetDirectArray().GetAt(vertCounter).mData[2]);
		} break;
		case FbxGeometryElement::eIndexToDirect: {
			i32 index = vertNormal->GetIndexArray().GetAt(vertCounter);
			outNormal.x = static_cast<f32>(vertNormal->GetDirectArray().GetAt(index).mData[0]);
			outNormal.y = static_cast<f32>(vertNormal->GetDirectArray().GetAt(index).mData[1]);
			outNormal.z = static_cast<f32>(vertNormal->GetDirectArray().GetAt(index).mData[2]);
		} break;
		default:
			throw std::exception("Invalid Reference");
		} break;
	}
}

static void read_texture_coordinate(FbxMesh* inMesh, i32 ctrlPtIdx, i32 uvIndex, assimp::vector2& outTC) {
	FbxGeometryElementUV* vertUV = inMesh->GetElementUV(0);
	switch (vertUV->GetMappingMode()) {
	case FbxGeometryElement::eByControlPoint:
		switch (vertUV->GetReferenceMode()) {
		case FbxGeometryElement::eDirect: {
			outTC.x = static_cast<f32>(vertUV->GetDirectArray().GetAt(ctrlPtIdx).mData[0]);
			outTC.y = static_cast<f32>(vertUV->GetDirectArray().GetAt(ctrlPtIdx).mData[1]);
		} break;
		case FbxGeometryElement::eIndexToDirect: {
			i32 index = vertUV->GetIndexArray().GetAt(ctrlPtIdx);
			outTC.x = static_cast<f32>(vertUV->GetDirectArray().GetAt(index).mData[0]);
			outTC.y = static_cast<f32>(vertUV->GetDirectArray().GetAt(index).mData[1]);
		} break;
		default:
			throw std::exception("Invalid Reference");
		} break;
	case FbxGeometryElement::eByPolygonVertex:
		switch (vertUV->GetReferenceMode()) {
		case FbxGeometryElement::eDirect:
		case FbxGeometryElement::eIndexToDirect: {
			outTC.x = static_cast<f32>(vertUV->GetDirectArray().GetAt(uvIndex).mData[0]);
			outTC.y = static_cast<f32>(vertUV->GetDirectArray().GetAt(uvIndex).mData[1]);
		} break;
		default:
			throw std::exception("Invalid Reference");
		} break;
	}
}

static void process_ctrl_points(FbxMesh* mesh, assimp::Mesh& model, bool flipUV, bool generateUV) {
	u32 vertexCounter = 0;
	const u32 polyCount = mesh->GetPolygonCount();

	u32 polySizeTotal = 0;
	for (u32 i = 0; i < polyCount; i++) {
		polySizeTotal += mesh->GetPolygonSize(i);
	}
	model.vertices32.reserve(model.vertices32.size() + polySizeTotal);
	vertexCache.reserve(vertexCache.size() + polySizeTotal);

	for (u32 i = 0; i < polyCount; i++) {
		const u32 polySize = mesh->GetPolygonSize(i);

		for (u32 j = 0; j < polySize; j++) {
			i32 ctrlPtIdx = mesh->GetPolygonVertex(i, j);
			if (ctrlPtIdx < 0 || ctrlPtIdx >= mesh->GetControlPointsCount())
				throw std::runtime_error("FBX polygon references an invalid control point.");
			FbxVector4& currCtrlPoint = mesh->mControlPoints[ctrlPtIdx];
			assimp::vector3 normal;
			if (mesh->GetElementNormalCount() > 0) read_normal(mesh, ctrlPtIdx, vertexCounter, normal);
			assimp::vector2 uv;
			if (mesh->GetElementUVCount() > 0) read_texture_coordinate(mesh, ctrlPtIdx, mesh->GetTextureUVIndex(i, j), uv);
			if (generateUV && mesh->GetElementUVCount() == 0) uv = assimp::vector2(static_cast<f32>(currCtrlPoint.mData[0]), static_cast<f32>(currCtrlPoint.mData[1]));
			if (flipUV) uv.y *= -1.0f;
			assimp::vector3 position(
				static_cast<f32>(currCtrlPoint.mData[0]),
				static_cast<f32>(currCtrlPoint.mData[1]),
				static_cast<f32>(currCtrlPoint.mData[2]));

			assimp::VertexP3N3T2 vert = { position, normal, uv };

			if (auto it = vertexCache.find(vert); it != vertexCache.end()) {
				model.indexBuffer32.push_back(it->second);
			}
			else {
				u32 idx = static_cast<u32>(model.vertices32.size());
				vertexCache[vert] = idx;
				model.vertices32.push_back(std::move(vert));
				model.indexBuffer32.push_back(idx);
			}
			
			++vertexCounter;
		}
	}
}

errlib::Error assimp::FbxImporter::InternReadFile(const string& file, Collection* scene) {
	std::unique_ptr<fbxsdk::FbxManager, FbxManagerDeleter> manager(fbxsdk::FbxManager::Create());
	if (!manager) return errlib::Error::FromCode(errlib::ErrorCode_outOfMemory, "Could not create the FBX manager.");
	fbxsdk::FbxManager* lSdkManager = manager.get();
	fbxsdk::FbxIOSettings* ios = fbxsdk::FbxIOSettings::Create(lSdkManager, IOSROOT);
	if (!ios) return errlib::Error::FromCode(errlib::ErrorCode_outOfMemory, "Could not create FBX IO settings.");
	lSdkManager->SetIOSettings(ios);

	fbxsdk::FbxImporter* lImporter = fbxsdk::FbxImporter::Create(lSdkManager, "");
	if (!lImporter) return errlib::Error::FromCode(errlib::ErrorCode_outOfMemory, "Could not create the FBX importer.");
	if (!lImporter->Initialize(file.c_str(), -1, lSdkManager->GetIOSettings())) {
		return errlib::Error::FromCode(errlib::ErrorCode_invalidAsset, "FBX initialization failed: " + string(lImporter->GetStatus().GetErrorString()));
	}

	fbxsdk::FbxScene* lScene = fbxsdk::FbxScene::Create(lSdkManager, "myScene");
	if (!lScene) return errlib::Error::FromCode(errlib::ErrorCode_outOfMemory, "Could not create the FBX scene.");
	if (!lImporter->Import(lScene))
		return errlib::Error::FromCode(errlib::ErrorCode_invalidAsset, "FBX import failed: " + string(lImporter->GetStatus().GetErrorString()));
	lImporter->Destroy();

	fbxsdk::FbxGeometryConverter converter(lSdkManager);
	converter.Triangulate(lScene, true); // ensure all meshes are triangulated

	std::vector<fbxsdk::FbxNode*> nodeMap;
	auto findNode = [&](fbxsdk::FbxNode* node) {
		for (i32 i = 0; i < nodeMap.size(); ++i) { if (nodeMap[i] == node) return i; }
		return -1;
		};

#pragma region SPLIT MESHES BY MATERIAL

	const i32 geometryCount = lScene->GetGeometryCount();
	std::vector<fbxsdk::FbxMesh*> primaryMeshes;

	scene->models.reserve(geometryCount);
	for (i32 i = 0; i < geometryCount; ++i) {
		if (fbxsdk::FbxGeometry* geo = lScene->GetGeometry(i); geo && geo->GetAttributeType() == fbxsdk::FbxNodeAttribute::eMesh) {
			if (fbxsdk::FbxMesh* mesh = fbxsdk::FbxCast<fbxsdk::FbxMesh>(geo); mesh) {
				fbxsdk::FbxNode* node = mesh->GetNode();
				if (!node) return errlib::Error::FromCode(errlib::ErrorCode_invalidAsset, "FBX mesh is missing its scene node.");
				if (findNode(node) < 0) {
					nodeMap.push_back(node);
					scene->models.emplace_back();
				}
				primaryMeshes.push_back(mesh);
			}
		}
	}
	scene->models.shrink_to_fit();

	for (fbxsdk::FbxMesh* mesh : primaryMeshes) { converter.SplitMeshPerMaterial(mesh, true); } // splits a mesh into sub meshes based on material idx
	for (u32 i = 0; i < nodeMap.size(); ++i) {
		u32 meshCount = 0;
		for (i32 j = 0; j < nodeMap[i]->GetNodeAttributeCount(); ++j) {
			fbxsdk::FbxNodeAttribute* attribute = nodeMap[i]->GetNodeAttributeByIndex(j);
			if (attribute && attribute->GetAttributeType() == fbxsdk::FbxNodeAttribute::eMesh) meshCount++;
		}
		scene->models[i].subMeshes.reserve(meshCount);
		scene->models[i].materialCount = static_cast<u8>(std::min<u32>(std::max<u32>(1, meshCount), std::numeric_limits<u8>::max()));
		scene->models[i].name = nodeMap[i]->GetNameOnly();
	}

#pragma endregion

	// must edit to enable submeshing per fbx model

	const bool useTangents = (ppFlags & PostProcessingFlags_importTangents) || (ppFlags & PostProcessingFlags_pbrImport);
	const bool generateUV = (ppFlags & PostProcessingFlags_generateMissingUVs) || useTangents;
	const i32 geometryCountPS = lScene->GetGeometryCount();
	for (i32 i = 0; i < geometryCountPS; ++i) {
		fbxsdk::FbxMesh* mesh = fbxsdk::FbxCast<fbxsdk::FbxMesh>(lScene->GetGeometry(i));
		if (!mesh) continue;

		const i32 idx = findNode(mesh->GetNode());
		if (idx < 0 || static_cast<usize>(idx) >= scene->models.size())
			return errlib::Error::FromCode(errlib::ErrorCode_invalidAsset, "FBX mesh is not associated with an imported node.");
		Mesh& model = scene->models[static_cast<usize>(idx)];

		SubMesh sm;
		sm.indexOffset = static_cast<u32>(model.indexBuffer32.size());

		vertexCache.clear();

		process_ctrl_points(mesh, model, (ppFlags & PostProcessingFlags_flipUVs) != 0, generateUV);

		sm.indexCount = static_cast<u32>(model.indexBuffer32.size()) - sm.indexOffset;
		calculate_submesh_bounds(sm, model.vertices32, model.indexBuffer32);
		model.subMeshes.push_back(std::move(sm));
	}

	const bool generateNormals = (ppFlags & PostProcessingFlags_generateMissingNormals) || useTangents;
	for (Mesh& model : scene->models) {
		if (ppFlags & PostProcessingFlags_flipWindingOrder) {
			for (usize i = 0; i + 2 < model.indexBuffer32.size(); i += 3)
				std::swap(model.indexBuffer32[i + 1], model.indexBuffer32[i + 2]);
		}
		if (generateNormals) {
			for (usize i = 0; i + 2 < model.indexBuffer32.size(); i += 3) {
				const u32 i1 = model.indexBuffer32[i], i2 = model.indexBuffer32[i + 1], i3 = model.indexBuffer32[i + 2];
				if (i1 >= model.vertices32.size() || i2 >= model.vertices32.size() || i3 >= model.vertices32.size())
					return errlib::Error::FromCode(errlib::ErrorCode_invalidAsset, "FBX index exceeds the vertex buffer.");
				const vector3& a = model.vertices32[i1].position;
				const vector3& b = model.vertices32[i2].position;
				const vector3& c = model.vertices32[i3].position;
				const vector3 normal = normalize(cross(b - a, c - a));
				if (model.vertices32[i1].normal == vector3()) model.vertices32[i1].normal = normal;
				if (model.vertices32[i2].normal == vector3()) model.vertices32[i2].normal = normal;
				if (model.vertices32[i3].normal == vector3()) model.vertices32[i3].normal = normal;
			}
		}
		if (useTangents) {
			model.vertices48.reserve(model.vertices32.size());
			for (const VertexP3N3T2& vertex : model.vertices32) model.vertices48.emplace_back(vertex);
			model.vertices32.clear();
			std::vector<vector3> tangent(model.vertices48.size()), bitangent(model.vertices48.size());

			for (usize i = 0; i + 2 < model.indexBuffer32.size(); i += 3) {
				const u32 i1 = model.indexBuffer32[i], i2 = model.indexBuffer32[i + 1], i3 = model.indexBuffer32[i + 2];
				if (i1 >= model.vertices48.size() || i2 >= model.vertices48.size() || i3 >= model.vertices48.size())
					return errlib::Error::FromCode(errlib::ErrorCode_invalidAsset, "FBX index exceeds the vertex buffer.");
				const VertexP3N3T2T4& a = model.vertices48[i1];
				const VertexP3N3T2T4& b = model.vertices48[i2];
				const VertexP3N3T2T4& c = model.vertices48[i3];

				const f32 x1 = b.position.x - a.position.x, x2 = c.position.x - a.position.x;
				const f32 y1 = b.position.y - a.position.y, y2 = c.position.y - a.position.y;
				const f32 z1 = b.position.z - a.position.z, z2 = c.position.z - a.position.z;
				const f32 s1 = b.texCoord.x - a.texCoord.x, s2 = c.texCoord.x - a.texCoord.x;
				const f32 t1 = b.texCoord.y - a.texCoord.y, t2 = c.texCoord.y - a.texCoord.y;

				f32 r = s1 * t2 - s2 * t1;
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

				tangent[i1] += sdir; tangent[i2] += sdir; tangent[i3] += sdir;
				bitangent[i1] += tdir; bitangent[i2] += tdir; bitangent[i3] += tdir;
			}

			for (usize i = 0; i < model.vertices48.size(); ++i) {
				const vector3& norm = model.vertices48[i].normal;
				const vector3& tan = tangent[i];
				const vector3& bit = bitangent[i];

				const vector3 tangentXYZ = normalize(tan - norm * dot(norm, tan));
				const f32 handedness = dot(cross(norm, tan), bit) < 0.f ? -1.f : 1.f;
				model.vertices48[i].tangent = { tangentXYZ.x, tangentXYZ.y, tangentXYZ.z, handedness };
			}
		}
	}

	scene->name = std::filesystem::path(file).filename().stem().string();
	sourcePath = std::filesystem::absolute(file).string();

	return errlib::Error{};
}

#endif