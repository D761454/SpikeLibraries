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

#ifndef __ASSET_IMP_VECTOR3_H
#define __ASSET_IMP_VECTOR3_H

/*
----------------------------------------------------------------------

File Summary:
	Basic vector 3 struct used to store position data.

----------------------------------------------------------------------
*/

#include <cmath>

#include <spike/def_aliases.h>

namespace assimp {
	struct vector3 {
		f32 x, y, z;

		vector3() noexcept : x(0.f), y(0.f), z(0.f) {}
		vector3(f32 x, f32 y, f32 z) noexcept : x(x), y(y), z(z) {}

		vector3& operator+=(const vector3& other) {
			x += other.x;
			y += other.y;
			z += other.z;
			return *this;
		}

		vector3 operator-(const vector3& other) const {
			return { x - other.x, y - other.y, z - other.z };
		}

		vector3 operator*(const f32 scalar) const {
			return { x * scalar, y * scalar, z * scalar };
		}

		bool operator==(const vector3& other) const {
			return x == other.x && y == other.y && z == other.z;
		}

		bool operator!=(const vector3& other) const {
			return x != other.x || y != other.y || z != other.z;
		}
	};

	inline f32 dot(const assimp::vector3& lhs, const assimp::vector3& rhs) noexcept {
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	inline assimp::vector3 cross(const assimp::vector3& lhs, const assimp::vector3& rhs) noexcept {
		return {
			lhs.y * rhs.z - lhs.z * rhs.y,
			lhs.z * rhs.x - lhs.x * rhs.z,
			lhs.x * rhs.y - lhs.y * rhs.x
		};
	}

	inline assimp::vector3 normalize(const assimp::vector3& v) noexcept {
		f32 lenSq = dot(v, v);
		if (lenSq > 1e-8f) {
			f32 invLen = 1.0f / std::sqrt(lenSq);
			return v * invLen;
		}
		return { 0.0f, 0.0f, 0.0f };
	}
}

#endif