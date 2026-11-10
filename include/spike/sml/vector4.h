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

		/// <summary>
		/// Set the X, Y, Z and W components of this vector.
		/// </summary>
		/// <param name="ix">new X value</param>
		/// <param name="iy">new Y value</param>
		/// <param name="iz">new Z value</param>
		/// <param name="iw">new W value</param>
		inline void Set(f32 ix, f32 iy, f32 iz, f32 iw) noexcept { x = ix; y = iy; z = iz; w = iw; }

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

		bool operator==(const vector4& other) const;
		bool operator!=(const vector4& other) const;

		/// <summary>
		/// Returns a formatted string for this vector.
		/// </summary>
		/// <returns>string representation of this vector</returns>
		string ToString() const noexcept;
	};
}

#endif