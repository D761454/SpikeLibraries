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

#include "spike/assetimp/GltfImporter.h"

#include <fstream>
#include <string.h>
#include <algorithm>
#include <array>
#include <cstdlib>
#include <concepts>
#include <cctype>
#include <cstdint>
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
#include <stdexcept>
#include <string>
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
#include "spike/error.hpp"

DISABLE_WARNING_PUSH
DISABLE_WARNING_PDB_NOT_FOUND
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>
DISABLE_WARNING_POP

#include "spike/assetimp/PostProcessing.h"
#include "spike/assetimp/aicollection.h"
#include "spike/assetimp/aivector2.h"
#include "spike/assetimp/aivector3.h"
#include "spike/assetimp/aivector4.h"
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
			u32 indices = kMaxU32;
			u32 material = 0;
			u32 mode = 4;
		};
		std::vector<Primitive> primitives;
	};
	std::vector<Mesh> meshes;

	struct Accessor {
		u32 bufferView = kMaxU32;
		u32 componentType = 0;
		u32 count = 0;
		std::vector<f32> max;
		std::vector<f32> min;
		string type = "";
		u32 byteOffset = 0;
		bool normalized = false;
	};
	std::vector<Accessor> accessors;

	struct BufferView {
		u32 buffer = 0;
		u32 byteLength = 0;
		u32 byteOffset = 0;
		u32 byteStride = 0;
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
		if (parse_variable(val, it, "count")) acc.count = val.asUInt();
		if (parse_variable(val, it, "type")) acc.type = val.asString();
		if (parse_variable(val, it, "byteOffset")) acc.byteOffset = val.asUInt();
		if (parse_variable(val, it, "normalized")) acc.normalized = val.asBool();

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
		if (parse_variable(val, it, "byteOffset")) bv.byteOffset = val.asUInt();
		if (parse_variable(val, it, "byteStride")) bv.byteStride = val.asUInt();
		if (parse_variable(val, it, "target")) bv.target = val.asUInt();
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

static bool LoadBuffer(const string& path, niVector<u8>& data) {
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	if (!file.is_open()) return false;

	const std::streamoff length = file.tellg();
	if (length < 0 || static_cast<ull>(length) > data.max_size()) return false;
	data.resize(static_cast<usize>(length));
	file.seekg(0, std::ios::beg);
	if (!data.empty()) {
		file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size()));
		if (!file) return false;
	}
	return true;
}

static bool LoadBufferUri(const string& uri, const std::filesystem::path& directory, niVector<u8>& data) {
	if (uri.rfind("data:", 0) != 0) {
		string decodedUri;
		for (usize i = 0; i < uri.size(); ++i) {
			if (uri[i] == '%') {
				if (i + 2 >= uri.size()) return false;
				auto hex_value = [](char ch) -> i32 {
					if (ch >= '0' && ch <= '9') return ch - '0';
					if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
					if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
					return -1;
				};
				const i32 high = hex_value(uri[i + 1]), low = hex_value(uri[i + 2]);
				if (high < 0 || low < 0) return false;
				decodedUri.push_back(static_cast<char>((high << 4) | low));
				i += 2;
			}
			else decodedUri.push_back(uri[i]);
		}
		return LoadBuffer((directory / std::filesystem::path(decodedUri)).string(), data);
	}
	const usize comma = uri.find(',');
	if (comma == string::npos) return false;
	const string metadata = uri.substr(0, comma);
	const string payload = uri.substr(comma + 1);
	data.clear();
	if (metadata.find(";base64") != string::npos) {
		u32 accumulator = 0;
		i32 bits = 0;
		usize paddingCount = 0;
		bool padding = false;
		for (uchar ch : payload) {
			if (ch == '=') { padding = true; if (++paddingCount > 2) return false; continue; }
			if (padding) return false;
			i32 value = ch >= 'A' && ch <= 'Z' ? ch - 'A'
				: ch >= 'a' && ch <= 'z' ? ch - 'a' + 26
				: ch >= '0' && ch <= '9' ? ch - '0' + 52
				: ch == '+' ? 62 : ch == '/' ? 63 : -1;
			if (value < 0) return false;
			accumulator = ((accumulator << 6) | static_cast<u32>(value)) & 0xFFFFFFu;
			bits += 6;
			if (bits >= 8) {
				bits -= 8;
				data.push_back(static_cast<u8>((accumulator >> bits) & 0xFF));
			}
		}
		const bool validPadding = paddingCount == 0 ? (bits == 0 || bits == 2 || bits == 4)
			: paddingCount == 1 ? bits == 2 : bits == 4;
		const u32 unusedMask = bits == 0 ? 0u : (1u << bits) - 1u;
		return validPadding && (accumulator & unusedMask) == 0;
	}

	auto hex_value = [](char ch) -> i32 {
		if (ch >= '0' && ch <= '9') return ch - '0';
		if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
		if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
		return -1;
	};
	for (usize i = 0; i < payload.size(); ++i) {
		if (payload[i] == '%') {
			if (i + 2 >= payload.size()) return false;
			const i32 high = hex_value(payload[i + 1]);
			const i32 low = hex_value(payload[i + 2]);
			if (high < 0 || low < 0) return false;
			data.push_back(static_cast<u8>((high << 4) | low));
			i += 2;
		}
		else data.push_back(static_cast<u8>(payload[i]));
	}
	return true;
}

