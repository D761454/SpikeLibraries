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

#include "spike/sml/rect.h"

#include <string>

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

#include "spike/sml/vector2.h"

namespace sml {
	Rect Rect::zero = {};

	bool Rect::Contains(const vector2& point) const noexcept {
		return (point.x >= x && point.y >= y) && (point.x <= (x + width) && point.y <= (y + height));
	}

	bool Rect::Overlaps(const Rect& other) const noexcept {
		const vector2 min = Min();
		const vector2 max = Max();
		const vector2 omin = other.Min();
		const vector2 omax = other.Max();

		if (max.x < omin.x || min.x > omax.x) return false;
		if (max.y < omin.y || min.y > omax.y) return false;
		return true;

		return false;
	}

	Rect Rect::MinMaxRect(f32 xmin, f32 ymin, f32 xmax, f32 ymax) noexcept {
		return { xmin, ymin, xmax - xmin, ymax - ymin };
	}

	Rect Rect::MinMaxRect(const vector2& min, const vector2& max) noexcept {
		return { min.x, min.y, max.x - min.x, max.y - min.y };
	}

	Rect Rect::operator*(const f32 scalar) const {
		Rect t{ x, y, width, height };

		t.height *= scalar;
		t.width *= scalar;

		return t;
	}

	Rect Rect::operator*(const vector2& other) const {
		Rect t{ x, y, width, height };

		t.height *= other.y;
		t.width *= other.x;

		return t;
	}

	bool Rect::operator==(const Rect& other) const {
		return (x == other.x && y == other.y && width == other.width && height == other.height);
	}

	bool Rect::operator!=(const Rect& other) const {
		return (x != other.x || y != other.y || width != other.width || height != other.height);
	}

	string Rect::ToString() const noexcept {
		return "{" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(width) + ", " + std::to_string(height) + "}";
	}
}