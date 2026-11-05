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

#ifndef __SPIKE_SML_MATRIX4_H
#define __SPIKE_SML_MATRIX4_H

/*
----------------------------------------------------------------------

File Summary:
	matrix4 struct - Representation of a 4x4 matrix.
	Contains constructors, operators, and static methods for common
	matrix operations.

----------------------------------------------------------------------
*/

#include <cassert>

#include <spike/def_aliases.h>
#include <spike/stl_aliases.h>

namespace sml {
	struct vector3;
	struct quaternion;
	struct FrustumPlane;

	/// <summary>
	/// A standard 4x4 transformation matrix.
	/// Column-major notation is used as in OpenGL: the internal storage is an array of 4 columns.
	/// Each MatrixRow instance therefore represents a column and elements are accessed as data[column][row].
	/// The translation (position) component is stored in the right-most column (column index 3), rows 0..2.
	/// These matrices are rarely used, only in special cases such as creating non-standard camera projection.
	/// </summary>
	struct matrix4 {
	public:
		/// <summary>
		/// Representation of 4 floats within a matrix column.
		/// This struct provides simple access overloads for the 4 elements of a column.
		/// Note: although named MatrixRow for historical reasons, instances represent columns in memory.
		/// Use matrix[column][row] to access element at (row, column).
		/// </summary>
		struct MatrixRow {
		private:
			f32 data[4];

		public:
			static MatrixRow zero;

			MatrixRow() noexcept : data{} {}
			MatrixRow(f32 a, f32 b, f32 c, f32 d) noexcept : data{ a, b, c, d } {}

			/// <summary>Returns the number of zero elements in this column.</summary>
			/// <returns>number of zero elements</returns>
			u8 ZeroCount() const {
				u8 count = 0;
				for (int i = 0; i < 4; i++) {
					if (data[i] == 0) {
						count++;
					}
				}
				return count;
			}

			/// <summary>
			/// Access the element at the given index within this column.
			/// Index is the row (0..3).
			/// </summary>
			/// <param name="index">row index in the range 0..3</param>
			/// <returns>reference to the selected element</returns>
			inline f32& operator[](const u32 index) {
				assert(index < 4 && "Index Out of Range! MatrixRow data in range 0-3.");

				return data[index];
			}

			/// <summary>
			/// Const access to the element at the given row index in this column.
			/// </summary>
			/// <param name="index">row index in the range 0..3</param>
			/// <returns>selected element</returns>
			inline const f32 operator[](const u32 index) const {
				assert(index < 4 && "Index Out of Range! MatrixRow data in range 0-3.");

				return data[index];
			}

			MatrixRow operator+(const MatrixRow& other) const;
			MatrixRow operator-(const MatrixRow& other) const;
			MatrixRow operator*(const f32& scalar) const;
			f32 operator*(const MatrixRow& other) const;

			bool operator==(const MatrixRow& other) const;
			bool operator!=(const MatrixRow& other) const;

			string ToString() const noexcept;
		};

	private:
		// Each element in data[] is a column (column-major). Access element (row r, column c) as data[c][r]
		MatrixRow data[4];

	public:
		static matrix4 identity;
		static matrix4 zero;

		matrix4() noexcept : data{} {}
		matrix4(MatrixRow a, MatrixRow b, MatrixRow c, MatrixRow d) noexcept : data{ a, b, c, d } {}

		/// <summary>
		/// Access a column by index (0..3).
		/// operator[] returns the column at the specified index. To read/write element at (row, col) use matrix[col][row].
		/// </summary>
		/// <param name="index">column index in the range 0..3</param>
		/// <returns>reference to the selected column</returns>
		inline MatrixRow& operator[](const u32 index) {
			assert(index < 4 && "Index Out of Range! matrix4 data in range 0-3.");

			return data[index];
		}

