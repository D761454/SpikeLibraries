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

#ifndef __SPIKE_SML_QUATERNION_H
#define __SPIKE_SML_QUATERNION_H

/*
----------------------------------------------------------------------

File Summary:
	quaternion struct - Quaternion representation for rotations.
	Provides normalized rotation storage and interpolation methods.
	Note: callers must ensure quaternions used for repeated
	composition are normalized periodically to avoid drift.

----------------------------------------------------------------------
*/

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

#include "spike/sml/Vector3.h"

namespace sml {
	struct matrix4;

	/// <summary>
	/// Quaternions are used to represent rotations.
	/// Quaternions are an array of numbers {x, y, z, w}.
	/// </summary>
	struct quaternion {
	private:
		f32 x, y, z, w;

		/// <summary>
		/// Calculates the length (magnitude) of this quaternion.
		/// </summary>
		/// <returns>length of the quaternion</returns>
		const f32 Length() const noexcept;

	public:
		/// <summary>
		/// The identity rotation (no rotation applied).
		/// </summary>
		static quaternion identity;

		quaternion() noexcept : x(0.f), y(0.f), z(0.f), w(1.f) {}
		quaternion(const vector3& euler, const bool degrees = true) noexcept;

		/// <summary>
		/// Constructs a quaternion from an angle in degrees and an axis of rotation.
		/// </summary>
		/// <param name="angle">- rotation angle in degrees</param>
		/// <param name="axis">- axis of rotation (should be normalized)</param>
		quaternion(f32 angle, const vector3& axis) noexcept;
		quaternion(f32 ix, f32 iy, f32 iz, f32 iw) noexcept : x(ix), y(iy), z(iz), w(iw) {}

		// ensure POD
		quaternion(const quaternion& other) = default;
		quaternion(quaternion&& other) = default;
		quaternion& operator=(const quaternion& other) = default;
		quaternion& operator=(quaternion&& other) = default;
		// ensure POD

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
		/// Returns a normalized quaternion based on the current quaternion.
		/// The normalized quaternion has a magnitude of 1 and is in the same orientation as the current quaternion.
		/// </summary>
		/// <returns>normalized quaternion with magnitude of 1</returns>
		quaternion Normalized() const noexcept;

		/// <summary>
		/// Returns the Euler angle representation of the rotation.
		/// </summary>
		/// <param name="degrees">- if true returns result in degrees, otherwise radians</param>
		/// <returns>vector3 representing Euler angles</returns>
		vector3 EulerAngles(const bool degrees = true) const noexcept;
		vector3 EulerAnglesAlt(const bool degrees = true, const vector3* primaryEuler = nullptr) const noexcept;

		/// <summary>
		/// Set x, y, z and w components to those of the input quaternion.
		/// </summary>
		/// <param name="N">- source quaternion</param>
		inline void Set(const quaternion& N) noexcept { x = N.x; y = N.y; z = N.z; w = N.w; }

		/// <summary>
		/// Set x, y, z and w components to the input values.
		/// </summary>
		/// <param name="ix">- X component</param>
		/// <param name="iy">- Y component</param>
		/// <param name="iz">- Z component</param>
		/// <param name="iw">- W component</param>
		inline void Set(f32 ix, f32 iy, f32 iz, f32 iw) noexcept { x = ix; y = iy; z = iz; w = iw; }

		/// <summary>
		/// Creates a rotation with the specified forward and upwards directions.
		/// </summary>
		/// <param name="view">- forward direction</param>
		/// <param name="up">- upward direction (defaults to +Y)</param>
		void SetLookRotation(const vector3& view, const vector3& up = vector3::up) noexcept;

		/// <summary>
		/// Converts a rotation to angle-axis representation (degrees).
		/// </summary>
		/// <param name="angle">- output rotation angle in degrees</param>
		/// <param name="axis">- output rotation axis</param>
		void ToAngleAxis(float& angle, vector3& axis) const noexcept;

		/// <summary>
		/// Returns the angle in degrees between two rotations.
		/// </summary>
		/// <param name="lhs">- first quaternion</param>
		/// <param name="rhs">- second quaternion</param>
		/// <returns>angle between rotations in degrees</returns>
		static f32 Angle(const quaternion& lhs, const quaternion& rhs) noexcept;

		/// <summary>
		/// Calculates the dot product between two quaternions.
		/// </summary>
		/// <param name="a">- first quaternion</param>
		/// <param name="b">- second quaternion</param>
		/// <returns>dot product scalar value</returns>
		static f32 Dot(const quaternion& a, const quaternion& b) noexcept;

