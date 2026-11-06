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

#include "spike/sml/Colour.h"

#include <string>

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

#include "spike/sml/Vector3.h"
#include "spike/sml/Vector4.h"
#include "spike/sml/SMLCore.h"

namespace sml {
	// ============================================================================
	// colour24 Static Constants
	// ============================================================================

	colour24 colour24::white = colour24(255, 255, 255);
	colour24 colour24::black = colour24(0, 0, 0);
	colour24 colour24::red = colour24(255, 0, 0);
	colour24 colour24::green = colour24(0, 255, 0);
	colour24 colour24::blue = colour24(0, 0, 255);
	colour24 colour24::dbgBlue = colour24(50, 50, 175);
	colour24 colour24::brown = colour24(167, 43, 43);
	colour24 colour24::gray = colour24(128, 128, 128);
	colour24 colour24::grey = colour24(128, 128, 128);
	colour24 colour24::magenta = colour24(255, 0, 255);
	colour24 colour24::orange = colour24(255, 166, 0);
	colour24 colour24::pink = colour24(255, 191, 203);
	colour24 colour24::purple = colour24(160, 32, 240);
	colour24 colour24::silver = colour24(192, 192, 192);
	colour24 colour24::snow = colour24(255, 250, 250);
	colour24 colour24::violet = colour24(238, 130, 238);
	colour24 colour24::yellow = colour24(255, 235, 4);

	colour24 colour24::clear = colour24(0, 0, 0);
	colour24 colour24::aliceBlue = colour24(240, 247, 255);

	// ============================================================================
	// colour32 Static Constants
	// ============================================================================

	colour32 colour32::white = colour32(255, 255, 255, 255);
	colour32 colour32::black = colour32(0, 0, 0, 255);
	colour32 colour32::red = colour32(255, 0, 0, 255);
	colour32 colour32::green = colour32(0, 255, 0, 255);
	colour32 colour32::blue = colour32(0, 0, 255, 255);
	colour32 colour32::dbgBlue = colour32(50, 50, 175, 255);
	colour32 colour32::brown = colour32(167, 43, 43, 255);
	colour32 colour32::gray = colour32(128, 128, 128, 255);
	colour32 colour32::grey = colour32(128, 128, 128, 255);
	colour32 colour32::magenta = colour32(255, 0, 255, 255);
	colour32 colour32::orange = colour32(255, 166, 0, 255);
	colour32 colour32::pink = colour32(255, 191, 203, 255);
	colour32 colour32::purple = colour32(160, 32, 240, 255);
	colour32 colour32::silver = colour32(192, 192, 192, 255);
	colour32 colour32::snow = colour32(255, 250, 250, 255);
	colour32 colour32::violet = colour32(238, 130, 238, 255);
	colour32 colour32::yellow = colour32(255, 235, 4, 255);

	colour32 colour32::clear = colour32(0, 0, 0, 0);
	colour32 colour32::aliceBlue = colour32(240, 247, 255, 255);
}

// ============================================================================
// Constructors
// ============================================================================

sml::colour24::colour24(u8 ir, u8 ig, u8 ib) noexcept {
	r = sml::clamp(ir, 0, 255);
	g = sml::clamp(ig, 0, 255);
	b = sml::clamp(ib, 0, 255);
}

sml::colour24::colour24(u8 rgb) {
	r = g = b = sml::clamp(rgb, 0, 255);
}

sml::colour32::colour32(u8 ir, u8 ig, u8 ib, u8 ia) noexcept {
	r = sml::clamp(ir, 0, 255);
	g = sml::clamp(ig, 0, 255);
	b = sml::clamp(ib, 0, 255);
	a = sml::clamp(ia, 0, 255);
}

sml::colour32::colour32(u8 rgb, u8 ia) noexcept {
	u8 rgbc = sml::clamp(rgb, 0, 255);
	r = g = b = rgbc;
	a = sml::clamp(ia, 0, 255);
}

sml::colour32::colour32(const colour24& col, u8 ia) noexcept {
	r = sml::clamp(col.r, 0, 255);
	g = sml::clamp(col.g, 0, 255);
	b = sml::clamp(col.b, 0, 255);
	a = sml::clamp(ia, 0, 255);
}

sml::colour96::colour96(f32 ir, f32 ig, f32 ib) noexcept {
	r = sml::clamp01(ir);
	g = sml::clamp01(ig);
	b = sml::clamp01(ib);
}

