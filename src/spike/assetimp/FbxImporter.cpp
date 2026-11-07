#if defined(_USING_FBX)

#include "spike/assetimp/FbxImporter.h"

#include <string.h>
#include <exception>
#include <filesystem>
#include <utility>
#include <vector>

#include "spike/warning.h"
#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"
#include "spike/error.hpp"
#include "spike/debugl/debug.hpp"
#include "spike/sml/vector2.h"
#include "spike/sml/vector3.h"
#include "spike/sml/sml_core.h"

DISABLE_WARNING_PUSH
DISABLE_WARNING(4099)
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

#include "spike/assetimp/aimesh.h"
#include "spike/assetimp/BaseImporter.h"
#include "spike/assetimp/PostProcessing.h"
#include "spike/assetimp/aicollection.h"

DISABLE_WARNING_UNUSED_VARIABLE

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

static void read_normal(FbxMesh* inMesh, i32 ctrlPtIdx, i32 vertCounter, sml::vector3& outNormal) {
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

static void read_texture_coordinate(FbxMesh* inMesh, i32 ctrlPtIdx, i32 uvIndex, sml::vector2& outTC) {
	if (inMesh->GetElementUVCount() < 1) throw std::exception("Invalid UV Number");

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

static void process_ctrl_points(FbxMesh* mesh, assimp::Model& model) {
	// removed poly stuff as mesh pre tri'd

	//std::vector<Poly> faces;

	u32 vertexCounter = 0;
	const u32 polyCount = mesh->GetPolygonCount();
	//faces.reserve(polyCount);

	u32 polySizeTotal = 0;
	for (u32 i = 0; i < polyCount; i++) {
		polySizeTotal += mesh->GetPolygonSize(i);
	}
	model.vertices32.reserve(model.vertices32.size() + polySizeTotal);
	vertexCache.reserve(vertexCache.size() + polySizeTotal);

	for (u32 i = 0; i < polyCount; i++) {
		const u32 polySize = mesh->GetPolygonSize(i);
		//Poly face;
		//face.indices.reserve(polySize);

		for (u32 j = 0; j < polySize; j++) {
			i32 ctrlPtIdx = mesh->GetPolygonVertex(i, j);
			FbxVector4& currCtrlPoint = mesh->mControlPoints[ctrlPtIdx];
			sml::vector3 normal; read_normal(mesh, ctrlPtIdx, vertexCounter, normal);
			// edit in future to allow many UV layers (8 uv layers) and tangents + bitangents
			sml::vector2 uv; read_texture_coordinate(mesh, ctrlPtIdx, mesh->GetTextureUVIndex(i, j), uv);
			sml::vector3 position(
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
		//faces.emplace_back(face);
	}

	// turn faces into index buffers
}

errlib::Error assimp::FbxImporter::InternReadFile(const string& file, Collection* scene) {
	fbxsdk::FbxManager* lSdkManager = fbxsdk::FbxManager::Create();
	fbxsdk::FbxIOSettings* ios = fbxsdk::FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	fbxsdk::FbxImporter* lImporter = fbxsdk::FbxImporter::Create(lSdkManager, "");
	if (!lImporter->Initialize(file.c_str(), -1, lSdkManager->GetIOSettings())) {
		Debug::LogError("Call to FbxImporter::Initialize() failed. Error {}", string(lImporter->GetStatus().GetErrorString()));
		return errlib::Error(errlib::ErrorCode_unknown, "FbxImporter::Initialize() failed.");
	}

	fbxsdk::FbxScene* lScene = fbxsdk::FbxScene::Create(lSdkManager, "myScene");
	lImporter->Import(lScene);
	lImporter->Destroy();

	fbxsdk::FbxGeometryConverter converter(lSdkManager);
	converter.Triangulate(lScene, true); // ensure all meshes are triangulated

	std::vector<fbxsdk::FbxNode*> nodeMap; // holds all nodes with a mesh in the scene

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
			if (fbxsdk::FbxMesh* mesh = fbxsdk::FbxCast<fbxsdk::FbxMesh>(geo); geo) {
				primaryMeshes.push_back(mesh); scene->models.emplace_back();
				nodeMap.push_back(mesh->GetNode());
			}
		}
	}
	scene->models.shrink_to_fit();

	for (fbxsdk::FbxMesh* mesh : primaryMeshes) { converter.SplitMeshPerMaterial(mesh, true); } // splits a mesh into sub meshes based on material idx
	for (u32 i = 0; i < nodeMap.size(); ++i) {
		u32 meshCount = 0;
		for (i32 j = 0; j < nodeMap[i]->GetNodeAttributeCount(); ++j) {
			if (nodeMap[i]->GetNodeAttributeByIndex(j)->GetAttributeType() == fbxsdk::FbxNodeAttribute::eMesh) meshCount++;
		}
		scene->models[i].subMeshes.reserve(meshCount);
		scene->models[i].materialCount = sml::max(1, meshCount);
		scene->models[i].name = primaryMeshes[i]->GetNameOnly();
	}

#pragma endregion

	// must edit to enable submeshing per fbx model

	const i32 geometryCountPS = lScene->GetGeometryCount();
	for (i32 i = 0; i < geometryCountPS; ++i) {
		fbxsdk::FbxMesh* mesh = fbxsdk::FbxCast<fbxsdk::FbxMesh>(lScene->GetGeometry(i));
		if (!mesh) continue;

		const i32 idx = findNode(mesh->GetNode());
		Model& model = scene->models[idx];

		SubMesh sm;
		sm.indexOffset = static_cast<u32>(model.indexBuffer32.size());

		vertexCache.clear();

		process_ctrl_points(mesh, model);

		sm.indexCount = static_cast<u32>(model.indexBuffer32.size()) - sm.indexOffset;
		calculate_submesh_bounds(sm, model.vertices32, model.indexBuffer32);
		model.subMeshes.push_back(std::move(sm));
	}

	// can parse hierarchy once done but not important

	lSdkManager->Destroy();

	scene->name = std::filesystem::path(file).filename().stem().string();
	sourcePath = std::filesystem::absolute(file).string();

	return errlib::Error{};
}

#endif