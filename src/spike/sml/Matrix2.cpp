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

#include "spike/sml/Matrix2.h"

#include <cassert>
#include <string>

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

#include "spike/sml/Vector2.h"

namespace sml {
	// ============================================================================
	// Static Constants
	// ============================================================================

	matrix2::MatrixRow matrix2::MatrixRow::zero = MatrixRow();

	matrix2 matrix2::identity = matrix2(
		{ 1.0f, 0.0f },
		{ 0.0f, 1.0f }
	);

	matrix2 matrix2::zero = matrix2();

	// ============================================================================
	// Member Method Implementations
	// ============================================================================

	matrix2::MatrixRow matrix2::GetColumn(const u16 index) const {
		assert(index < 2 && "Index Out of Range! Column in range 0-1.");

		return { data[0][index], data[1][index] };
	}

	matrix2::MatrixRow matrix2::GetRow(const u16 index) const {
		assert(index < 2 && "Index Out of Range! Row in range 0-1.");

		return { data[index] };
	}

	// ============================================================================
	// Operator Implementations
	// ============================================================================

	matrix2::MatrixRow matrix2::MatrixRow::operator+(const MatrixRow& other) const {
		return { data[0] + other[0], data[1] + other[1] };
	}

	matrix2::MatrixRow matrix2::MatrixRow::operator-(const MatrixRow& other) const {
		return { data[0] - other[0], data[1] - other[1] };
	}

	matrix2::MatrixRow matrix2::MatrixRow::operator*(const f32& scalar) const {
		return { data[0] * scalar, data[1] * scalar };
	}

	f32 matrix2::MatrixRow::operator*(const MatrixRow& other) const {
		return (data[0] * other[0]) + (data[1] * other[1]);
	}

	matrix2::MatrixRow& matrix2::MatrixRow::operator=(const MatrixRow& other) {
		data[0] = other[0];
		data[1] = other[1];
		return *this;
	}

	bool matrix2::MatrixRow::operator==(const MatrixRow& other) const {
		return data[0] == other[0] &&
			data[1] == other[1];
	}

	bool matrix2::MatrixRow::operator!=(const MatrixRow& other) const {
		return data[0] != other[0] ||
			data[1] != other[1];
	}

	matrix2 matrix2::operator+(const matrix2& other) const {
		return { data[0] + other[0], data[1] + other[1] };
	}

	matrix2 matrix2::operator-(const matrix2& other) const {
		return { data[0] - other[0], data[1] - other[1] };
	}

	matrix2 matrix2::operator*(const f32& scalar) const {
		return { data[0] * scalar, data[1] * scalar };
	}

	matrix2 matrix2::operator*(const matrix2& other) const {
		return {
			{ data[0] * other.GetColumn(0), data[0] * other.GetColumn(1) },
			{ data[1] * other.GetColumn(0), data[1] * other.GetColumn(1) }
		};
	}

	vector2 matrix2::operator*(const vector2& other) const {
		return {
			(data[0][0] * other.x) + (data[0][1] * other.y),
			(data[1][0] * other.x) + (data[1][1] * other.y)
		};
	}

	matrix2& matrix2::operator=(const matrix2& other) {
		data[0] = other[0];
		data[1] = other[1];
		return *this;
	}

	bool matrix2::operator==(const matrix2& other) const {
		return data[0] == other[0] &&
			data[1] == other[1];
	}

	bool matrix2::operator!=(const matrix2& other) const {
		return data[0] != other[0] ||
			data[1] != other[1];
	}

	string matrix2::MatrixRow::ToString() const noexcept {
		return "{" + std::to_string(data[0]) + ", " + std::to_string(data[1]) + "}";
	}

	string matrix2::ToString() const noexcept {
		return "{" + data[0].ToString() + ", " + data[1].ToString() + "}";
	}
}