sml::colour96::colour96(f32 rgb) {
	r = g = b = sml::clamp01(rgb);
}

sml::colour128::colour128(f32 ir, f32 ig, f32 ib, f32 ia) noexcept {
	r = sml::clamp01(ir);
	g = sml::clamp01(ig);
	b = sml::clamp01(ib);
	a = sml::clamp01(ia);
}

sml::colour128::colour128(f32 rgb, f32 ia) noexcept {
	r = g = b = sml::clamp01(rgb);
	a = sml::clamp01(ia);
}

sml::colour128::colour128(const colour96& col, f32 ia) noexcept {
	r = sml::clamp01(col.r);
	g = sml::clamp01(col.g);
	b = sml::clamp01(col.b);
	a = sml::clamp01(ia);
}

// ============================================================================
// Static Method Implementations
// ============================================================================

sml::colour24 sml::colour24::Lerp(const colour24& a, const colour24& b, f32 t) noexcept {
	colour24 temp;

	temp.r = static_cast<u8>(static_cast<f32>(a.r) * (1.f - t) + (static_cast<f32>(b.r) * t));
	temp.g = static_cast<u8>(static_cast<f32>(a.g) * (1.f - t) + (static_cast<f32>(b.g) * t));
	temp.b = static_cast<u8>(static_cast<f32>(a.b) * (1.f - t) + (static_cast<f32>(b.b) * t));

	return temp;
}

sml::colour32 sml::colour32::Lerp(const colour32& a, const colour32& b, f32 t) noexcept {
	colour32 temp;

	temp.r = static_cast<u8>(static_cast<f32>(a.r) * (1.f - t) + (static_cast<f32>(b.r) * t));
	temp.g = static_cast<u8>(static_cast<f32>(a.g) * (1.f - t) + (static_cast<f32>(b.g) * t));
	temp.b = static_cast<u8>(static_cast<f32>(a.b) * (1.f - t) + (static_cast<f32>(b.b) * t));
	temp.a = static_cast<u8>(static_cast<f32>(a.a) * (1.f - t) + (static_cast<f32>(b.a) * t));

	return temp;
}

sml::colour96 sml::colour96::Lerp(const colour96& a, const colour96& b, f32 t) noexcept {
	colour96 temp;

	temp.r = a.r * (1.f - t) + (b.r * t);
	temp.g = a.g * (1.f - t) + (b.g * t);
	temp.b = a.b * (1.f - t) + (b.b * t);

	return temp;
}

sml::colour128 sml::colour128::Lerp(const colour128& a, const colour128& b, f32 t) noexcept {
	colour128 temp;

	temp.r = a.r * (1.f - t) + (b.r * t);
	temp.g = a.g * (1.f - t) + (b.g * t);
	temp.b = a.b * (1.f - t) + (b.b * t);
	temp.a = a.a * (1.f - t) + (b.a * t);

	return temp;
}

// ============================================================================
// Member Method Implementations
// ============================================================================

sml::vector3 sml::colour24::To01RangeV() const noexcept {
	return { r * k255to1, g * k255to1, b * k255to1 };
}

sml::vector4 sml::colour24::To01RangeVA() const noexcept {
	return { r * k255to1, g * k255to1, b * k255to1, 1.f };
}

sml::colour96 sml::colour24::To01Range() const noexcept {
	return { r * k255to1, g * k255to1, b * k255to1 };
}

sml::colour128 sml::colour24::To01RangeA() const noexcept {
	return { r * k255to1, g * k255to1, b * k255to1, 1.f };
}

sml::vector4 sml::colour32::To01RangeV() const noexcept {
	return { r * k255to1, g * k255to1, b * k255to1, a * k255to1 };
}

sml::colour128 sml::colour32::To01Range() const noexcept {
	return { r * k255to1, g * k255to1, b * k255to1, a * k255to1 };
}

string sml::colour24::ToString() const noexcept {
	return "{" + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + "}";
}

string sml::colour32::ToString() const noexcept {
	return "{" + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ", " + std::to_string(a) + "}";
}

string sml::colour96::ToString() const noexcept {
	return "{" + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + "}";
}

string sml::colour128::ToString() const noexcept {
	return "{" + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ", " + std::to_string(a) + "}";
}