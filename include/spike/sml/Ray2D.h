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

#ifndef __SPIKE_SML_RAY2D_H
#define __SPIKE_SML_RAY2D_H

/*
----------------------------------------------------------------------

File Summary:
	Ray2D struct - Representation of a 2D direction.
	Contains constructors and methods for ray creation and point
	acquisition.

----------------------------------------------------------------------
*/

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

#include "Vector2.h"

namespace sml {
	/// <summary>
	/// Representation of a 2D direction originating from a world position
	/// </summary>
	struct Ray2D {
		vector2 origin, direction;

		Ray2D() noexcept : origin(vector2::zero), direction(vector2::up) {}
		Ray2D(const vector2& o, const vector2& d) noexcept : origin(o), direction(d) {}

		/// <summary>
		/// returns a point along the rays path
		/// </summary>
		/// <param name="dist">- distance along the rays path</param>
		/// <returns>world position at distance dist along the ray</returns>
		vector2 GetPoint(f32 dist) const noexcept;

		/// <summary>
		/// Returns a formatted string for this Ray
		/// </summary>
		/// <returns>string representation of this ray</returns>
		string ToString() const noexcept;
	};
}

#endif