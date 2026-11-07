#include "spike/assetimp/GltfImporter.h"

#include <fstream>
#include <string.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <concepts>
#include <deque>
#include <exception>
#include <filesystem>
#include <forward_list>
#include <functional>
#include <ios>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <system_error>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "spike/nivector.h"
#include "spike/warning.h"
#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"
#include "spike/sml/sml_core.h"
#include "spike/sml/vector2.h"
#include "spike/sml/vector3.h"
#include "spike/sml/vector4.h"
#include "spike/error.hpp"
#include "spike/debugl/debug.hpp"

DISABLE_WARNING_PUSH
DISABLE_WARNING(4099)
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>
DISABLE_WARNING_POP

#include "spike/assetimp/PostProcessing.h"
#include "spike/assetimp/aicollection.h"
#include "spike/assetimp/aimesh.h"
#include "spike/assetimp/BaseImporter.h"

DISABLE_WARNING_UNUSED_VARIABLE

#pragma region GLTF MACROS

inline constexpr i32 kGltfTargetArrayBuffer = 34962;
inline constexpr i32 kGltfTargetElementArrayBuffer = 34963;

inline constexpr i32 kGltfComponentTypeByte = 5120;
inline constexpr i32 kGltfComponentTypeUByte = 5121;
inline constexpr i32 kGltfComponentTypeShort = 5122;
inline constexpr i32 kGltfComponentTypeUShort = 5123;
inline constexpr i32 kGltfComponentTypeInt = 5124;
inline constexpr i32 kGltfComponentTypeUInt = 5125;
inline constexpr i32 kGltfComponentTypeFloat = 5126;
inline constexpr i32 kGltfComponentTypeDouble = 5127;

inline constexpr i32 kGltfParameterTypeByte = 5120;
inline constexpr i32 kGltfParameterTypeUByte = 5121;
inline constexpr i32 kGltfParameterTypeShort = 5122;
inline constexpr i32 kGltfParameterTypeUShort = 5123;
inline constexpr i32 kGltfParameterTypeInt = 5124;
inline constexpr i32 kGltfParameterTypeUInt = 5125;
inline constexpr i32 kGltfParameterTypeFloat = 5126;
inline constexpr i32 kGltfParameterTypeDouble = 5127;

inline constexpr i32 kGltfParameterTypeFloatVec2 = 35664;
inline constexpr i32 kGltfParameterTypeFloatVec3 = 35665;
inline constexpr i32 kGltfParameterTypeFloatVec4 = 35666;

#pragma endregion

struct GLTFData {
	struct Collection {
		string name = "";
		std::vector<u32> nodes;
	};
	std::vector<Collection> scenes;

	struct Node {
		u32 mesh = 0;
		string name = "";
	};
	std::vector<Node> nodes;

	struct Mesh {
		string name = "";
		struct Primitive {
			umap<string, u32> attributeIndices;
			u32 indices = 0;
			u32 material = 0;
			u32 mode = 4;
		};
		std::vector<Primitive> primitives;
	};
	std::vector<Mesh> meshes;

	struct Accessor {
		ushort bufferView = 0;
		ushort componentType = 0;
		u32 count = 0;
		std::vector<f32> max;
		std::vector<f32> min;
		string type = "";
		u32 byteOffset = 0;
	};
	std::vector<Accessor> accessors;

	struct BufferView {
		u32 buffer = 0;
		u32 byteLength = 0;
		u32 byteOffset = 0;
		u32 target = 0;
	};
	std::vector<BufferView> bufferViews;

	struct Buffer {
		u32 byteLength = 0;
		string uri = "";
	};
	std::vector<Buffer> buffers;

	std::vector<niVector<u8>> bufferData;
};

static bool parse_variable(Json::Value& val, Json::ValueIterator& it, string attribute) {
	if (val = (*it).get(attribute, Json::Value()); !val.isNull()) return true;
	else return false;
}

static bool parse_variable(Json::Value& val, Json::Value& it, string attribute) {
	if (val = it.get(attribute, Json::Value()); !val.isNull()) return true;
	else return false;
}

