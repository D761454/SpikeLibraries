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

#ifndef __SPIKE_SML_VECTOR4_H
#define __SPIKE_SML_VECTOR4_H

/*
----------------------------------------------------------------------

File Summary:
	vector4 struct - Representation of 4D vectors and points.
	Contains constructors, operators, and accessor methods for
	homogeneous coordinates and color data.

----------------------------------------------------------------------
*/

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

#include "spike/sml/vector3.h"

namespace sml {
	/// <summary>
	/// Representation of 4D vectors and points.
	/// It is used throughout Spike to pass 4D positions and directions around.
	/// It also contains functions for doing common vector operations.
	/// </summary>
	struct vector4 {
		f32 x, y, z, w;

		vector4() noexcept : x(0.f), y(0.f), z(0.f), w(0.f) {}
		vector4(f32 ix, f32 iy, f32 iz, f32 iw) noexcept : x(ix), y(iy), z(iz), w(iw) {}
		explicit vector4(f32 xyzw) : x(xyzw), y(xyzw), z(xyzw), w(xyzw) {}

		vector4(const vector4& other) = default;
		vector4(vector4&& other) = default;
		vector4& operator=(const vector4& other) = default;
		vector4& operator=(vector4&& other) = default;

		inline f32 operator[](u32 i) const {
			switch (i) {
			case 0: return x;
			case 1: return y;
			case 2: return z;
			case 3: return w;
			default: return x;
			}
		}

		inline f32& operator[](u32 i) {
			switch (i) {
			case 0: return x;
			case 1: return y;
			case 2: return z;
			case 3: return w;
			default: return x;
			}
		}

		/// <summary>
		/// Shorthand for writing vector4(0, 1, 0, 0).
		/// </summary>
		static const vector4 up;
		/// <summary>
		/// Shorthand for writing vector4(0, -1, 0, 0).
		/// </summary>
		static const vector4 down;
		/// <summary>
		/// Shorthand for writing vector4(-1, 0, 0, 0).
		/// </summary>
		static const vector4 left;
		/// <summary>
		/// Shorthand for writing vector4(1, 0, 0, 0).
		/// </summary>
		static const vector4 right;
		/// <summary>
		/// Shorthand for writing vector4(0, 0, 1, 0).
		/// </summary>
		static const vector4 front;
		/// <summary>
		/// Shorthand for writing vector4(0, 0, -1, 0).
		/// </summary>
		static const vector4 back;
		/// <summary>
		/// Shorthand for writing vector4(1, 1, 1, 0).
		/// </summary>
		static const vector4 one;
		/// <summary>
		/// Shorthand for writing vector4(0, 0, 0, 0).
		/// </summary>
		static const vector4 zero;

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
		vector4 Normalized() const noexcept;

		/// <summary>
		/// Set the X, Y, Z and W components of this vector
		/// </summary>
		/// <param name="N">new vector data</param>
		inline void Set(const vector4& N) noexcept { x = N.x; y = N.y; z = N.z; w = N.w; }
		/// <summary>
		/// Set the X, Y, Z and W components of this vector.
		/// </summary>
		/// <param name="ix">new X value</param>
		/// <param name="iy">new Y value</param>
		/// <param name="iz">new Z value</param>
		/// <param name="iw">new W value</param>
		inline void Set(f32 ix, f32 iy, f32 iz, f32 iw) noexcept { x = ix; y = iy; z = iz; w = iw; }

		/// <summary>
		/// Returns a vector that is made from the smallest components of two vectors.
		/// </summary>
		/// <param name="lhs">vector to compare</param>
		/// <param name="rhs">vector to compare</param>
		/// <returns>vector made from the smallest components of two vectors</returns>
		static vector4 Min(const vector4& lhs, const vector4& rhs) noexcept;
		/// <summary>
		/// Returns a vector that is made from the largest components of two vectors.
		/// </summary>
		/// <param name="lhs">vector to compare</param>
		/// <param name="rhs">vector to compare</param>
		/// <returns>vector made from the largest components of two vectors</returns>
		static vector4 Max(const vector4& lhs, const vector4& rhs) noexcept;

