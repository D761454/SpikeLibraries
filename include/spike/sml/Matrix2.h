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

#ifndef __SPIKE_SML_MATRIX2_H
#define __SPIKE_SML_MATRIX2_H

/*
----------------------------------------------------------------------

File Summary:
	matrix2 struct - Representation of a 2x2 matrix.
	Contains constructors, operators, and static methods for common
	matrix operations.

----------------------------------------------------------------------
*/

#include <cassert>

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

namespace sml {
	struct vector2;

	/// <summary>
	/// A standard 2x2 matrix.
	/// </summary>
	struct matrix2 {
	protected:
		/// <summary>
		/// Representation of 2 floats within a matrix row or column.
		/// Contains only simple access overloads.
		/// </summary>
		struct MatrixRow {
		private:
			f32 data[2];

		public:
			static MatrixRow zero;

			MatrixRow() noexcept : data{} {}
			MatrixRow(f32 a, f32 b) noexcept : data{ a, b } {}

			/// <summary>Returns the number of zero elements in this row.</summary>
			/// <returns>number of zero elements</returns>
			u8 ZeroCount() const noexcept {
				u8 count = 0;
				for (int i = 0; i < 2; i++) {
					if (data[i] == 0) {
						count++;
					}
				}
				return count;
			}

			/// <summary>Returns a mutable element at the specified index.</summary>
			/// <param name="index">element index in the range 0..1</param>
			/// <returns>reference to the selected element</returns>
			inline f32& operator[](const u32 index) {
				assert(index < 2 && "Index Out of Range! MatrixRow data in range 0-1.");

				return data[index];
			}

			/// <summary>Returns an element at the specified index.</summary>
			/// <param name="index">element index in the range 0..1</param>
			/// <returns>selected element</returns>
			inline const f32 operator[](const u32 index) const {
				assert(index < 2 && "Index Out of Range! MatrixRow data in range 0-1.");

				return data[index];
			}

			MatrixRow operator+(const MatrixRow& other) const;
			MatrixRow operator-(const MatrixRow& other) const;
			MatrixRow operator*(const f32& scalar) const;
			f32 operator*(const MatrixRow& other) const;
			MatrixRow& operator=(const MatrixRow& other);

			bool operator==(const MatrixRow& other) const;
			bool operator!=(const MatrixRow& other) const;

			/// <summary>Returns a formatted string for this row.</summary>
			/// <returns>string representation of this row</returns>
			string ToString() const noexcept;
		};

	private:
		MatrixRow data[2];

	public:
		static matrix2 identity;
		static matrix2 zero;

		matrix2() noexcept : data{} {}
		matrix2(MatrixRow a, MatrixRow b) noexcept : data{ a, b } {}

		/// <summary>
		/// Access a column by index (0..1).
		/// operator[] returns the column at the specified index. To read/write element at (row, col) use matrix[col][row].
		/// </summary>
		/// <param name="index">column index in the range 0..1</param>
		/// <returns>reference to the selected column</returns>
		inline MatrixRow& operator[](const u32 index) {
			assert(index < 2 && "Index Out of Range! matrix2 data in range 0-1.");

			return data[index];
		}

		/// <summary>
		/// Const access to a column by index (0..1).
		/// </summary>
		/// <param name="index">column index in the range 0..1</param>
		/// <returns>selected matrix column</returns>
		inline const MatrixRow operator[](const u32 index) const {
			assert(index < 2 && "Index Out of Range! matrix2 data in range 0-1.");

			return data[index];
		}

		/// <summary>
		/// Returns the stored column at index (0..1).
		/// </summary>
		/// <param name="index">column index in the range 0..1</param>
		/// <returns>selected matrix column</returns>
		MatrixRow GetColumn(const u16 index) const;
		/// <summary>
		/// Returns the row assembled from the columns (row r = { data[0][r], data[1][r] }).
		/// </summary>
		/// <param name="index">row index in the range 0..1</param>
		/// <returns>selected matrix row</returns>
		MatrixRow GetRow(const u16 index) const;

		matrix2 operator+(const matrix2& other) const;
		matrix2 operator-(const matrix2& other) const;

		matrix2 operator*(const f32& scalar) const;

		matrix2 operator*(const matrix2& other) const;
		vector2 operator*(const vector2& other) const;

		matrix2& operator=(const matrix2& other);

		bool operator==(const matrix2& other) const;
		bool operator!=(const matrix2& other) const;

		/// <summary>
		/// Returns a formatted string for this matrix2
		/// </summary>
		/// <returns>string representation of this matrix2</returns>
		string ToString() const noexcept;
	};
}

#endif