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

#include "spike/sml/matrix2.h"

#include <cassert>
#include <string.h>

#include "spike/def_aliases.h"

#include "spike/sml/vector2.h"
#include "spike/sml/sml_core.h"

namespace sml {
	matrix2 matrix2::identity = matrix2({
		1.f, 0.f,
		0.f, 1.f
	});

	matrix2 matrix2::zero = matrix2();

	sml::vector2 matrix2::GetColumn(const i32 index) const {
		assert(index < 2 && "Index Out of Range! Column in range 0-1.");
		const i32 base = index * 2;

		return { _data[base], _data[base + 1] };
	}

	sml::vector2 matrix2::GetRow(const i32 index) const {
		assert(index < 2 && "Index Out of Range! Column in range 0-1.");

		return { _data[index], _data[index + 2] };
	}

	void matrix2::SetColumn(const i32 index, const sml::vector2& column) {
		assert(index < 2 && "Index Out of Range! Column in range 0-1.");
		const i32 base = index * 2;
		_data[base] = column.x;
		_data[base + 1] = column.y;
	}

	void matrix2::SetRow(const i32 index, const sml::vector2& row) {
		assert(index < 2 && "Index Out of Range! Column in range 0-1.");

		_data[index] = row.x;
		_data[index + 2] = row.y;
	}

	f32 matrix2::Determinant() const noexcept {
		f64 m00 = _data[0], m10 = _data[1];
		f64 m01 = _data[2], m11 = _data[3];

		f64 det = m00 * m11 - m01 * m10;
		return (f32)det;
	}

	matrix2 matrix2::Inverse() const noexcept {
		f64 m00 = _data[0], m10 = _data[1];
		f64 m01 = _data[2], m11 = _data[3];

		f64 det = m00 * m11 - m01 * m10;

		if (sml::abs(det) < 1e-8) {
			return matrix2::zero;
		}

		f64 invDet = 1.0 / det;
		matrix2 out;
		out[0] = (f32)(m11 * invDet);
		out[1] = (f32)(-m10 * invDet);
		out[2] = (f32)(-m01 * invDet);
		out[3] = (f32)(m00 * invDet);
		return out;
	}

	bool matrix2::IsIdentity() const noexcept {
		return *this == matrix2::identity;
	}

	matrix2 matrix2::Transpose() const noexcept {
		matrix2 out;
		for (i32 r = 0; r < 2; ++r) {
			for (i32 c = 0; c < 2; ++c) {
				out[c * 2 + r] = _data[r * 2 + c];
			}
		}
		return out;
	}

	void matrix2::Multiply(const matrix2& other) {
		*this = (*this) * other;
	}

	void matrix2::PreMultiply(const matrix2& other) {
		*this = other * (*this);
	}

	matrix2 matrix2::operator+(const matrix2& other) const {
		return matrix2({ _data[0] + other[0], _data[1] + other[1], _data[2] + other[2], _data[3] + other[3] });
	}

	matrix2 matrix2::operator-(const matrix2& other) const {
		return matrix2({ _data[0] - other[0], _data[1] - other[1], _data[2] - other[2], _data[3] - other[3] });
	}

	matrix2 matrix2::operator*(const f32& scalar) const {
		return matrix2({ _data[0] * scalar, _data[1] * scalar, _data[2] * scalar, _data[3] * scalar});
	}

	matrix2 matrix2::operator*(const matrix2& other) const {
		matrix2 out;
		for (i32 c = 0; c < 2; ++c) {
			for (i32 r = 0; r < 2; ++r) {
				f32 sum = 0.f;
				for (i32 k = 0; k < 3; ++k) {
					sum += _data[k * 2 + r] * other[c * 2 + k];
				}
				out[c * 2 + r] = sum;
			}
		}
		return out;
	}

	vector2 matrix2::operator*(const vector2& other) const {
		return {
			(_data[0] * other.x) + (_data[2] * other.y),
			(_data[1] * other.x) + (_data[3] * other.y)
		};
	}

	matrix2& matrix2::operator=(const matrix2& other) {
		std::memcpy(&_data[0], other.Data(), sizeof(matrix2));
		return *this;
	}

	bool matrix2::operator==(const matrix2& other) const {
		return _data[0] == other[0] && _data[1] == other[1] && _data[2] == other[2] && _data[3] == other[3];
	}

	bool matrix2::operator!=(const matrix2& other) const {
		return _data[0] != other[0] || _data[1] != other[1] || _data[2] != other[2] || _data[3] != other[3];
	}
}