#pragma endregion

static bool load_document(Json::Value& json, GLTFData& gltf) {
	if (!json.isObject()) return false;
	LoadScenes(gltf, json);
	LoadNodes(gltf, json);
	return LoadMeshes(gltf, json)
		&& LoadAccessors(gltf, json)
		&& LoadBufferViews(gltf, json)
		&& LoadBuffers(gltf, json);
}

static bool load_gltf(Json::Value& json, GLTFData& gltf, const string& path) {
	if (!load_document(json, gltf)) return false;
	const std::filesystem::path directory = std::filesystem::path(path).parent_path();
	gltf.bufferData.resize(gltf.buffers.size());
	for (usize i = 0; i < gltf.buffers.size(); ++i) {
		const GLTFData::Buffer& buffer = gltf.buffers[i];
		if (buffer.uri.empty() || !LoadBufferUri(buffer.uri, directory, gltf.bufferData[i])) return false;
		if (gltf.bufferData[i].size() < buffer.byteLength) return false;
	}
	return true;
}

static bool load_glb(std::ifstream& inFile, GLTFData& gltf, const string& path) {
	inFile.seekg(0, std::ios::end);
	const std::streamoff fileLength = inFile.tellg();
	if (fileLength < 12 || static_cast<ull>(fileLength) > kMaxU32) return false;
	inFile.seekg(0, std::ios::beg);

	auto read_u32 = [&inFile](u32& value) {
		inFile.read(reinterpret_cast<char*>(&value), sizeof(value));
		if (!inFile) return false;
		return true;
	};

	u32 magic = 0, version = 0, totalLength = 0;
	if (!read_u32(magic) || !read_u32(version) || !read_u32(totalLength)
		|| magic != 0x46546C67 || version != 2 || totalLength != static_cast<u32>(fileLength)) return false;

	string jsonText;
	niVector<u8> binaryChunk;
	bool hasJsonChunk = false;
	bool hasBinaryChunk = false;
	usize remaining = static_cast<usize>(totalLength) - 12;
	while (remaining != 0) {
		if (remaining < 8) return false;
		u32 chunkLength = 0, chunkType = 0;
		if (!read_u32(chunkLength) || !read_u32(chunkType)) return false;
		remaining -= 8;
		if (chunkLength > remaining) return false;
		if (chunkType == 0x4E4F534A) {
			if (hasJsonChunk || chunkLength > jsonText.max_size()) return false;
			jsonText.resize(chunkLength);
			if (chunkLength != 0) inFile.read(jsonText.data(), chunkLength);
			if (!inFile) return false;
			hasJsonChunk = true;
		}
		else if (chunkType == 0x004E4942) {
			if (hasBinaryChunk || chunkLength > binaryChunk.max_size()) return false;
			binaryChunk.resize(chunkLength);
			if (chunkLength != 0) inFile.read(reinterpret_cast<char*>(binaryChunk.data()), chunkLength);
			if (!inFile) return false;
			hasBinaryChunk = true;
		}
		else {
			inFile.seekg(chunkLength, std::ios::cur);
			if (!inFile) return false;
		}
		remaining -= chunkLength;
	}
	while (!jsonText.empty() && (jsonText.back() == '\0' || jsonText.back() == ' ')) jsonText.pop_back();
	if (!hasJsonChunk || jsonText.empty()) return false;

	Json::Reader reader;
	Json::Value json;
	if (!reader.parse(jsonText, json) || !load_document(json, gltf)) return false;

	const std::filesystem::path directory = std::filesystem::path(path).parent_path();
	gltf.bufferData.resize(gltf.buffers.size());
	bool usedBinaryChunk = false;
	for (usize i = 0; i < gltf.buffers.size(); ++i) {
		const GLTFData::Buffer& buffer = gltf.buffers[i];
		if (buffer.uri.empty()) {
			if (usedBinaryChunk || !hasBinaryChunk || binaryChunk.size() < buffer.byteLength) return false;
			gltf.bufferData[i] = std::move(binaryChunk);
			usedBinaryChunk = true;
		}
		else if (!LoadBufferUri(buffer.uri, directory, gltf.bufferData[i])) {
			return false;
		}
		if (gltf.bufferData[i].size() < buffer.byteLength) return false;
	}
	return true;
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
		for (u32 i = 0; i < count; ++i) out[i] = 0;
		return;
	}
}