#pragma region LOADERS

static bool LoadScenes(GLTFData& gltf, Json::Value& json) {
	Json::Value node;
	if (!parse_variable(node, json, "scenes")) return false;

	for (Json::ValueIterator it = node.begin(); it != node.end(); it++) {
		GLTFData::Collection sc;
		Json::Value val;
		if (parse_variable(val, it, "name")) sc.name = val.asString();

		Json::Value nodes;
		if (parse_variable(nodes, it, "nodes")) {
			const u32 n = nodes.size();
			for (u32 i = 0; i < n; i++) sc.nodes.push_back(nodes[i].asInt());
		}

		gltf.scenes.push_back(sc);
	}

	return true;
}

static bool LoadNodes(GLTFData& gltf, Json::Value& json) {
	Json::Value node;
	if (!parse_variable(node, json, "nodes")) return false;

	for (Json::ValueIterator it = node.begin(); it != node.end(); it++) {
		GLTFData::Node n;
		Json::Value val;
		if (parse_variable(val, it, "mesh")) n.mesh = val.asInt();
		if (parse_variable(val, it, "name")) n.name = val.asString();
		gltf.nodes.push_back(n);
	}

	return true;
}

static bool LoadMeshes(GLTFData& gltf, Json::Value& json) {
	Json::Value node;
	if (!parse_variable(node, json, "meshes")) return false;

	for (Json::ValueIterator it = node.begin(); it != node.end(); it++) {
		GLTFData::Mesh m;
		Json::Value val;
		if (parse_variable(val, it, "name")) m.name = val.asString();

		Json::Value prim;
		if (parse_variable(prim, it, "primitives")) {
			for (Json::ValueIterator it2 = prim.begin(); it2 != prim.end(); it2++) {
				GLTFData::Mesh::Primitive p;

				Json::Value val2;
				if (parse_variable(val2, it2, "indices")) p.indices = val2.asInt();
				if (parse_variable(val2, it2, "mode")) p.mode = val2.asInt();
				if (parse_variable(val2, it2, "material")) p.material = val2.asInt();

				Json::Value attr;
				if (parse_variable(attr, it2, "attributes")) {
					std::vector<string> attrNames = attr.getMemberNames();

					for (string& str : attrNames) p.attributeIndices[str] = attr[str].asInt();
				}

				m.primitives.push_back(p);
			}
		}

		gltf.meshes.push_back(m);
	}

	return true;
}

static bool LoadAccessors(GLTFData& gltf, Json::Value& json) {
	Json::Value node;
	if (!parse_variable(node, json, "accessors")) return false;

	for (Json::ValueIterator it = node.begin(); it != node.end(); it++) {
		GLTFData::Accessor acc;
		Json::Value val;
		if (parse_variable(val, it, "bufferView")) acc.bufferView = val.asInt();
		if (parse_variable(val, it, "componentType")) acc.componentType = val.asInt();
		if (parse_variable(val, it, "count")) acc.count = val.asInt();
		if (parse_variable(val, it, "type")) acc.type = val.asString();

		// dont need min / max

		gltf.accessors.push_back(acc);
	}

	return true;
}

static bool LoadBufferViews(GLTFData& gltf, Json::Value& json) {
	Json::Value node;
	if (!parse_variable(node, json, "bufferViews")) return false;

	for (Json::ValueIterator it = node.begin(); it != node.end(); it++) {
		GLTFData::BufferView bv;
		Json::Value val;
		if (parse_variable(val, it, "buffer")) bv.buffer = val.asInt();
		if (parse_variable(val, it, "byteLength")) bv.byteLength = val.asInt();
		if (parse_variable(val, it, "byteOffset")) bv.byteOffset = val.asInt();
		if (parse_variable(val, it, "target")) bv.target = val.asInt();
		gltf.bufferViews.push_back(bv);
	}

	return true;
}

