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

#ifndef __SPIKE_SML_VECTOR2_H
#define __SPIKE_SML_VECTOR2_H

/*
----------------------------------------------------------------------

File Summary:
	vector2 struct - Representation of 2D vectors and points.
	Contains constructors, operators, and static methods for common
	vector operations including normalization, dot product, cross
	product, distance calculations, and linear interpolation.

----------------------------------------------------------------------
*/

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

namespace sml {
	/// <summary>
	/// Representation of 3D vectors and points.
	/// It is used throughout Spike to pass 3D positions and directions around.
	/// It also contains functions for doing common vector operations.
	/// </summary>
	struct vector3;

	/// <summary>
	/// Representation of 2D vectors and points.
	/// It is used throughout Spike to pass 2D positions and directions around.
	/// It also contains functions for doing common vector operations.
	/// </summary>
	struct vector2 {
		f32 x, y;

		vector2() noexcept : x(0.f), y(0.f) {}
		vector2(f32 ix, f32 iy) noexcept : x(ix), y(iy) {}
		explicit vector2(f32 xy) : x(xy), y(xy) {}

		vector2(const vector2& other) = default;
		vector2(vector2&& other) = default;
		vector2& operator=(const vector2& other) = default;
		vector2& operator=(vector2&& other) = default;

		inline f32 operator[](u32 i) const noexcept {
			switch (i) {
			case 0: return x;
			case 1: return y;
			default: return x;
			}
		}

		inline f32& operator[](u32 i) noexcept {
			switch (i) {
			case 0: return x;
			case 1: return y;
			default: return x;
			}
		}

		/// <summary>
		/// Shorthand for writing vector2(0, 1).
		/// </summary>
		static const vector2 up;
		/// <summary>
		/// Shorthand for writing vector2(0, -1).
		/// </summary>
		static const vector2 down;
		/// <summary>
		/// Shorthand for writing vector2(-1, 0).
		/// </summary>
		static const vector2 left;
		/// <summary>
		/// Shorthand for writing vector2(1, 0).
		/// </summary>
		static const vector2 right;
		/// <summary>
		/// Shorthand for writing vector2(1, 1).
		/// </summary>
		static const vector2 one;
		/// <summary>
		/// Shorthand for writing vector2(0, 0).
		/// </summary>
		static const vector2 zero;

		/// <summary>
		/// Returns the length of this vector.
		/// </summary>
		/// <returns>vector length</returns>
		f32 Magnitude() const noexcept;

		/// <summary>
		/// Returns a normalized vector based on the current vector.
		/// The normalized vector has a magnitude of 1 and is in the same direction as the current vector.
		/// </summary>
		/// <returns>vector with a magnitude of 1 and preserved direction</returns>
		vector2	Normalized() const noexcept;

		/// <summary>
		/// Set the X and Y components of this vector
		/// </summary>
		/// <param name="N">new vector data</param>
		inline void Set(const vector2& N) noexcept { x = N.x; y = N.y; }
		/// <summary>
		/// Set the X and Y components of this vector
		/// </summary>
		/// <param name="ix">new X value</param>
		/// <param name="iy">new Y value</param>
		inline void Set(f32 ix, f32 iy) noexcept { x = ix; y = iy; }

		/// <summary>
		/// Returns a vector that is made from the smallest components of two vectors.
		/// </summary>
		/// <param name="lhs">vector to compare</param>
		/// <param name="rhs">vector to compare</param>
		/// <returns>vector made from the smallest components of two vectors</returns>
		static vector2 Min(const vector2& lhs, const vector2& rhs) noexcept;
		/// <summary>
		/// Returns a vector that is made from the largest components of two vectors.
		/// </summary>
		/// <param name="lhs">vector to compare</param>
		/// <param name="rhs">vector to compare</param>
		/// <returns>vector made from the largest components of two vectors</returns>
		static vector2 Max(const vector2& lhs, const vector2& rhs) noexcept;

		/// <summary>
		/// Gets the unsigned angle in degrees between lhs and rhs.
		/// </summary>
		/// <param name="lhs">first vector</param>
		/// <param name="rhs">second vector</param>
		/// <returns>angle between the 2 vectors</returns>
		static f32 Angle(const vector2& lhs, const vector2& rhs) noexcept;
		/// <summary>
		/// Computes the 2D cross product (scalar) of two vectors.
		/// </summary>
		/// <returns>A float value representing the Cross Product</returns>
		static f32 Cross(const vector2& lhs, const vector2& rhs) noexcept;
		/// <summary>
		/// Dot Product of two vectors.
		/// </summary>
		/// <param name="lhs">first vector</param>
		/// <param name="rhs">second vector</param>
		/// <returns>Dot Product of two vectors.</returns>
		static f32 Dot(const vector2& lhs, const vector2& rhs) noexcept;
		/// <summary>
		/// Returns the distance between lhs and rhs.
		/// </summary>
		/// <returns>Distance between lhs and rhs</returns>
		static f32 Distance(const vector2& lhs, const vector2& rhs) noexcept;

		/// <summary>
		/// Modifies this vector to ensure a magnitude of 1 while preserving direction.
		/// </summary>
		/// <param name="v">- vector to normalize</param>
		/// <returns>vector with a magnitude of 1 and preserved direction</returns>
		static vector2 Normalize(const vector2& v) noexcept;
		/// <summary>
		/// Linearly interpolates between vectors lhs and rhs by t.
		/// </summary>
		/// <param name="lhs">start vector</param>
		/// <param name="rhs">end vector</param>
		/// <param name="t">interpolation factor</param>
		/// <returns>vector representing a point at t, between lhs and rhs</returns>
		static vector2 Lerp(const vector2& lhs, const vector2& rhs, f32 t) noexcept;
		/// <summary>
		/// Multiplies two vectors component-wise.
		/// </summary>
		/// <returns>Vector made from each input vectors components multiplied by the others</returns>
		static vector2 Scale(const vector2& lhs, const vector2& rhs) noexcept;

		vector2 operator+(const vector2& other) const;
		vector2 operator*(const f32 scalar) const;
		vector2 operator*(const vector2& other) const;
		vector2 operator-(const vector2& other) const;
		vector2 operator-() const;
		vector2 operator/(const f32 scalar) const;
		vector2 operator/(const vector2& other) const;

		vector2& operator+=(const vector2& other);
		vector2& operator*=(const vector2& other);
		vector2& operator*=(const f32 scalar);
		vector2& operator-=(const vector2& other);
		vector2& operator/=(const vector2& other);
		vector2& operator/=(const f32 scalar);

		bool operator>(const vector2& other) const;
		bool operator<(const vector2& other) const;
		bool operator>=(const vector2& other) const;
		bool operator<=(const vector2& other) const;

		bool operator==(const vector2& other) const;
		bool operator!=(const vector2& other) const;

		/// <summary>
		/// produces a vector3 where the X and Z values are from this vector, with Y set to 0
		/// </summary>
		/// <returns>vector3 with this vector's components in X and Z</returns>
		vector3 ToVector3XZ() const noexcept;
		/// <summary>
		/// produces a vector3 where the X and Y values are from this vector, with Z set to 0
		/// </summary>
		vector3 ToVector3XY() const noexcept;

		/// <summary>
		/// Returns a formatted string for this vector.
		/// </summary>
		/// <returns>string representation of this vector</returns>
		string ToString() const noexcept;
	};
}

#endif