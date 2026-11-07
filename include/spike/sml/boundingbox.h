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

#ifndef __SPIKE_SML_BOUNDING_BOX_H
#define __SPIKE_SML_BOUNDING_BOX_H

/*
----------------------------------------------------------------------

File Summary:
	BoundingBox struct - Axis-aligned bounding box defined by center
	and size. Provides methods for containment checks, collision
	detection, and point encapsulation.

----------------------------------------------------------------------
*/

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

#include "spike/sml/vector3.h"
#include "spike/sml/ray.h"

namespace sml {
	/// <summary>
	/// Axis-aligned bounding box defined by center and size.
	/// </summary>
	struct BoundingBox {
		// change def to just center and size (or extents)
		sml::vector3 center, extents;

		BoundingBox() noexcept : center(0, 0, 0), extents(0.5f, 0.5f, 0.5f) {}
		BoundingBox(const sml::vector3& icenter, const sml::vector3& isize) noexcept;

		// enusure POD
		BoundingBox(const BoundingBox& other) = default;
		BoundingBox(BoundingBox&& other) = default;
		BoundingBox& operator=(const BoundingBox& other) = default;
		BoundingBox& operator=(BoundingBox&& other) = default;
		// ensure POD

		inline sml::vector3 Min() const noexcept {
			return center - extents;
		}

		inline sml::vector3 Max() const noexcept {
			return center + extents;
		}

		inline sml::vector3 Size() const noexcept {
			return extents * 2;
		}

		/// <summary>
		/// Finds the closest point on or inside this bounding box to the given point.
		/// </summary>
		/// <param name="point">- the external point</param>
		/// <returns>closest point on or within the bounding box</returns>
		sml::vector3 ClosestPoint(const sml::vector3& point) const noexcept;

		/// <summary>
		/// Checks if the given point is contained within this bounding box.
		/// </summary>
		/// <param name="point">- the point to test</param>
		/// <returns>true if point is within the bounding box, false otherwise</returns>
		bool Contains(const sml::vector3& point) const noexcept;

		/// <summary>
		/// Expands this bounding box to encapsulate the given point.
		/// </summary>
		/// <param name="point">- the point to encapsulate</param>
		void Encapsulate(const sml::vector3& point) noexcept;

		/// <summary>
		/// Expands this bounding box by the specified amount in all directions.
		/// </summary>
		/// <param name="amount">- the expansion amount</param>
		void Expand(f32 amount) noexcept;

		/// <summary>
		/// Tests if a ray intersects this bounding box.
		/// </summary>
		/// <param name="ray">- the ray to test</param>
		/// <returns>true if ray intersects bounding box, false otherwise</returns>
		bool IntersectRay(const Ray& ray) const;

		/// <summary>
		/// Tests if this bounding box intersects with another bounding box.
		/// </summary>
		/// <param name="bounds">- the other bounding box</param>
		/// <returns>true if bounding boxes intersect, false otherwise</returns>
		bool Intersect(const BoundingBox& bounds) const noexcept;

		/// <summary>
		/// Sets the min and max bounds of this bounding box and recalculates derived values.
		/// </summary>
		/// <param name="min">- minimum corner</param>
		/// <param name="max">- maximum corner</param>
		void SetMinMax(const sml::vector3& min, const sml::vector3& max) noexcept;

		/// <summary>
		/// Returns a formatted string for this bounding box.
		/// </summary>
		/// <returns>string representation of this bounding box</returns>
		string ToString() const noexcept;
	};
}

#endif