static bool LoadBuffers(GLTFData& gltf, Json::Value& json) {
	Json::Value node;
	if (!parse_variable(node, json, "buffers")) return false;

	for (Json::ValueIterator it = node.begin(); it != node.end(); it++) {
		GLTFData::Buffer buf;
		Json::Value val;
		if (parse_variable(val, it, "byteLength")) buf.byteLength = val.asInt();
		if (parse_variable(val, it, "uri")) buf.uri = val.asString();
		gltf.buffers.push_back(buf);
	}

	return true;
}

static niVector<u8> LoadBuffer(const string& path) {
	usize binLength = 0;
	niVector<u8> binData;

	try {
		std::ifstream binFile = std::ifstream(path, std::ios::binary | std::ios::ate);

		if (!binFile.is_open()) {
			throw std::ifstream::failure("Could not find bin file at path", { 1, std::iostream_category() });
		}

		binLength = binFile.tellg();
		binFile.seekg(0);

		binData.resize(binLength);
		binFile.read(reinterpret_cast<char*>(binData.data()), binLength);
		binFile.close();
	}
	catch (std::ifstream::failure e) {
		Debug::LogError("Error reading bin file at path: {} \nError: {}, Code: {}: {}", path, e.what(), e.code().value(), e.code().message());
		return {};
	}
	return binData;
}

#pragma endregion

static void load_gltf(Json::Value& json, GLTFData& gltf, const string& path) {
	LoadScenes(gltf, json);
	LoadNodes(gltf, json);
	LoadMeshes(gltf, json);
	LoadAccessors(gltf, json);
	LoadBufferViews(gltf, json);
	LoadBuffers(gltf, json);

	string binPath = path.substr(0, path.find_last_of("\\/") + 1); // assume bin in same dir as gltf
	gltf.bufferData.reserve(gltf.buffers.size());
	for (u32 i = 0; i < gltf.buffers.size(); i++) {
		string bfp = binPath + gltf.buffers[i].uri;
		gltf.bufferData.push_back(LoadBuffer(bfp));
	}
}

static void load_glb(std::ifstream& inFile, GLTFData& gltf, const string& path) {
	inFile.seekg(12);
	u32 jsonSize = 0;
	inFile.read(reinterpret_cast<char*>(&jsonSize), sizeof(u32));

	string jsonStr;
	jsonStr.resize(jsonSize);
	inFile.seekg(20);
	inFile.read(jsonStr.data(), jsonSize);

	Json::Reader reader;
	Json::Value json;
	if (!reader.parse(jsonStr, json)) {
		Debug::LogError("Problem parsing assetData: {}", jsonStr);
		return;
	}
	else if (json.isNull()) {
		Debug::LogError("Json Root is NULL");
		return;
	}

	LoadScenes(gltf, json);
	LoadNodes(gltf, json);
	LoadMeshes(gltf, json);
	LoadAccessors(gltf, json);
	LoadBufferViews(gltf, json);
	LoadBuffers(gltf, json);

	gltf.bufferData.resize(1);

	u32 binLength = 0;
	inFile.read(reinterpret_cast<char*>(&binLength), sizeof(binLength));
	gltf.bufferData[0].reserve(binLength);
	inFile.seekg(sizeof(u32), std::ios_base::cur);
	inFile.read(reinterpret_cast<char*>(gltf.bufferData[0].data()), binLength);
}

#pragma region helpers

static u32 get_component_size(const ushort& type) {
	switch (type) {
	case kGltfComponentTypeByte:
	case kGltfComponentTypeUByte:
		return 1;
		break;
	case kGltfComponentTypeShort:
	case kGltfComponentTypeUShort:
		return 2;
		break;
	case kGltfComponentTypeInt:
	case kGltfComponentTypeUInt:
		return 4;
		break;
	case kGltfComponentTypeFloat:
		return 4;
		break;
	case kGltfComponentTypeDouble:
		return 8;
		break;
	default:
		return 0;
		break;
	}
}

static u32 get_component_count(const string& type) {
	if (type == "SCALAR") return 1;
	else if (type == "VEC2") return 2;
	else if (type == "VEC3") return 3;
	else if (type == "VEC4") return 4;
	else if (type == "MAT2") return 4;
	else if (type == "MAT3") return 9;
	else if (type == "MAT4") return 16;
	Debug::LogError("Invalid Accessor Type: {}", type);
	return 0;
}