		/// <summary>
		/// Gets the unsigned angle in degrees between lhs and rhs.
		/// </summary>
		/// <param name="lhs">first vector</param>
		/// <param name="rhs">second vector</param>
		/// <returns>angle between the 2 vectors in degrees</returns>
		static f32 Angle(const vector4& lhs, const vector4& rhs) noexcept;
		/// <summary>
		/// Calculates the cross product of two three-dimensional vectors.
		/// </summary>
		/// <param name="lhs">first vector</param>
		/// <param name="rhs">second vector</param>
		/// <returns>cross product vector perpendicular to both inputs</returns>
		static vector4 Cross(const vector4& lhs, const vector4& rhs) noexcept;
		/// <summary>
		/// Calculates the dot product of two three-dimensional vectors defined in the same coordinate space.
		/// </summary>
		/// <param name="lhs">first vector</param>
		/// <param name="rhs">second vector</param>
		/// <returns>dot product scalar value</returns>
		static f32 Dot(const vector4& lhs, const vector4& rhs) noexcept;
		/// <summary>
		/// Returns the distance between lhs and rhs.
		/// </summary>
		/// <param name="lhs">first point</param>
		/// <param name="rhs">second point</param>
		/// <returns>distance between the two points</returns>
		static f32 Distance(const vector4& lhs, const vector4& rhs) noexcept;

		/// <summary>
		/// Modifies this vector to ensure a magnitude of 1 while preserving direction.
		/// </summary>
		/// <param name="v">vector to normalize</param>
		/// <returns>normalized vector with magnitude of 1 and preserved direction</returns>
		static vector4 Normalize(const vector4& v) noexcept;
		/// <summary>
		/// Linearly interpolates between vectors lhs and rhs by t.
		/// </summary>
		/// <param name="lhs">start vector</param>
		/// <param name="rhs">end vector</param>
		/// <param name="t">interpolation factor (typically in range [0, 1])</param>
		/// <returns>vector representing a point at t, between lhs and rhs</returns>
		static vector4 Lerp(const vector4& lhs, const vector4& rhs, f32 t) noexcept;
		/// <summary>
		/// Multiplies two vectors component-wise.
		/// </summary>
		/// <param name="lhs">first vector</param>
		/// <param name="rhs">second vector</param>
		/// <returns>component-wise product</returns>
		static vector4 Scale(const vector4& lhs, const vector4& rhs) noexcept;

		vector4 operator+(const vector4& other) const;
		vector4 operator*(const f32 scalar) const;
		vector4 operator*(const vector4& other) const;
		vector4 operator-(const vector4& other) const;
		vector4 operator-() const;
		vector4 operator/(const f32 scalar) const;
		vector4 operator/(const vector4& other) const;

		vector4& operator+=(const vector4& other);
		vector4& operator*=(const vector4& other);
		vector4& operator*=(const f32 scalar);
		vector4& operator-=(const vector4& other);
		vector4& operator/=(const vector4& other);
		vector4& operator/=(const f32 scalar);

		bool operator>(const vector4& other) const;
		bool operator<(const vector4& other) const;
		bool operator>=(const vector4& other) const;
		bool operator<=(const vector4& other) const;

		bool operator==(const vector4& other) const;
		bool operator!=(const vector4& other) const;

		/// <summary>
		/// Produces a vector 3 from the X, Y and Z components
		/// </summary>
		/// <returns>vector 3 version of this vector</returns>
		vector3 ToVector3() const noexcept;

		/// <summary>
		/// Returns a formatted string for this vector.
		/// </summary>
		/// <returns>string representation of this vector</returns>
		string ToString() const noexcept;
	};
}

#endif