#pragma endregion

static inline const GLTFData::Accessor* get_accessor(const GLTFData& gltf, const GLTFData::Mesh::Primitive& primitiveData, const string& name) {
	if (auto it = primitiveData.attributeIndices.find(name); it != primitiveData.attributeIndices.end() && it->second < gltf.accessors.size())
		return &gltf.accessors[it->second];
	return nullptr;
}

static inline const GLTFData::BufferView* get_buffer_views(const GLTFData& gltf, const GLTFData::Accessor* accessor) {
	if (accessor && accessor->bufferView < gltf.bufferViews.size()) return &gltf.bufferViews[accessor->bufferView];
	return nullptr;
}

static inline const niVector<u8>* get_buffer_data(const GLTFData& gltf, const GLTFData::BufferView* bufferView) {
	if (bufferView && bufferView->buffer < gltf.bufferData.size()) return &gltf.bufferData[bufferView->buffer];
	return nullptr;
}

static bool read_accessor_values(const GLTFData& gltf, const GLTFData::Accessor* accessor, u32 expectedComponents, std::vector<f32>& values) {
	if (!accessor || get_component_count(accessor->type) != expectedComponents || accessor->count == 0) return false;
	const GLTFData::BufferView* bufferView = get_buffer_views(gltf, accessor);
	const niVector<u8>* buffer = get_buffer_data(gltf, bufferView);
	const u32 componentSize = get_component_size(static_cast<ushort>(accessor->componentType));
	if (!bufferView || !buffer || componentSize == 0 || accessor->count > values.max_size() / expectedComponents) return false;

	const usize packedSize = static_cast<usize>(componentSize) * expectedComponents;
	const usize stride = bufferView->byteStride == 0 ? packedSize : bufferView->byteStride;
	if (stride < packedSize || accessor->byteOffset > bufferView->byteLength) return false;
	const usize viewAvailable = static_cast<usize>(bufferView->byteLength) - accessor->byteOffset;
	if (packedSize > viewAvailable || static_cast<usize>(accessor->count - 1) > (viewAvailable - packedSize) / stride) return false;
	const usize start = static_cast<usize>(bufferView->byteOffset) + accessor->byteOffset;
	if (start > buffer->size()) return false;
	const usize available = buffer->size() - start;
	if (packedSize > available || static_cast<usize>(accessor->count - 1) > (available - packedSize) / stride) return false;

	values.resize(static_cast<usize>(accessor->count) * expectedComponents);
	const u8* base = buffer->data() + start;
	for (u32 row = 0; row < accessor->count; ++row) {
		for (u32 column = 0; column < expectedComponents; ++column) {
			const u8* source = base + static_cast<usize>(row) * stride + static_cast<usize>(column) * componentSize;
			f32 value = 0;
			switch (accessor->componentType) {
			case kGltfComponentTypeByte: {
				u8 raw = 0; std::memcpy(&raw, source, sizeof(raw));
				value = accessor->normalized ? std::max(-1.0f, static_cast<f32>(raw) / kMaxI8) : static_cast<f32>(raw);
			} break;
			case kGltfComponentTypeUByte: {
				u8 raw = 0; std::memcpy(&raw, source, sizeof(raw));
				value = accessor->normalized ? static_cast<f32>(raw) / kMaxU8 : static_cast<f32>(raw);
			} break;
			case kGltfComponentTypeShort: {
				u16 raw = 0; std::memcpy(&raw, source, sizeof(raw));
				value = accessor->normalized ? std::max(-1.0f, static_cast<f32>(raw) / kMaxI16) : static_cast<f32>(raw);
			} break;
			case kGltfComponentTypeUShort: {
				u16 raw = 0; std::memcpy(&raw, source, sizeof(raw));
				value = accessor->normalized ? static_cast<f32>(raw) / kMaxUshort : static_cast<f32>(raw);
			} break;
			case kGltfComponentTypeFloat:
				std::memcpy(&value, source, sizeof(value));
				break;
			default:
				return false;
			}
			values[static_cast<usize>(row) * expectedComponents + column] = value;
		}
	}
	return true;
}

