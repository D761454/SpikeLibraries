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

#ifndef __SPIKE_SML_COLOUR_H
#define __SPIKE_SML_COLOUR_H

/*
----------------------------------------------------------------------

File Summary:
	colour24  struct - RGB color representation using 8-bit integers.
	colour32  struct - RGBA color representation using 8-bit integers.
	colour96  struct - RGB color representation using 32-bit floats (0.0-1.0).
	colour128 struct - RGBA color representation using 32-bit floats (0.0-1.0).

----------------------------------------------------------------------
*/

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

#include "spike/sml/vector3.h"
#include "spike/sml/vector4.h"

namespace sml {
	struct colour96;
	struct colour128;

	struct colour24 {
		u8 r, g, b;

		colour24() noexcept : r(0), g(0), b(0) {}
		colour24(u8 ir, u8 ig, u8 ib) noexcept;
		explicit colour24(u8 rgb);

		// ensure POD
		colour24(const colour24& other) = default;
		colour24(colour24&& other) = default;
		colour24& operator=(const colour24& other) = default;
		colour24& operator=(colour24&& other) = default;
		// ensure POD

		/// <summary>
		/// Linearly interpolates between two colours by t.
		/// </summary>
		/// <param name="a">- start colour</param>
		/// <param name="b">- end colour</param>
		/// <param name="t">- interpolation factor</param>
		/// <returns>interpolated colour</returns>
		static colour24 Lerp(const colour24& a, const colour24& b, f32 t) noexcept;

		/// <summary>
		/// Converts this colour to vector3 with normalized components (0.0-1.0).
		/// </summary>
		/// <returns>vector3 with normalized RGB values</returns>
		sml::vector3 To01RangeV() const noexcept;
		/// <summary>
		/// Converts this colour to vector4 with normalized components (0.0-1.0) and alpha set to 1.0.
		/// </summary>
		/// <returns>vector4 with normalized RGBA values</returns>
		sml::vector4 To01RangeVA() const noexcept;
		/// <summary>
		/// Converts this colour to colour96 with normalized components (0.0-1.0).
		/// </summary>
		/// <returns>colour96 representation</returns>
		sml::colour96 To01Range() const noexcept;
		/// <summary>
		/// Converts this colour to colour128 with normalized components (0.0-1.0) and alpha set to 1.0.
		/// </summary>
		/// <returns>colour128 representation</returns>
		sml::colour128 To01RangeA() const noexcept;

		bool operator==(const colour24& other) const {
			return r == other.r && g == other.g && b == other.b;
		}

		bool operator!=(const colour24& other) const {
			return r != other.r || g != other.g || b != other.b;
		}

		/// <summary>
		/// Returns a formatted string for this colour.
		/// </summary>
		/// <returns>string representation of this colour</returns>
		string ToString() const noexcept;

		static colour24 white;
		static colour24 black;
		static colour24 red;
		static colour24 green;
		static colour24 blue;
		static colour24 dbgBlue;
		static colour24 brown;
		static colour24 gray;
		static colour24 grey;
		static colour24 magenta;
		static colour24 orange;
		static colour24 pink;
		static colour24 purple;
		static colour24 silver;
		static colour24 snow;
		static colour24 violet;
		static colour24 yellow;

		static colour24 clear;
		static colour24 aliceBlue;
	};

	struct colour32 {
		u8 r, g, b, a;

		colour32() noexcept : r(0), g(0), b(0), a(255) {}
		colour32(u8 ir, u8 ig, u8 ib, u8 ia = 255) noexcept;
		colour32(u8 rgb, u8 ia) noexcept;
		colour32(const colour24& col, u8 ia = 255) noexcept;

		// ensure POD
		colour32(const colour32& other) = default;
		colour32(colour32&& other) = default;
		colour32& operator=(const colour32& other) = default;
		colour32& operator=(colour32&& other) = default;
		// ensure POD

		/// <summary>
		/// Linearly interpolates between two colours by t.
		/// </summary>
		/// <param name="a">- start colour</param>
		/// <param name="b">- end colour</param>
		/// <param name="t">- interpolation factor</param>
		/// <returns>interpolated colour</returns>
		static colour32 Lerp(const colour32& a, const colour32& b, f32 t) noexcept;

