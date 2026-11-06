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

#include "spike/sml/BoundingBox.h"

#include <limits>

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

#include "spike/sml/Vector3.h"
#include "spike/sml/Ray.h"
#include "spike/sml/SMLCore.h"

// ============================================================================
// Constructors
// ============================================================================

sml::BoundingBox::BoundingBox(const sml::vector3& icenter, const sml::vector3& isize) noexcept {
	center = icenter;
	extents = isize * 0.5f;
}

// ============================================================================
// member Method Implementations
// ============================================================================

sml::vector3 sml::BoundingBox::ClosestPoint(const sml::vector3& point) const noexcept {
	sml::vector3 min = Min(), max = Max();

	return {
		sml::clamp(point.x, min.x, max.x),
		sml::clamp(point.y, min.y, max.y),
		sml::clamp(point.z, min.z, max.z)
	};
}

bool sml::BoundingBox::Contains(const sml::vector3& point) const noexcept {
	sml::vector3 min = Min(), max = Max();

	if (max.x < point.x) return false;
	if (max.y < point.y) return false;
	if (max.z < point.z) return false;

	if (min.x > point.x) return false;
	if (min.y > point.y) return false;
	if (min.z > point.z) return false;

	return true;
}

void sml::BoundingBox::Encapsulate(const sml::vector3& point) noexcept {
	if (Contains(point)) return;

	sml::vector3 min = Min(), max = Max();

	if (point.x > max.x) max.x = point.x;
	if (point.y > max.y) max.y = point.y;
	if (point.z > max.z) max.z = point.z;

	if (point.x < min.x) min.x = point.x;
	if (point.y < min.y) min.y = point.y;
	if (point.z < min.z) min.z = point.z;

	center = (min + max) * 0.5f;
	extents = (max - min) * 0.5f;
}

void sml::BoundingBox::Expand(f32 amount) noexcept {
	extents *= amount;
}

bool sml::BoundingBox::IntersectRay(const Ray& ray) const {
	const vector3& o = ray.origin, & d = ray.direction;
	constexpr f32 kEps = 1e-8f;
	constexpr f32 kFinf = std::numeric_limits<f32>::infinity();

	auto slab = [&](f32 origin, f32 dir, f32 bmin, f32 bmax, f32& outMin, f32& outMax) -> bool {
		if (sml::abs(dir) < kEps) {
			if (origin < bmin || origin > bmax) return false;
			outMin = -kFinf;
			outMax = kFinf;
			return true;
		}
		f32 invDir = 1.f / dir;
		f32 t1 = (bmin - origin) * invDir;
		f32 t2 = (bmax - origin) * invDir;
		outMin = sml::min(t1, t2);
		outMax = sml::max(t1, t2);
		return true;
		};

	sml::vector3 min = Min(), max = Max();

	f32 txMin, txMax, tyMin, tyMax, tzMin, tzMax;
	if (!slab(o.x, d.x, min.x, max.x, txMin, txMax)) return false;
	if (!slab(o.y, d.y, min.y, max.y, tyMin, tyMax)) return false;
	if (!slab(o.z, d.z, min.z, max.z, tzMin, tzMax)) return false;

	f32 tMin = sml::max(txMin, tyMin, tzMin);
	f32 tMax = sml::min(txMax, tyMax, tzMax);

	return (tMax >= tMin) && (tMax >= 0.0f);
}

bool sml::BoundingBox::Intersect(const BoundingBox& bounds) const noexcept {
	sml::vector3 min = Min(), max = Max(), bmax = bounds.Max(), bmin = bounds.Min();

	if (max.x < bmin.x || min.x > bmax.x) return false;
	if (max.y < bmin.y || min.y > bmax.y) return false;
	if (max.z < bmin.z || min.z > bmax.z) return false;
	return true;
}

void sml::BoundingBox::SetMinMax(const sml::vector3& min, const sml::vector3& max) noexcept {
	center = (min + max) * 0.5f;
	extents = (max - min) * 0.5f;
}

string sml::BoundingBox::ToString() const noexcept {
	return "{Center: " + center.ToString() + ", Extents: " + extents.ToString() + "}";
}