template<typename T>
concept arithmetic = std::integral<T> or std::floating_point<T>;

template<typename T>
	requires arithmetic<T>
void read_to_nivector(const niVector<u8>& bufferData, const u32 byteOffset, const u32 count, const u32 componentSize, niVector<T>& out) {
	static_assert(std::is_arithmetic<T>::value, "T must be numeric");
	out.resize(count);
	const u8* base = bufferData.data() + byteOffset;

	if (componentSize == sizeof(T)) { // safe on little-endian PFs where GLTF buffer == host endianness
		std::memcpy(out.data(), base, (usize)count * componentSize);
		return;
	}

	switch (componentSize) {
	case 1: {
		for (u32 i = 0; i < count; ++i) {
			const u8* src = base + (usize)i * componentSize;
			u8 tmp = 0;
			std::memcpy(&tmp, src, 1);
			out[i] = static_cast<T>(tmp);
		}
	} break;
	case 2: {
		for (u32 i = 0; i < count; ++i) {
			const u8* src = base + (usize)i * componentSize;
			u16 tmp = 0;
			std::memcpy(&tmp, src, 2);
			out[i] = static_cast<T>(tmp);
		}
	} break;
	case 4: {
		for (u32 i = 0; i < count; ++i) {
			const u8* src = base + (usize)i * componentSize;
			u32 tmp = 0;
			std::memcpy(&tmp, src, 4);
			out[i] = static_cast<T>(tmp);
		}
	} break;
	case 8: {
		for (u32 i = 0; i < count; ++i) {
			const u8* src = base + (usize)i * componentSize;
			u64 tmp = 0;
			std::memcpy(&tmp, src, 8);
			out[i] = static_cast<T>(tmp);
		}
	} break;
	default:
		Debug::LogError("Unsupported index component size: {}", componentSize);
		for (u32 i = 0; i < count; ++i) out[i] = 0;
		return;
	}
}

#pragma endregion

static inline const GLTFData::Accessor* get_accessor(const GLTFData& gltf, const GLTFData::Mesh::Primitive& primitiveData, const string& name) {
	if (auto it = primitiveData.attributeIndices.find(name); it != primitiveData.attributeIndices.end()) return &gltf.accessors[it->second];
	return nullptr;
}

static inline const GLTFData::BufferView* get_buffer_views(const GLTFData& gltf, const GLTFData::Accessor* acc) {
	if (acc) return &gltf.bufferViews[acc->bufferView];
	return nullptr;
}

static inline const niVector<u8>* get_buffer_data(const GLTFData& gltf, const GLTFData::BufferView* bv) {
	if (bv) return &gltf.bufferData[bv->buffer];
	return nullptr;
}

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

