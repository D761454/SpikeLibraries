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

#ifndef __SPIKE_SML_BOUNDING_SPHERE_H
#define __SPIKE_SML_BOUNDING_SPHERE_H

/*
----------------------------------------------------------------------

File Summary:
	BoundingSphere struct - Bounding sphere defined by position
	and radius.

----------------------------------------------------------------------
*/

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

#include "spike/sml/vector3.h"

namespace sml {
	/// <summary>
	/// Bounding sphere defined by position and radius.
	/// </summary>
	struct BoundingSphere {
		f32	radius;
		sml::vector3 position;

		BoundingSphere() noexcept : position(), radius(1.f) {}
		BoundingSphere(const sml::vector3& pos, f32 r) noexcept : position(pos), radius(r) {}

		// enusure POD
		BoundingSphere(const BoundingSphere& other) = default;
		BoundingSphere(BoundingSphere&& other) = default;
		BoundingSphere& operator=(const BoundingSphere& other) = default;
		BoundingSphere& operator=(BoundingSphere&& other) = default;
		// ensure POD

		/// <summary>
		/// Returns a formatted string for this bounding sphere.
		/// </summary>
		/// <returns>string representation of this bounding sphere</returns>
		string ToString() const noexcept;
	};
}

#endif