static bool read_accessor_indices(const GLTFData& gltf, const GLTFData::Accessor* accessor, std::vector<u32>& indices) {
	if (!accessor || accessor->type != "SCALAR" || accessor->count == 0) return false;
	if (accessor->componentType != kGltfComponentTypeUByte
		&& accessor->componentType != kGltfComponentTypeUShort
		&& accessor->componentType != kGltfComponentTypeUInt) return false;
	const GLTFData::BufferView* bufferView = get_buffer_views(gltf, accessor);
	const niVector<u8>* buffer = get_buffer_data(gltf, bufferView);
	const u32 componentSize = get_component_size(static_cast<ushort>(accessor->componentType));
	if (!bufferView || !buffer || bufferView->byteStride != 0) return false;
	if (accessor->byteOffset > bufferView->byteLength) return false;
	const usize size = static_cast<usize>(accessor->count) * componentSize;
	if (size > static_cast<usize>(bufferView->byteLength) - accessor->byteOffset) return false;
	const usize start = static_cast<usize>(bufferView->byteOffset) + accessor->byteOffset;
	if (start > buffer->size() || size > buffer->size() - start) return false;
	indices.resize(accessor->count);
	for (u32 i = 0; i < accessor->count; ++i) {
		const u8* source = buffer->data() + start + static_cast<usize>(i) * componentSize;
		if (componentSize == 1) indices[i] = *source;
		else if (componentSize == 2) {
			u16 value = 0; std::memcpy(&value, source, sizeof(value)); indices[i] = value;
		}
		else std::memcpy(&indices[i], source, sizeof(u32));
	}
	return true;
}