static void construct_mesh(const GLTFData& gltf, const u32 idx, const string& path, assimp::Collection* scene, const u32 ppSteps) {
	assimp::Model model = assimp::Model();

	const GLTFData::Mesh& meshData = gltf.meshes[idx];
	model.name = meshData.name;

	const u32 subMeshCount = static_cast<const u32>(meshData.primitives.size());
	std::set<u32> matz;

	model.subMeshes.reserve(subMeshCount);

	for (u32 i = 0; i < subMeshCount; i++) {
		const GLTFData::Mesh::Primitive& primitiveData = meshData.primitives[i];

		const GLTFData::Accessor& idxAcc = gltf.accessors[primitiveData.indices];
		const GLTFData::BufferView& idxBv = gltf.bufferViews[idxAcc.bufferView];
		const u32 idxCompSize = get_component_size(idxAcc.componentType);
		const niVector<u8>& idxBufferD = gltf.bufferData[idxBv.buffer];

		const GLTFData::Accessor* posAcc = get_accessor(gltf, primitiveData, "POSITION");
		const GLTFData::Accessor* norAcc = get_accessor(gltf, primitiveData, "NORMAL");
		const GLTFData::Accessor* tcoAcc = get_accessor(gltf, primitiveData, "TEXCOORD_0");
		const GLTFData::Accessor* tanAcc = ((ppSteps & assimp::PostProcessingFlags_importTangents || ppSteps & assimp::PostProcessingFlags_pbrImport) ? get_accessor(gltf, primitiveData, "TANGENT") : nullptr);

		const GLTFData::BufferView* posBv = get_buffer_views(gltf, posAcc);
		const GLTFData::BufferView* norBv = get_buffer_views(gltf, norAcc);
		const GLTFData::BufferView* tcoBv = get_buffer_views(gltf, tcoAcc);
		const GLTFData::BufferView* tanBv = get_buffer_views(gltf, tanAcc);

		const niVector<u8>* posBufferD = get_buffer_data(gltf, posBv);
		const niVector<u8>* norBufferD = get_buffer_data(gltf, norBv);
		const niVector<u8>* tcoBufferD = get_buffer_data(gltf, tcoBv);
		const niVector<u8>* tanBufferD = get_buffer_data(gltf, tanBv);

		const u32 vertexCount = posAcc ? posAcc->count : 0;
		const u32 indexCount = idxAcc.count;

		f32* posBuffer = (posBufferD && posBv) ? (f32*)(posBufferD->data() + posBv->byteOffset) : nullptr;
		f32* norBuffer = (norBufferD && norBv) ? (f32*)(norBufferD->data() + norBv->byteOffset) : nullptr;
		f32* tcoBuffer = (tcoBufferD && tcoBv) ? (f32*)(tcoBufferD->data() + tcoBv->byteOffset) : nullptr;
		f32* tanBuffer = (tanBufferD && tanBv) ? (f32*)(tanBufferD->data() + tanBv->byteOffset) : nullptr;

		const u32 startIdx = static_cast<const u32>(std::max(model.indexBuffer16.size(), model.indexBuffer32.size()));

		if (ppSteps & assimp::PostProcessingFlags_importTangents || ppSteps & assimp::PostProcessingFlags_pbrImport) {
			const usize arrSize = model.vertices48.size();
			model.vertices48.reserve(arrSize + vertexCount);

			for (u32 j = 0; j < vertexCount; j++) {
				model.vertices48.emplace_back(
					sml::vector3(posBuffer[j * 3], posBuffer[j * 3 + 1], posBuffer[j * 3 + 2]),
					norBuffer ? sml::vector3(norBuffer[j * 3], norBuffer[j * 3 + 1], norBuffer[j * 3 + 2]) : sml::vector3(),
					tcoBuffer ? sml::vector2(tcoBuffer[j * 2], ppSteps & assimp::PostProcessingFlags_flipUVs ? tcoBuffer[j * 2 + 1] * -1 : tcoBuffer[j * 2 + 1]) : sml::vector2()
				);

				if (tanBuffer && !(ppSteps & assimp::PostProcessingFlags_flipUVs)) model.vertices48[arrSize + j].tangent = sml::vector4{ tanBuffer[j * 3], tanBuffer[j * 3 + 1], tanBuffer[j * 3 + 2], tanBuffer[j * 3 + 3] };
			}

			if (!tanBuffer || ppSteps & assimp::PostProcessingFlags_flipUVs) {
				if (model.indexBuffer16.empty()) generate_tangents(model, model.indexBuffer32);
				else generate_tangents(model, model.indexBuffer16);
			}

			const usize iarrSize = model.indexBuffer32.size();

			niVector<u32> iVec;
			read_to_nivector(idxBufferD, idxBv.byteOffset, indexCount, idxCompSize, iVec);

			for (auto& i : iVec) {
				i += static_cast<u32>(arrSize); // ensure primitive indices in global array start from correct idx
			}

			model.indexBuffer32.reserve(iarrSize + indexCount);
			model.indexBuffer32.insert(model.indexBuffer32.end(), iVec.begin(), iVec.end());
		}
		else {
			const usize arrSize = model.vertices32.size();
			model.vertices32.reserve(arrSize + vertexCount);

			for (u32 j = 0; j < vertexCount; j++) {
				model.vertices32.emplace_back(
					sml::vector3(posBuffer[j * 3], posBuffer[j * 3 + 1], posBuffer[j * 3 + 2]),
					norBuffer ? sml::vector3(norBuffer[j * 3], norBuffer[j * 3 + 1], norBuffer[j * 3 + 2]) : sml::vector3(),
					tcoBuffer ? sml::vector2(tcoBuffer[j * 2], ppSteps & assimp::PostProcessingFlags_flipUVs ? tcoBuffer[j * 2 + 1] * -1 : tcoBuffer[j * 2 + 1]) : sml::vector2()
				);
			}

			const usize iarrSize = model.indexBuffer32.size();

			niVector<u32> iVec;
			read_to_nivector(idxBufferD, idxBv.byteOffset, indexCount, idxCompSize, iVec);

			for (auto& i : iVec) {
				i += static_cast<u32>(arrSize); // ensure primitive indices in global array start from correct idx
			}

			model.indexBuffer32.reserve(iarrSize + indexCount);
			model.indexBuffer32.insert(model.indexBuffer32.end(), iVec.begin(), iVec.end());
		}

		assimp::SubMesh sm;
		sm.indexOffset = startIdx;
		sm.indexCount = static_cast<const u32>(std::max(model.indexBuffer16.size(), model.indexBuffer32.size())) - startIdx;
		sm.materialIdx = primitiveData.material;
		matz.insert(primitiveData.material);
		if (ppSteps & assimp::PostProcessingFlags_importTangents || ppSteps & assimp::PostProcessingFlags_pbrImport) {
			calculate_submesh_bounds(sm, model.vertices48, model.indexBuffer32);
		}
		else {
			calculate_submesh_bounds(sm, model.vertices32, model.indexBuffer32);
		}

		model.subMeshes.push_back(std::move(sm));
	}

	if (ppSteps & assimp::PostProcessingFlags_flipWindingOrder) {
		if (model.indexBuffer16.empty()) flip_winding_order(model.indexBuffer32);
		else flip_winding_order(model.indexBuffer16);
	}

	model.materialCount = sml::max(1, static_cast<u32>(matz.size()));

	scene->models.push_back(std::move(model));
}

