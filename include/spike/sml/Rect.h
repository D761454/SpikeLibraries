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

#ifndef __SPIKE_SML_RECT_H
#define __SPIKE_SML_RECT_H

/*
----------------------------------------------------------------------

File Summary:
	rect struct - Representation of a 2D rectangle, defined by its
	width, height and position. Contains constructors, operators,
	and accessor methods for size and position data.

----------------------------------------------------------------------
*/

#include <spike/def_aliases.h>
#include <spike/stl_aliases.h>

#include "Vector2.h"

namespace sml {
	/// <summary>
	/// A 2D Rectangle defined by X and Y position, width and height.
	/// </summary>
	struct Rect {
		f32 x, y, width, height;

		static Rect zero;

		Rect() noexcept : x(0.0f), y(0.0f), width(0.0f), height(0.0f) {}
		Rect(f32 ix, f32 iy, f32 iwidth, f32 iheight) noexcept : x(ix), y(iy), width(iwidth), height(iheight) {}

		inline vector2 Center() const noexcept { return { x + (width / 2), y + (height / 2) }; }

		inline vector2 Max() const noexcept { return { x + width, y + height }; }
		inline vector2 Min() const noexcept { return { x, y }; }

		inline vector2 Position() const noexcept { return { x, y }; }

		inline vector2 Size() const noexcept { return { width, height }; }

		inline f32 XMax() const noexcept { return x + width; }
		inline f32 XMin() const noexcept { return x; }
		inline f32 YMax() const noexcept { return y + height; }
		inline f32 YMin() const noexcept { return y; }

		inline void Set(const Rect& N) noexcept { x = N.x; y = N.y; width = N.width; height = N.height; }
		inline void Set(f32 ix, f32 iy, f32 iwidth, f32 iheight) noexcept { x = ix; y = iy; width = iwidth; height = iheight; }

		bool Contains(const vector2& point) const noexcept;
		bool Overlaps(const Rect& other) const noexcept;

		static Rect	MinMaxRect(f32 xmin, f32 ymin, f32 xmax, f32 ymax) noexcept;
		static Rect	MinMaxRect(const vector2& min, const vector2& max) noexcept;

		Rect operator*(const f32 scalar) const;
		Rect operator*(const vector2& other) const;

		bool operator==(const Rect& other) const;
		bool operator!=(const Rect& other) const;

		/// <summary>
		/// Returns a formatted string for this Rect
		/// </summary>
		/// <returns>string representation of this rectangle</returns>
		string ToString() const noexcept;
	};
}

#endif