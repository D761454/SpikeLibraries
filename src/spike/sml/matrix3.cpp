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

#include "spike/sml/matrix3.h"

#include <cassert>

#include "spike/def_aliases.h"

#include "spike/sml/vector3.h"
#include "spike/sml/sml_core.h"

namespace sml {
	matrix3 matrix3::identity = matrix3({
		1.0, 0.0, 0.f,
		0.0, 1.0, 0.f,
		0.0, 0.0, 1.f
	});

	matrix3 matrix3::zero = matrix3();

	sml::vector3 matrix3::GetColumn(const i32 index) const {
		assert(index < 3 && "Index Out of Range! Column in range 0-2.");
		const i32 base = index * 3;

		return { _data[base], _data[base + 1], _data[base + 2] };
	}

	sml::vector3 matrix3::GetRow(const i32 index) const {
		assert(index < 3 && "Index Out of Range! Column in range 0-2.");

		return { _data[index], _data[index + 3], _data[index + 6] };
	}

	void matrix3::SetColumn(const i32 index, const sml::vector3& column) {
		assert(index < 3 && "Index Out of Range! Column in range 0-2.");
		const i32 base = index * 3;
		_data[base] = column.x;
		_data[base + 1] = column.y;
		_data[base + 2] = column.z;
	}

	void matrix3::SetRow(const i32 index, const sml::vector3& row) {
		assert(index < 3 && "Index Out of Range! Column in range 0-2.");

		_data[index] = row.x;
		_data[index + 3] = row.y;
		_data[index + 6] = row.z;
	}

	f32 matrix3::Determinant() const noexcept {
		f64 m00 = _data[0], m10 = _data[1], m20 = _data[2];
		f64 m01 = _data[3], m11 = _data[4], m21 = _data[5];
		f64 m02 = _data[6], m12 = _data[7], m22 = _data[8];

		f64 det = m00 * (m11 * m22 - m12 * m21) -
			m01 * (m10 * m22 - m12 * m20) +
			m02 * (m10 * m21 - m11 * m20);

		return (f32)det;
	}

	matrix3 matrix3::Inverse() const noexcept {
		f64 m00 = _data[0], m10 = _data[1], m20 = _data[2];
		f64 m01 = _data[3], m11 = _data[4], m21 = _data[5];
		f64 m02 = _data[6], m12 = _data[7], m22 = _data[8];

		f64 adj[9];
		adj[0] = m11 * m22 - m12 * m21;
		adj[3] = -(m10 * m22 - m12 * m20);
		adj[6] = m10 * m21 - m11 * m20;

		adj[1] = -(m01 * m22 - m02 * m21);
		adj[4] = m00 * m22 - m02 * m20;
		adj[7] = -(m00 * m21 - m01 * m20);

		adj[2] = m01 * m12 - m02 * m11;
		adj[5] = -(m00 * m12 - m02 * m10);
		adj[8] = m00 * m11 - m01 * m10;

		f64 det = m00 * adj[0] + m01 * adj[3] + m02 * adj[6];

		if (sml::abs(det) < 1e-8) {
			return matrix3::zero;
		}

		f64 invDet = 1.0 / det;
		matrix3 out;
		for (i32 i = 0; i < 9; ++i) {
			out[i] = (f32)(adj[i] * invDet);
		}
		return out;
	}

	bool matrix3::IsIdentity() const noexcept {
		return *this == matrix3::identity;
	}

	matrix3 matrix3::Transpose() const noexcept {
		matrix3 out;
		for (i32 r = 0; r < 3; ++r) {
			for (i32 c = 0; c < 3; ++c) {
				out[c * 3 + r] = _data[r * 3 + c];
			}
		}
		return out;
	}

	void matrix3::Multiply(const matrix3& other) {
		*this = (*this) * other;
	}

	void matrix3::PreMultiply(const matrix3& other) {
		*this = other * (*this);
	}

	matrix3 matrix3::operator+(const matrix3& other) const {
		return matrix3({
			_data[0] + other[0], _data[1] + other[1], _data[2] + other[2],
			_data[3] + other[3], _data[4] + other[4], _data[5] + other[5],
			_data[6] + other[6], _data[7] + other[7], _data[8] + other[8]
		});
	}

	matrix3 matrix3::operator-(const matrix3& other) const {
		return matrix3({
			_data[0] - other[0], _data[1] - other[1], _data[2] - other[2],
			_data[3] - other[3], _data[4] - other[4], _data[5] - other[5],
			_data[6] - other[6], _data[7] - other[7], _data[8] - other[8]
		});
	}

	matrix3 matrix3::operator*(const f32& scalar) const {
		return matrix3({
			_data[0] * scalar, _data[1] * scalar, _data[2] * scalar,
			_data[3] * scalar, _data[4] * scalar, _data[5] * scalar,
			_data[6] * scalar, _data[7] * scalar, _data[8] * scalar,
		});
	}

	matrix3 matrix3::operator*(const matrix3& other) const {
		matrix3 out;
		for (i32 c = 0; c < 3; ++c) {
			for (i32 r = 0; r < 3; ++r) {
				f32 sum = 0.f;
				for (i32 k = 0; k < 3; ++k) {
					sum += _data[k * 3 + r] * other[c * 3 + k];
				}
				out[c * 3 + r] = sum;
			}
		}
		return out;
	}

	vector3 matrix3::operator*(const vector3& other) const {
		return {
			(_data[0] * other.x) + (_data[3] * other.y) + (_data[6] * other.z),
			(_data[1] * other.x) + (_data[4] * other.y) + (_data[7] * other.z),
			(_data[2] * other.x) + (_data[5] * other.y) + (_data[8] * other.z)
		};
	}

	bool matrix3::operator==(const matrix3& other) const {
		return {
			_data[0] && other[0] && _data[3] == other[3] && _data[6] == other[6] &&
			_data[1] && other[1] && _data[4] == other[4] && _data[7] == other[7] &&
			_data[2] && other[2] && _data[5] == other[5] && _data[8] == other[8]
		};
	}

	bool matrix3::operator!=(const matrix3& other) const {
		return {
			_data[0] != other[0] || _data[3] != other[3] || _data[6] != other[6] || 
			_data[1] != other[1] || _data[4] != other[4] || _data[7] != other[7] || 
			_data[2] != other[2] || _data[5] != other[5] || _data[8] != other[8]
		};
	}
}