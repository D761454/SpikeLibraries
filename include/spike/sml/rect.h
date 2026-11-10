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

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

#include "spike/sml/vector2.h"

namespace sml {
	/// <summary>
	/// A 2D Rectangle defined by X and Y position, width and height.
	/// </summary>
	struct Rect {
		f32 x, y, width, height;

		static Rect zero;

		Rect() noexcept : x(0.0f), y(0.0f), width(0.0f), height(0.0f) {}
		Rect(f32 ix, f32 iy, f32 iwidth, f32 iheight) noexcept : x(ix), y(iy), width(iwidth), height(iheight) {}

		/// <summary>
		/// Used to access the center position of the rect
		/// </summary>
		/// <returns>Thye center of the rect</returns>
		inline vector2 Center() const noexcept { return { x + (width / 2), y + (height / 2) }; }

		/// <summary>
		/// Used to access the maximum vertex of the rect
		/// </summary>
		/// <returns>The position of the top right vertex of the rect</returns>
		inline vector2 Max() const noexcept { return { x + width, y + height }; }

		/// <summary>
		/// Used to access the minimum vertex of the rect
		/// </summary>
		/// <returns>The position of the bottom left vertex of the rect</returns>
		inline vector2 Min() const noexcept { return { x, y }; }

		/// <summary>
		/// Access the position of the rect as a vector2
		/// </summary>
		/// <returns>Position of the rect as a vector2</returns>
		inline vector2 Position() const noexcept { return { x, y }; }

		/// <summary>
		/// Access the size of the rect as a vector2
		/// </summary>
		/// <returns>Size of the rect as a vector2</returns>
		inline vector2 Size() const noexcept { return { width, height }; }

		inline f32 XMax() const noexcept { return x + width; }
		inline f32 XMin() const noexcept { return x; }
		inline f32 YMax() const noexcept { return y + height; }
		inline f32 YMin() const noexcept { return y; }

		/// <summary>
		/// Set the rects values to equal that of the parameters
		/// </summary>
		/// <param name="N">rect containing the values to set this one to</param>
		inline void Set(const Rect& N) noexcept { x = N.x; y = N.y; width = N.width; height = N.height; }

		/// <summary>
		/// Set the rects values to equal that of the parameters
		/// </summary>
		/// <param name="ix">new x position</param>
		/// <param name="iy">new y position</param>
		/// <param name="iwidth">new width</param>
		/// <param name="iheight">new height</param>
		inline void Set(const f32 ix, const f32 iy, const f32 iwidth, const f32 iheight) noexcept { x = ix; y = iy; width = iwidth; height = iheight; }

		/// <summary>
		/// Used to determine if a 2D position is contained within the bounds of the rect
		/// </summary>
		/// <param name="point">position to check for overlap</param>
		/// <returns>Whether the point lies within the rects bounds</returns>
		bool Contains(const vector2& point) const noexcept;

		/// <summary>
		/// Used to determine if another rect overlaps with this rect
		/// </summary>
		/// <param name="other">rect to check</param>
		/// <returns>Whether the rect overlaps on either axis</returns>
		bool Overlaps(const Rect& other) const noexcept;

		/// <summary>
		/// Generates a rect based on the supplied parameters
		/// </summary>
		/// <param name="xmin">new position for the lower left vertex on the x axis</param>
		/// <param name="ymin">new position for the lower left vertex on the y axis</param>
		/// <param name="xmax">new position for the upper right vertex on the x axis</param>
		/// <param name="ymax">new position for the upper right vertex on the y axis</param>
		/// <returns>Newly generated rect object</returns>
		static Rect	MinMaxRect(f32 xmin, f32 ymin, f32 xmax, f32 ymax) noexcept;

		/// <summary>
		/// Generates a rect based on the supplied parameters
		/// </summary>
		/// <param name="min">new position for the lower left vertex</param>
		/// <param name="max">new position for the upper right vertex</param>
		/// <returns>Newly generated rect object</returns>
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