		/// <summary>
		/// Const access to a column by index (0..3).
		/// </summary>
		/// <param name="index">column index in the range 0..3</param>
		/// <returns>selected matrix column</returns>
		inline const MatrixRow operator[](const u32 index) const {
			assert(index < 4 && "Index Out of Range! matrix4 data in range 0-3.");

			return data[index];
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
		matrix4	Inverse() const noexcept;

		/// <summary>
		/// Checks whether this is an identity matrix.
		/// </summary>
		/// <returns>true if this is an identity matrix</returns>
		bool IsIdentity() const noexcept;

		/// <summary>
		/// Attempts to get a rotation quaternion from this matrix (reads upper-left 3x3).
		/// </summary>
		/// <returns>rotation represented by this matrix</returns>
		quaternion Rotation() const noexcept;

		/// <summary>
		/// Attempots to get a scale vector from this matrix (reads upper-left 3x3).
		/// </summary>
		/// <returns>scale vector represented by this matrix</returns>
		vector3 Scale() const noexcept;

		/// <summary>
		/// Returns the transpose of this matrix
		/// </summary>
		/// <returns>transposed matrix</returns>
		matrix4	Transpose() const noexcept;

		/// <summary>
		/// Get position vector from the matrix (reads column 3 rows 0..2).
		/// </summary>
		/// <returns>position stored in this matrix</returns>
		vector3	GetPosition() const noexcept;

		/// <summary>
		/// Transforms a position by this matrix (uses w=1 and performs perspective divide if needed).
		/// </summary>
		/// <param name="point">position to transform</param>
		/// <returns>transformed position</returns>
		vector3	MultiplyPoint(const vector3& point) const noexcept;

		/// <summary>
		/// Transforms a direction by this matrix (uses w=0, ignores translation).
		/// </summary>
		/// <param name="point">direction to transform</param>
		/// <returns>transformed direction</returns>
		vector3	MultiplyVector(const vector3& point) const noexcept;

		/// <summary>
		/// Sets this matrix to a translation, rotation and scaling matrix.
		/// </summary>
		/// <param name="pos">translation position</param>
		/// <param name="q">rotation quaternion</param>
		/// <param name="s">scale vector</param>
		void SetTRS(const vector3& pos, const quaternion& q, const vector3& s) noexcept;

		/// <summary>
		/// Checks if this matrix is a valid transform matrix.
		/// </summary>
		/// <returns>true if the matrix represents a valid transform</returns>
		bool ValidTRS() const noexcept;

		/// <summary>
		/// This function returns a projection matrix with viewing frustum that has a near plane defined by the coordinates that were passed in.
		/// The returned matrix is in column-major form suitable for OpenGL (use glUniformMatrix4fv(..., GL_FALSE, &matrix[0][0]) ).
		/// </summary>
		/// <param name="left">left frustum plane</param>
		/// <param name="right">right frustum plane</param>
		/// <param name="bottom">bottom frustum plane</param>
		/// <param name="top">top frustum plane</param>
		/// <param name="zNear">near clipping distance</param>
		/// <param name="zFar">far clipping distance</param>
		/// <returns>perspective frustum matrix</returns>
		static matrix4 Frustum(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar) noexcept;
		/// <summary>
		/// This function returns a projection matrix with viewing frustum that has a near plane defined by the coordinates that were passed in.
		/// </summary>
		/// <param name="frustum">frustum plane values</param>
		/// <returns>perspective frustum matrix</returns>
		static matrix4 Frustum(const FrustumPlane& frustum) noexcept;

		/// <summary>
		/// Create a "look at" matrix (column-major).
		/// </summary>
		/// <param name="from">camera position</param>
		/// <param name="to">look-at target</param>
		/// <param name="up">up direction</param>
		/// <returns>look-at view matrix</returns>
		static matrix4 LookAt(const vector3& from, const vector3& to, const vector3& up) noexcept;

		/// <summary>
		/// Create an orthogonal projection matrix (column-major).
		/// </summary>
		/// <param name="left">left boundary</param><param name="right">right boundary</param>
		/// <param name="bottom">bottom boundary</param><param name="top">top boundary</param>
		/// <param name="zNear">near clipping distance</param><param name="zFar">far clipping distance</param>
		/// <returns>orthographic projection matrix</returns>
		static matrix4 Ortho(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar) noexcept;
		/// <summary>Creates an orthographic projection matrix from frustum planes.</summary>
		/// <param name="fPlane">frustum plane values</param>
		/// <returns>orthographic projection matrix</returns>
		static matrix4 Ortho(FrustumPlane fPlane) noexcept;

		/// <summary>
		/// Create a perspective projection matrix (column-major).
		/// </summary>
		/// <param name="fov">vertical field of view</param>
		/// <param name="aspect">viewport aspect ratio</param>
		/// <param name="zNear">near clipping distance</param>
		/// <param name="zFar">far clipping distance</param>
		/// <returns>perspective projection matrix</returns>
		static matrix4 Perspective(f32 fov, f32 aspect, f32 zNear, f32 zFar) noexcept;

		/// <summary>
		/// Creates a rotation matrix (column-major).
		/// </summary>
		/// <param name="q">rotation quaternion</param>
		/// <returns>rotation matrix</returns>
		static matrix4 Rotate(const quaternion& q) noexcept;
		/// <summary>Applies a rotation to a matrix.</summary>
		/// <param name="m">matrix to modify</param><param name="q">rotation quaternion</param>
		static void	Rotate(matrix4& m, const quaternion& q) noexcept;

		/// <summary>
		/// Creates a scaling matrix (column-major).
		/// </summary>
		/// <param name="vector">scale vector</param>
		/// <returns>scaling matrix</returns>
		static matrix4 Scale(const vector3& vector) noexcept;
		/// <summary>Applies a scale to a matrix.</summary>
		/// <param name="m">matrix to modify</param><param name="vector">scale vector</param>
		static void Scale(matrix4& m, const vector3& vector) noexcept;

		/// <summary>
		/// Creates a translation matrix (column-major). Translation stored in column 3 rows 0..2.
		/// </summary>
		/// <param name="vector">translation vector</param>
		/// <returns>translation matrix</returns>
		static matrix4 Translate(const vector3& vector) noexcept;
		/// <summary>Applies a translation to a matrix.</summary>
		/// <param name="m">matrix to modify</param><param name="vector">translation vector</param>
		static void Translate(matrix4& m, const vector3& vector) noexcept;

		/// <summary>
		/// Creates a translation, rotation and scaling matrix (T * R * S).
		/// </summary>
		/// <param name="pos">translation position</param><param name="q">rotation quaternion</param>
		/// <param name="s">scale vector</param>
		/// <returns>translation, rotation and scale matrix</returns>
		static matrix4 TRS(const vector3& pos, const quaternion& q, const vector3& s) noexcept;

		/// <summary>
		/// In-place multiply: this = this * other (post-multiply)
		/// </summary>
		/// <param name="other">matrix to multiply by</param>
		void Multiply(const matrix4& other);
		/// <summary>
		/// In-place multiply: this = other * this (pre-multiply)
		/// </summary>
		/// <param name="other">matrix to multiply by</param>
		void PreMultiply(const matrix4& other);

		/// <summary>
		/// Apply a rotation to this matrix in-place.
		/// If local == true the rotation is applied in local/object space (post-multiply).
		/// If local == false the rotation is applied in world space (pre-multiply).
		/// </summary>
		/// <param name="q">rotation to apply</param>
		/// <param name="local">whether to apply in local space</param>
		void ApplyRotation(const quaternion& q, bool local = true);

		/// <summary>
		/// Apply a translation to this matrix in-place.
		/// If local == true translation is in local/object space (post-multiply).
		/// If local == false translation is in world space (pre-multiply).
		/// </summary>
		/// <param name="t">translation to apply</param>
		/// <param name="local">whether to apply in local space</param>
		void ApplyTranslation(const vector3& t, bool local = true);

		/// <summary>
		/// Apply a scale to this matrix in-place.
		/// If local == true scale is in local/object space (post-multiply).
		/// If local == false scale is in world space (pre-multiply).
		/// </summary>
		/// <param name="s">scale to apply</param>
		/// <param name="local">whether to apply in local space</param>
		void ApplyScale(const vector3& s, bool local = true);

		/// <summary>
		/// Returns the stored column at index (0..3).
		/// </summary>
		/// <param name="index">column index in the range 0..3</param>
		/// <returns>selected matrix column</returns>
		MatrixRow GetColumn(const u16 index) const;
		/// <summary>
		/// Returns the row assembled from the columns (row r = { data[0][r], data[1][r], data[2][r], data[3][r] }).
		/// </summary>
		/// <param name="index">row index in the range 0..3</param>
		/// <returns>selected matrix row</returns>
		MatrixRow GetRow(const u16 index) const;

		void SetColumn(const u16 index, const MatrixRow& column);
		void SetRow(const u16 index, const MatrixRow& row);

		matrix4 operator+(const matrix4& other) const;
		matrix4 operator-(const matrix4& other) const;

		matrix4 operator*(const f32& scalar) const;

		matrix4 operator*(const matrix4& other) const;
		vector3 operator*(const vector3& other) const;

		bool operator==(const matrix4& other) const;
		bool operator!=(const matrix4& other) const;

		/// <summary>
		/// Returns pointer to contiguous column-major data suitable for glUniformMatrix4fv(..., GL_FALSE, ptr)
		/// </summary>
		/// <returns>pointer to the contiguous column-major matrix data</returns>
		const f32* DataPtr() noexcept;

		/// <summary>
		/// Returns a formatted string for this matrix4
		/// </summary>
		/// <returns>string representation of this matrix4</returns>
		string ToString() const noexcept;
	};
}

// glUniformMatrix4fv(location, count, GL_FALSE, RCAST(&mvp[0][0], const GLfloat*))

#endif