		/// <summary>
		/// Converts this colour to vector4 with normalized components (0.0-1.0).
		/// </summary>
		/// <returns>vector4 with normalized RGBA values</returns>
		sml::vector4 To01RangeV() const noexcept;
		/// <summary>
		/// Converts this colour to colour128 with normalized components (0.0-1.0).
		/// </summary>
		/// <returns>colour128 representation</returns>
		sml::colour128 To01Range() const noexcept;

		bool operator==(const colour32& other) const {
			return r == other.r && g == other.g && b == other.b && a == other.a;
		}

		bool operator!=(const colour32& other) const {
			return r != other.r || g != other.g || b != other.b || a != other.a;
		}

		/// <summary>
		/// Returns a formatted string for this colour.
		/// </summary>
		/// <returns>string representation of this colour</returns>
		string ToString() const noexcept;

		static colour32 white;
		static colour32 black;
		static colour32 red;
		static colour32 green;
		static colour32 blue;
		static colour32 dbgBlue;
		static colour32 brown;
		static colour32 gray;
		static colour32 grey;
		static colour32 magenta;
		static colour32 orange;
		static colour32 pink;
		static colour32 purple;
		static colour32 silver;
		static colour32 snow;
		static colour32 violet;
		static colour32 yellow;

		static colour32 clear;
		static colour32 aliceBlue;
	};

	struct colour96 {
		f32 r, g, b;

		colour96() noexcept : r(0.f), g(0.f), b(0.f) {}
		colour96(f32 ir, f32 ig, f32 ib) noexcept;
		explicit colour96(f32 rgb);

		// ensure POD
		colour96(const colour96& other) = default;
		colour96(colour96&& other) = default;
		colour96& operator=(const colour96& other) = default;
		colour96& operator=(colour96&& other) = default;
		// ensure POD

		/// <summary>
		/// Linearly interpolates between two colours by t.
		/// </summary>
		/// <param name="a">- start colour</param>
		/// <param name="b">- end colour</param>
		/// <param name="t">- interpolation factor</param>
		/// <returns>interpolated colour</returns>
		static colour96 Lerp(const colour96& a, const colour96& b, f32 t) noexcept;

		bool operator==(const colour96& other) const {
			return r == other.r && g == other.g && b == other.b;
		}

		bool operator!=(const colour96& other) const {
			return r != other.r || g != other.g || b != other.b;
		}

		/// <summary>
		/// Returns a formatted string for this colour.
		/// </summary>
		/// <returns>string representation of this colour</returns>
		string ToString() const noexcept;
	};

	struct colour128 {
		f32 r, g, b, a;

		colour128() noexcept : r(0.f), g(0.f), b(0.f), a(1.f) {}
		colour128(f32 ir, f32 ig, f32 ib, f32 ia = 1.f) noexcept;
		colour128(f32 rgb, f32 ia = 1.f) noexcept;
		colour128(const colour96& col, f32 ia = 1.f) noexcept;

		// ensure POD
		colour128(const colour128& other) = default;
		colour128(colour128&& other) = default;
		colour128& operator=(const colour128& other) = default;
		colour128& operator=(colour128&& other) = default;
		// ensure POD

		/// <summary>
		/// Linearly interpolates between two colours by t.
		/// </summary>
		/// <param name="a">- start colour</param>
		/// <param name="b">- end colour</param>
		/// <param name="t">- interpolation factor</param>
		/// <returns>interpolated colour</returns>
		static colour128 Lerp(const colour128& a, const colour128& b, f32 t) noexcept;

		bool operator==(const colour128& other) const {
			return r == other.r && g == other.g && b == other.b && a == other.a;
		}

		bool operator!=(const colour128& other) const {
			return r != other.r || g != other.g || b != other.b || a != other.a;
		}

		/// <summary>
		/// Returns a formatted string for this colour.
		/// </summary>
		/// <returns>string representation of this colour</returns>
		string ToString() const noexcept;
	};
}

#endif