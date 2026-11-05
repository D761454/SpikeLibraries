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

#include "Matrix3.h"

#include <cassert>
#include <string>

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

#include "Vector3.h"

namespace sml {
	matrix3::MatrixRow matrix3::MatrixRow::zero = MatrixRow();

	matrix3 matrix3::identity = matrix3(
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f }
	);

	matrix3 matrix3::zero = matrix3();

	matrix3::MatrixRow matrix3::GetColumn(const u16 index) const {
		assert(index < 3 && "Index Out of Range! Column in range 0-2.");

		return { data[0][index], data[1][index], data[2][index] };
	}

	matrix3::MatrixRow matrix3::GetRow(const u16 index) const {
		assert(index < 3 && "Index Out of Range! Column in range 0-2.");

		return { data[index] };
	}

	matrix3::MatrixRow matrix3::MatrixRow::operator+(const MatrixRow& other) const {
		return { data[0] + other[0], data[1] + other[1], data[2] + other[2] };
	}

	matrix3::MatrixRow matrix3::MatrixRow::operator-(const MatrixRow& other) const {
		return { data[0] - other[0], data[1] - other[1], data[2] - other[2] };
	}

	matrix3::MatrixRow matrix3::MatrixRow::operator*(const f32& scalar) const {
		return { data[0] * scalar, data[1] * scalar, data[2] * scalar };
	}

	f32 matrix3::MatrixRow::operator*(const MatrixRow& other) const {
		return (data[0] * other[0]) + (data[1] * other[1]) + (data[2] * other[2]);
	}

	matrix3::MatrixRow& matrix3::MatrixRow::operator=(const MatrixRow& other) {
		data[0] = other[0];
		data[1] = other[1];
		data[2] = other[2];
		return *this;
	}

	bool matrix3::MatrixRow::operator==(const MatrixRow& other) const {
		return data[0] == other[0] &&
			data[1] == other[1] &&
			data[2] == other[2];
	}

	bool matrix3::MatrixRow::operator!=(const MatrixRow& other) const {
		return data[0] != other[0] ||
			data[1] != other[1] ||
			data[2] != other[2];
	}

	matrix3 matrix3::operator+(const matrix3& other) const {
		return { data[0] + other[0], data[1] + other[1], data[2] + other[2] };
	}

	matrix3 matrix3::operator-(const matrix3& other) const {
		return { data[0] - other[0], data[1] - other[1], data[2] - other[2] };
	}

	matrix3 matrix3::operator*(const f32& scalar) const {
		return { data[0] * scalar, data[1] * scalar, data[2] * scalar };
	}

	matrix3 matrix3::operator*(const matrix3& other) const {
		return {
			{ data[0] * other.GetColumn(0), data[0] * other.GetColumn(1), data[0] * other.GetColumn(2) },
			{ data[1] * other.GetColumn(0), data[1] * other.GetColumn(1), data[1] * other.GetColumn(2) },
			{ data[2] * other.GetColumn(0), data[2] * other.GetColumn(1), data[2] * other.GetColumn(2) }
		};
	}

	vector3 matrix3::operator*(const vector3& other) const {
		return {
			(data[0][0] * other.x) + (data[0][1] * other.y) + (data[0][2] * other.z),
			(data[1][0] * other.x) + (data[1][1] * other.y) + (data[1][2] * other.z),
			(data[2][0] * other.x) + (data[2][1] * other.y) + (data[2][2] * other.z)
		};
	}

	matrix3& matrix3::operator=(const matrix3& other) {
		data[0] = other[0];
		data[1] = other[1];
		data[2] = other[2];
		return *this;
	}

	bool matrix3::operator==(const matrix3& other) const {
		return data[0] == other[0] &&
			data[1] == other[1] &&
			data[2] == other[2];
	}

	bool matrix3::operator!=(const matrix3& other) const {
		return data[0] != other[0] ||
			data[1] != other[1] ||
			data[2] != other[2];
	}

	string matrix3::MatrixRow::ToString() const noexcept {
		return "{" + std::to_string(data[0]) + ", " + std::to_string(data[1]) + ", " + std::to_string(data[2]) + "}";
	}

	string matrix3::ToString() const noexcept {
		return "{" + data[0].ToString() + ", " + data[1].ToString() + ", " + data[2].ToString() + "}";
	}
}