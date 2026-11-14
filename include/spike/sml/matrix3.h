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

#ifndef __SPIKE_SML_MATRIX3_H
#define __SPIKE_SML_MATRIX3_H

/*
----------------------------------------------------------------------

File Summary:
	matrix3 struct - Representation of a 3x3 matrix.
	Contains constructors, operators, and static methods for common
	matrix operations.

----------------------------------------------------------------------
*/

#include <cassert>
#include <array>

#include "spike/def_aliases.h"

#include "spike/sml/vector3.h"

namespace sml {
	/// <summary>
	/// A standard 3x3 transformation matrix.
	/// Column-major notation is used as in OpenGL: the internal storage is an array of 3 columns.
	/// Each MatrixRow instance therefore represents a column and elements are accessed as data[column][row].
	/// </summary>
	struct matrix3 {
	private:
		std::array<f32, 9> _data;

	public:
		static matrix3 identity;
		static matrix3 zero;

		matrix3() noexcept : _data({}) {}
		explicit matrix3(const std::array<f32, 9> arr) noexcept : _data(arr) {}

		/// <summary>
		/// Access to an element by index (0..8).
		/// Elements are stored in column major
		/// </summary>
		/// <param name="index">element index in the range 0..8</param>
		/// <returns>reference to selected matrix element</returns>
		inline f32& operator[](const u32 index) {
			assert(index < 9 && "Index Out of Range! matrix3 data in range 0-8.");

			return _data[index];
		}

		/// <summary>
		/// Const access to an element by index (0..8).
		/// Elements are stored in column major
		/// </summary>
		/// <param name="index">element index in the range 0..8</param>
		/// <returns>selected matrix element</returns>
		inline const f32 operator[](const u32 index) const {
			assert(index < 9 && "Index Out of Range! matrix3 data in range 0-8.");

			return _data[index];
		}

		/// <summary>
		/// The determinant of the matrix.
		/// </summary>
		/// <returns>determinant of this matrix</returns>
		f32	Determinant() const noexcept;

		/// <summary>
		/// The inverse of this matrix.
		/// </summary>
		/// <returns>inverse matrix</returns>
		matrix3	Inverse() const noexcept;

		/// <summary>
		/// Checks whether this is an identity matrix.
		/// </summary>
		/// <returns>true if this is an identity matrix</returns>
		bool IsIdentity() const noexcept;

		/// <summary>
		/// Returns the transpose of this matrix
		/// </summary>
		/// <returns>transposed matrix</returns>
		matrix3	Transpose() const noexcept;

		/// <summary>
		/// Returns the stored column at index (0..2).
		/// </summary>
		/// <param name="index">column index in the range 0..2</param>
		/// <returns>selected matrix column</returns>
		sml::vector3 GetColumn(const u32 index) const;
		/// <summary>
		/// Returns the row assembled from the columns at index (0..2).
		/// </summary>
		/// <param name="index">row index in the range 0..2</param>
		/// <returns>selected matrix row</returns>
		sml::vector3 GetRow(const u32 index) const;

		matrix3 operator+(const matrix3& other) const;
		matrix3 operator-(const matrix3& other) const;

		matrix3 operator*(const f32& scalar) const;

		matrix3 operator*(const matrix3& other) const;
		vector3 operator*(const vector3& other) const;

		matrix3& operator=(const matrix3& other);

		bool operator==(const matrix3& other) const;
		bool operator!=(const matrix3& other) const;

		const f32* Data() const noexcept { return &_data[0]; }
		f32* Data() noexcept { return &_data[0]; }
	};
}

#endif