		/// <summary>
		/// Returns the Conjugate of the quaternion (all vector components negated).
		/// </summary>
		/// <param name="quat">- input quaternion</param>
		/// <returns>conjugate quaternion</returns>
		static quaternion Conjugate(const quaternion& quat) noexcept;

		/// <summary>
		/// Returns the Negated quaternion (all components multiplied by -1).
		/// </summary>
		/// <param name="quat">- input quaternion</param>
		/// <returns>negated quaternion</returns>
		static quaternion Negate(const quaternion& quat) noexcept;

		/// <summary>
		/// Returns the Inverse of the rotation.
		/// </summary>
		/// <param name="rotation">- input rotation quaternion</param>
		/// <returns>inverse rotation quaternion</returns>
		static quaternion Inverse(const quaternion& rotation) noexcept;

		/// <summary>
		/// Converts input Euler angles specified as three floats to a quaternion.
		/// </summary>
		/// <param name="ix">- rotation around X axis</param>
		/// <param name="iy">- rotation around Y axis</param>
		/// <param name="iz">- rotation around Z axis</param>
		/// <param name="degrees">- if true input is in degrees, otherwise radians</param>
		/// <returns>quaternion representing the rotation</returns>
		static quaternion Euler(f32 ix, f32 iy, f32 iz, const bool degrees = true) noexcept;

		/// <summary>
		/// Converts input Euler angles specified as a vector3 to a quaternion.
		/// </summary>
		/// <param name="euler">- Euler angles (X, Y, Z)</param>
		/// <param name="degrees">- if true input is in degrees, otherwise radians</param>
		/// <returns>quaternion representing the rotation</returns>
		static quaternion Euler(const vector3& euler, const bool degrees = true) noexcept;

		/// <summary>
		/// Interpolates between a and b by t and returns the normalized result (NLerp).
		/// </summary>
		/// <param name="a">- start rotation</param>
		/// <param name="b">- end rotation</param>
		/// <param name="t">- interpolation factor (typically in range [0, 1])</param>
		/// <returns>normalized interpolated quaternion</returns>
		static quaternion Lerp(const quaternion& a, const quaternion& b, f32 t) noexcept;

		/// <summary>
		/// Spherical linear interpolation between a and b (Shortest path).
		/// </summary>
		/// <param name="a">- start rotation</param>
		/// <param name="b">- end rotation</param>
		/// <param name="t">- interpolation factor (typically in range [0, 1])</param>
		/// <returns>interpolated quaternion via spherical linear interpolation</returns>
		static quaternion Slerp(const quaternion& a, const quaternion& b, f32 t) noexcept;

		/// <summary>
		/// Creates a rotation with the specified forward and upwards directions.
		/// </summary>
		/// <param name="forward">- forward direction</param>
		/// <param name="up">- upward direction (defaults to +Y)</param>
		/// <returns>quaternion representing the look rotation</returns>
		static quaternion LookRotation(const vector3& forward, const vector3& up = vector3::up) noexcept;

		/// <summary>
		/// Modifies this quaternion to ensure a magnitude of 1 while preserving orientation.
		/// </summary>
		/// <param name="v">- quaternion to normalize</param>
		/// <returns>normalized quaternion</returns>
		static quaternion Normalize(const quaternion& v) noexcept;

		/// <summary>
		/// Creates a rotation that rotates vector 'from' to vector 'to'.
		/// Handles nearly opposite vectors.
		/// </summary>
		/// <param name="from">- source direction</param>
		/// <param name="to">- target direction</param>
		/// <returns>quaternion rotating from to to</returns>
		static quaternion FromToRotation(const vector3& from, const vector3& to) noexcept;

		/// <summary>
		/// Interpolates between quaternions a and b with interpolation distance d.
		/// </summary>
		/// <param name="out">- output quaternion</param>
		/// <param name="a">- start rotation</param>
		/// <param name="b">- end rotation</param>
		/// <param name="d">- interpolation distance</param>
		static void Interpolate(quaternion& out, const quaternion& a, const quaternion& b, f32 d) noexcept;

		/// <summary>
		/// Converts this quaternion to a 4x4 rotation matrix.
		/// </summary>
		/// <returns>4x4 matrix representing this rotation</returns>
		matrix4 ToMatrix4() const noexcept;

		vector3 operator*(const vector3& v) const;
		quaternion operator*(const quaternion& other) const;

		bool operator==(const quaternion& other) const;
		bool operator!=(const quaternion& other) const;

		/// <summary>
		/// Returns a formatted string for this quaternion.
		/// </summary>
		/// <returns>string representation of this quaternion</returns>
		string ToString() const noexcept;
	};
}

#endif