static void construct_meshes(const GLTFData& gltf, const string& path, assimp::Collection* scene, const u32 ppSteps) {
	const u32 meshCount = static_cast<const u32>(gltf.meshes.size());
	for (u32 i = 0; i < meshCount; i++) {
		construct_mesh(gltf, i, path, scene, ppSteps);
	}
}

assimp::GltfImporter::GltfImporter() : BaseImporter(), sourcePath() {
	extensions = { ".gltf", ".glb" };
}

assimp::GltfImporter::~GltfImporter() {}

errlib::Error assimp::GltfImporter::InternReadFile(const string& file, Collection* scene) {
	std::ifstream fileStream;
	fileStream.exceptions(std::ifstream::badbit);

	try {
		fileStream.open(file, std::ios::in | std::ios::binary);
		if (!fileStream.is_open()) throw std::ifstream::failure("Could not find file at path", { 1, std::iostream_category() });
		GLTFData gltf;

		if (const std::filesystem::path path = std::filesystem::path(file); path.extension() == ".gltf") {
			Json::Value json;

			try { fileStream >> json; }
			catch (const std::exception& e) {
				Debug::LogError("Json parsing error: {}", string(e.what()));
				throw std::ifstream::failure("Json parsing error", { 1, std::iostream_category() });
			}
			fileStream.close();
			load_gltf(json, gltf, file);
		}
		else {
			load_glb(fileStream, gltf, file);
			fileStream.close();
		}

		construct_meshes(gltf, file, scene, ppFlags);
	}
	catch (std::ifstream::failure e) {
		Debug::LogError("Error reading file at path: {} \nError: {}, Code: {}: {}", file, e.what(), e.code().value(), e.code().message());
		scene->models.clear();
		scene->textures.clear();
		return errlib::Error(errlib::ErrorCode_IOerror, file);
	}

	scene->name = std::filesystem::path(file).filename().stem().string();
	sourcePath = std::filesystem::absolute(file).string();

	return errlib::Error{};
}