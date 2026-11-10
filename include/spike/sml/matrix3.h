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

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

#include "spike/sml/vector3.h"

namespace sml {
	/// <summary>
	/// A standard 3x3 transformation matrix.
	/// Column-major notation is used as in OpenGL: the internal storage is an array of 3 columns.
	/// Each MatrixRow instance therefore represents a column and elements are accessed as data[column][row].
	/// </summary>
	struct matrix3 {
	protected:
		/// <summary>
		/// Representation of 3 floats within a matrix column.
		/// This struct provides simple access overloads for the 3 elements of a column.
		/// Note: although named MatrixRow for historical reasons, instances represent columns in memory.
		/// Use matrix[column][row] to access element at (row, column).
		/// </summary>
		struct MatrixRow {
		private:
			f32 data[3];

		public:
			static MatrixRow zero;

			MatrixRow() noexcept : data{} {}
			MatrixRow(f32 a, f32 b, f32 c) noexcept : data{ a, b, c } {}

			/// <summary>Returns the number of zero elements in this column.</summary>
			/// <returns>number of zero elements</returns>
			u8 ZeroCount() const {
				u8 count = 0;
				for (int i = 0; i < 3; i++) {
					if (data[i] == 0) {
						count++;
					}
				}
				return count;
			}

			/// <summary>
			/// Access the element at the given index within this column.
			/// Index is the row (0..2).
			/// </summary>
			/// <param name="index">row index in the range 0..2</param>
			/// <returns>reference to the selected element</returns>
			inline f32& operator[](const u32 index) {
				assert(index < 3 && "Index Out of Range! MatrixRow data in range 0-2.");

				return data[index];
			}

			/// <summary>
			/// Const access to the element at the given row index in this column.
			/// </summary>
			/// <param name="index">row index in the range 0..2</param>
			/// <returns>selected element</returns>
			inline const f32 operator[](const u32 index) const {
				assert(index < 3 && "Index Out of Range! MatrixRow data in range 0-2.");

				return data[index];
			}

			MatrixRow operator+(const MatrixRow& other) const;
			MatrixRow operator-(const MatrixRow& other) const;
			MatrixRow operator*(const f32& scalar) const;
			f32 operator*(const MatrixRow& other) const;

			MatrixRow& operator=(const MatrixRow& other);

			bool operator==(const MatrixRow& other) const;
			bool operator!=(const MatrixRow& other) const;

			string ToString() const noexcept;
		};

	private:
		// Each element in data[] is a column (column-major). Access element (row r, column c) as data[c][r]
		MatrixRow data[3];

	public:
		static matrix3 identity;
		static matrix3 zero;

		matrix3() noexcept : data{} {}
		matrix3(MatrixRow a, MatrixRow b, MatrixRow c) noexcept : data{ a, b, c } {}

		/// <summary>
		/// Access a column by index (0..2).
		/// operator[] returns the column at the specified index. To read/write element at (row, col) use matrix[col][row].
		/// </summary>
		/// <param name="index">column index in the range 0..2</param>
		/// <returns>reference to the selected column</returns>
		inline MatrixRow& operator[](const u32 index) {
			assert(index < 3 && "Index Out of Range! matrix3 data in range 0-2.");

			return data[index];
		}

		/// <summary>
		/// Const access to a column by index (0..2).
		/// </summary>
		/// <param name="index">column index in the range 0..2</param>
		/// <returns>selected matrix column</returns>
		inline const MatrixRow operator[](const u32 index) const {
			assert(index < 3 && "Index Out of Range! matrix3 data in range 0-2.");

			return data[index];
		}

		/// <summary>
		/// Returns the stored column at index (0..2).
		/// </summary>
		/// <param name="index">column index in the range 0..2</param>
		/// <returns>selected matrix column</returns>
		MatrixRow GetColumn(const u16 index) const;
		/// <summary>
		/// Returns the row assembled from the columns (row r = { data[0][r], data[1][r], data[2][r] }).
		/// </summary>
		/// <param name="index">row index in the range 0..2</param>
		/// <returns>selected matrix row</returns>
		MatrixRow GetRow(const u16 index) const;

		matrix3 operator+(const matrix3& other) const;
		matrix3 operator-(const matrix3& other) const;

		matrix3 operator*(const f32& scalar) const;

		matrix3 operator*(const matrix3& other) const;
		vector3 operator*(const vector3& other) const;

		matrix3& operator=(const matrix3& other);

		bool operator==(const matrix3& other) const;
		bool operator!=(const matrix3& other) const;

		/// <summary>
		/// Returns a formatted string for this matrix3
		/// </summary>
		/// <returns>string representation of this matrix3</returns>
		string ToString() const noexcept;
	};
}

#endif