template<typename T>
inline void generate_tangents(assimp::Mesh& model, const std::vector<T>& idxBuff) {
	std::vector<assimp::vector3> tan1(model.vertices48.size()), tan2(model.vertices48.size());

	for (usize i = 0; i + 2 < idxBuff.size(); i += 3) {
		const usize i1 = idxBuff[i];
		const usize i2 = idxBuff[i + 1];
		const usize i3 = idxBuff[i + 2];
		if (i1 >= model.vertices48.size() || i2 >= model.vertices48.size() || i3 >= model.vertices48.size())
			throw std::runtime_error("glTF index exceeds the vertex buffer.");

		const assimp::VertexP3N3T2T4& v1 = model.vertices48[i1];
		const assimp::VertexP3N3T2T4& v2 = model.vertices48[i2];
		const assimp::VertexP3N3T2T4& v3 = model.vertices48[i3];

		const f32 x1 = v2.position.x - v1.position.x;
		const f32 x2 = v3.position.x - v1.position.x;
		const f32 y1 = v2.position.y - v1.position.y;
		const f32 y2 = v3.position.y - v1.position.y;
		const f32 z1 = v2.position.z - v1.position.z;
		const f32 z2 = v3.position.z - v1.position.z;

		const f32 s1 = v2.texCoord.x - v1.texCoord.x;
		const f32 s2 = v3.texCoord.x - v1.texCoord.x;
		const f32 t1 = v2.texCoord.y - v1.texCoord.y;
		const f32 t2 = v3.texCoord.y - v1.texCoord.y;

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

static void triangulate_primitive(std::vector<u32>& indices, u32 mode) {
	if (mode == 4) return;
	if (mode != 5 && mode != 6) throw std::runtime_error("Only triangle, triangle-strip, and triangle-fan glTF primitives are supported.");
	std::vector<u32> triangles;
	if (mode == 5) {
		if (indices.size() >= 3) triangles.reserve((indices.size() - 2) * 3);
		for (usize i = 0; i + 2 < indices.size(); ++i) {
			if ((i & 1) == 0) triangles.insert(triangles.end(), { indices[i], indices[i + 1], indices[i + 2] });
			else triangles.insert(triangles.end(), { indices[i + 1], indices[i], indices[i + 2] });
		}
	}
	else {
		if (indices.size() >= 3) triangles.reserve((indices.size() - 2) * 3);
		for (usize i = 1; i + 1 < indices.size(); ++i)
			triangles.insert(triangles.end(), { indices[0], indices[i], indices[i + 1] });
	}
	indices.swap(triangles);
}

static void construct_mesh(const GLTFData& gltf, const u32 idx, const string&, assimp::Collection* scene, const u32 ppSteps) {
	if (idx >= gltf.meshes.size()) throw std::runtime_error("glTF mesh index is out of range.");
	assimp::Mesh model;
	const GLTFData::Mesh& meshData = gltf.meshes[idx];
	model.name = meshData.name;
	const bool useTangents = (ppSteps & assimp::PostProcessingFlags_importTangents) || (ppSteps & assimp::PostProcessingFlags_pbrImport);
	const bool makeNormals = (ppSteps & assimp::PostProcessingFlags_generateMissingNormals) || useTangents;
	const bool makeUVs = (ppSteps & assimp::PostProcessingFlags_generateMissingUVs) || useTangents;
	std::set<u32> materials;
	std::vector<u32> missingTangentIndices;
	model.subMeshes.reserve(meshData.primitives.size());

	for (const GLTFData::Mesh::Primitive& primitive : meshData.primitives) {
		const GLTFData::Accessor* positionAccessor = get_accessor(gltf, primitive, "POSITION");
		const GLTFData::Accessor* normalAccessor = get_accessor(gltf, primitive, "NORMAL");
		const GLTFData::Accessor* uvAccessor = get_accessor(gltf, primitive, "TEXCOORD_0");
		const GLTFData::Accessor* tangentAccessor = useTangents ? get_accessor(gltf, primitive, "TANGENT") : nullptr;
		if (!positionAccessor || positionAccessor->componentType != kGltfComponentTypeFloat)
			throw std::runtime_error("glTF primitive has no valid POSITION accessor.");

		std::vector<f32> positions, normals, uvs, tangents;
		if (!read_accessor_values(gltf, positionAccessor, 3, positions))
			throw std::runtime_error("glTF POSITION accessor is invalid or out of bounds.");
		if (normalAccessor && (!read_accessor_values(gltf, normalAccessor, 3, normals) || normalAccessor->componentType != kGltfComponentTypeFloat))
			throw std::runtime_error("glTF NORMAL accessor is invalid or out of bounds.");
		if (uvAccessor && !read_accessor_values(gltf, uvAccessor, 2, uvs))
			throw std::runtime_error("glTF TEXCOORD_0 accessor is invalid or out of bounds.");
		if (tangentAccessor && (!read_accessor_values(gltf, tangentAccessor, 4, tangents) || tangentAccessor->componentType != kGltfComponentTypeFloat))
			throw std::runtime_error("glTF TANGENT accessor is invalid or out of bounds.");

		const u32 vertexCount = positionAccessor->count;
		if ((normalAccessor && normalAccessor->count != vertexCount)
			|| (uvAccessor && uvAccessor->count != vertexCount)
			|| (tangentAccessor && tangentAccessor->count != vertexCount))
			throw std::runtime_error("glTF vertex attribute counts do not match POSITION.");

		std::vector<u32> indices;
		if (primitive.indices != kMaxU32) {
			if (primitive.indices >= gltf.accessors.size() || !read_accessor_indices(gltf, &gltf.accessors[primitive.indices], indices))
				throw std::runtime_error("glTF index accessor is invalid or out of bounds.");
		}
		else {
			indices.resize(vertexCount);
			for (u32 i = 0; i < vertexCount; ++i) indices[i] = i;
		}
		for (u32 index : indices) {
			if (index >= vertexCount) throw std::runtime_error("glTF index exceeds the primitive vertex count.");
		}
		if (primitive.mode != 4 && !(ppSteps & assimp::PostProcessingFlags_triangulateFaces))
			throw std::runtime_error("glTF non-triangle primitive requires triangulateFaces.");
		triangulate_primitive(indices, primitive.mode);
		if (indices.size() % 3 != 0) throw std::runtime_error("glTF triangle primitive has an invalid index count.");

		const usize vertexStart = useTangents ? model.vertices48.size() : model.vertices32.size();
		const usize indexStart = model.indexBuffer32.size();
		for (u32 vertex = 0; vertex < vertexCount; ++vertex) {
			const usize p = static_cast<usize>(vertex) * 3;
			const usize t = static_cast<usize>(vertex) * 2;
			assimp::vector3 position(positions[p], positions[p + 1], positions[p + 2]);
			assimp::vector3 normal = normalAccessor ? assimp::vector3(normals[p], normals[p + 1], normals[p + 2]) : assimp::vector3();
			assimp::vector2 uv = uvAccessor ? assimp::vector2(uvs[t], uvs[t + 1])
				: makeUVs ? assimp::vector2(position.x, position.y) : assimp::vector2();
			if (ppSteps & assimp::PostProcessingFlags_flipUVs) uv.y *= -1.0f;
			if (useTangents) {
				assimp::VertexP3N3T2T4 output(position, normal, uv);
				if (tangentAccessor && !(ppSteps & assimp::PostProcessingFlags_flipUVs)) {
					const usize tangentIndex = static_cast<usize>(vertex) * 4;
					output.tangent = assimp::vector4(tangents[tangentIndex], tangents[tangentIndex + 1], tangents[tangentIndex + 2], tangents[tangentIndex + 3]);
				}
				model.vertices48.push_back(output);
			}
			else model.vertices32.emplace_back(position, normal, uv);
		}

		for (u32 index : indices) model.indexBuffer32.push_back(static_cast<u32>(vertexStart + index));

		if (makeNormals && !normalAccessor) {
			std::vector<assimp::vector3> generated(vertexCount);
			for (usize i = 0; i < indices.size(); i += 3) {
				const u32 ia = indices[i], ib = indices[i + 1], ic = indices[i + 2];
				const assimp::vector3 a = useTangents ? model.vertices48[vertexStart + ia].position : model.vertices32[vertexStart + ia].position;
				const assimp::vector3 b = useTangents ? model.vertices48[vertexStart + ib].position : model.vertices32[vertexStart + ib].position;
				const assimp::vector3 c = useTangents ? model.vertices48[vertexStart + ic].position : model.vertices32[vertexStart + ic].position;
				const assimp::vector3 faceNormal = cross(b - a, c - a);
				generated[ia] += faceNormal; generated[ib] += faceNormal; generated[ic] += faceNormal;
			}
			for (u32 vertex = 0; vertex < vertexCount; ++vertex) {
				const assimp::vector3 normal = normalize(generated[vertex]);
				if (useTangents) model.vertices48[vertexStart + vertex].normal = normal;
				else model.vertices32[vertexStart + vertex].normal = normal;
			}
		}

		if (useTangents && (!tangentAccessor || (ppSteps & assimp::PostProcessingFlags_flipUVs))) {
			for (u32 index : indices) missingTangentIndices.push_back(static_cast<u32>(vertexStart + index));
		}

		assimp::SubMesh subMesh;
		subMesh.indexOffset = static_cast<u32>(indexStart);
		subMesh.indexCount = static_cast<u32>(indices.size());
		if (primitive.material > kMaxU8) throw std::runtime_error("glTF material index exceeds the collection format limit.");
		subMesh.materialIdx = static_cast<u8>(primitive.material);
		materials.insert(primitive.material);
		model.subMeshes.push_back(std::move(subMesh));
	}

	if (useTangents && !missingTangentIndices.empty()) generate_tangents(model, missingTangentIndices);
	if (ppSteps & assimp::PostProcessingFlags_flipWindingOrder) flip_winding_order(model.indexBuffer32);
	if (materials.size() > kMaxU8) throw std::runtime_error("glTF material count exceeds the collection format limit.");
	model.materialCount = static_cast<u8>(std::max<usize>(1, materials.size()));
	for (auto& subMesh : model.subMeshes) {
		if (useTangents) calculate_submesh_bounds(subMesh, model.vertices48, model.indexBuffer32);
		else calculate_submesh_bounds(subMesh, model.vertices32, model.indexBuffer32);
	}
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
				fileStream.close();
				throw std::ifstream::failure("Json parsing error", { 1, std::iostream_category() });
				return errlib::Error(errlib::ErrorCode_unknown, "Json parsing error: " + string(e.what()));
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
		scene->models.clear();
		scene->textures.clear();
		return errlib::Error(errlib::ErrorCode_IOerror, "Error reading file at path: " + file + "Error: " + e.what());
	}

	scene->name = std::filesystem::path(file).filename().stem().string();
	sourcePath = std::filesystem::absolute(file).string();

	return errlib::Error{};
}