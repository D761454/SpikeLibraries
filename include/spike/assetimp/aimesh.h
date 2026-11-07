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

#ifndef __ASSET_IMP_MESH_H
#define __ASSET_IMP_MESH_H

#include <algorithm>
#include <type_traits>
#include <vector>

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"
#include "spike/sml/vector2.h"
#include "spike/sml/vector3.h"
#include "spike/sml/vector4.h"

namespace assimp {
	/*default layout*/
	struct VertexP3N3T2 {
		sml::vector3 position;
		sml::vector3 normal;
		sml::vector2 texCoord;

		VertexP3N3T2() noexcept = default;
		VertexP3N3T2(sml::vector3 p, sml::vector3 n, sml::vector2 uv) : position(p), normal(n), texCoord(uv) {}

		bool operator==(const VertexP3N3T2& other) const {
			return position == other.position && normal == other.normal && texCoord == other.texCoord;
		}
	};

	/*mesh layout for normal mapping*/
	struct VertexP3N3T2T4 {
		sml::vector3 position;
		sml::vector3 normal;
		sml::vector2 texCoord;
		sml::vector4 tangent = { 1, 0, 0, 1 };

		VertexP3N3T2T4() noexcept = default;
		VertexP3N3T2T4(sml::vector3 p, sml::vector3 n, sml::vector2 uv) : position(p), normal(n), texCoord(uv) {}
		VertexP3N3T2T4(sml::vector3 p, sml::vector3 n, sml::vector2 uv, sml::vector4 tan) : position(p), normal(n), texCoord(uv), tangent(tan) {}
		VertexP3N3T2T4(const VertexP3N3T2& vert) : position(vert.position), normal(vert.normal), texCoord(vert.texCoord) {}

		bool operator==(const VertexP3N3T2T4& other) const {
			return position == other.position && normal == other.normal && texCoord == other.texCoord && tangent == other.tangent;
		}
	};

	struct SubMesh {
		sml::vector3 min, max;

		u32 indexOffset = 0;
		u32 indexCount = 0;
		u8 materialIdx = 0;

		SubMesh() noexcept = default;
		~SubMesh() {}
	};

	struct Model {
		string name;

		/*each sub mesh used to define a model, holding index offset and count for use with the vertex buffers*/
		std::vector<SubMesh> subMeshes;

		/*vertex buffer for pos,norm,uv data*/
		std::vector<VertexP3N3T2> vertices32;
		/*vertex buffer for pos,norm,uv,tangent data (bi-tangent is reconstructed within a shader)*/
		std::vector<VertexP3N3T2T4> vertices48;

		std::vector<u32> indexBuffer32; // used for meshes with > 65,535 vertices
		std::vector<u16> indexBuffer16; // used for meshes with <= 65,535 vertices

		u8 materialCount = 0;

		Model() noexcept = default;
		~Model() {}
	};

	struct VertexHash {
		usize operator()(const VertexP3N3T2& v) const {
			usize seed = 0;
			auto hash_combine = [](usize& s, f32 val) {
				// Normalize -0.0 to 0.0 to prevent bit-pattern mismatches
				f32 normalized = (val == 0.0f) ? 0.0f : val;
				s ^= std::hash<f32>{}(normalized)+0x9e3779b9 + (s << 6) + (s >> 2);
				};

			hash_combine(seed, v.position.x);
			hash_combine(seed, v.position.y);
			hash_combine(seed, v.position.z);
			hash_combine(seed, v.normal.x);
			hash_combine(seed, v.normal.y);
			hash_combine(seed, v.normal.z);
			hash_combine(seed, v.texCoord.x);
			hash_combine(seed, v.texCoord.y);

			return seed;
		}

		usize operator()(const VertexP3N3T2T4& v) const {
			usize seed = 0;
			auto hash_combine = [](usize& s, f32 val) {
				// Normalize -0.0 to 0.0 to prevent bit-pattern mismatches
				f32 normalized = (val == 0.0f) ? 0.0f : val;
				s ^= std::hash<f32>{}(normalized)+0x9e3779b9 + (s << 6) + (s >> 2);
				};

			hash_combine(seed, v.position.x);
			hash_combine(seed, v.position.y);
			hash_combine(seed, v.position.z);
			hash_combine(seed, v.normal.x);
			hash_combine(seed, v.normal.y);
			hash_combine(seed, v.normal.z);
			hash_combine(seed, v.texCoord.x);
			hash_combine(seed, v.texCoord.y);
			hash_combine(seed, v.tangent.x);
			hash_combine(seed, v.tangent.y);
			hash_combine(seed, v.tangent.z);

			return seed;
		}
	};

	template<typename Vertex, typename Index>
	inline void calculate_submesh_bounds(SubMesh& subMesh, const std::vector<Vertex>& vertices, const std::vector<Index>& indices) {
		const usize start = subMesh.indexOffset;
		if (start >= indices.size()) return;

		const usize end = std::min(start + subMesh.indexCount, indices.size());
		bool hasBounds = false;
		for (usize i = start; i < end; ++i) {
			const sml::vector3& position = vertices[indices[i]].position;
			if (!hasBounds) {
				subMesh.min = position;
				subMesh.max = position;
				hasBounds = true;
				continue;
			}

			if (position.x < subMesh.min.x) subMesh.min.x = position.x;
			if (position.y < subMesh.min.y) subMesh.min.y = position.y;
			if (position.z < subMesh.min.z) subMesh.min.z = position.z;
			if (position.x > subMesh.max.x) subMesh.max.x = position.x;
			if (position.y > subMesh.max.y) subMesh.max.y = position.y;
			if (position.z > subMesh.max.z) subMesh.max.z = position.z;
		}
	}
}

#endif