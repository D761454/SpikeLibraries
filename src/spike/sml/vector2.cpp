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

#include "spike/sml/vector2.h"

#include <string>

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

#include "spike/sml/vector3.h"
#include "spike/sml/sml_core.h"

namespace sml {
	const vector2 vector2::up = vector2(0.0f, 1.0f);
	const vector2 vector2::down = vector2(0.0f, -1.0f);
	const vector2 vector2::left = vector2(-1.0f, 0.0f);
	const vector2 vector2::right = vector2(1.0f, 0.0f);
	const vector2 vector2::one = vector2(1.0f, 1.0f);
	const vector2 vector2::zero = vector2(0.0f, 0.0f);

	f32 vector2::Magnitude() const noexcept {
		return sml::sqrt(SQUARE(x) + SQUARE(y));
	}

	vector2 vector2::Normalized() const noexcept {
		f32 mag = this->Magnitude();
		if (mag == 0.f) return vector2();
		f32 invMag = 1.f / mag;
		return *this * invMag;
	}

	vector2	vector2::Min(const vector2& lhs, const vector2& rhs) noexcept {
		return {
			lhs.x < rhs.x ? lhs.x : rhs.x,
			lhs.y < rhs.y ? lhs.y : rhs.y
		};
	}

	vector2	vector2::Max(const vector2& lhs, const vector2& rhs) noexcept {
		return {
			lhs.x > rhs.x ? lhs.x : rhs.x,
			lhs.y > rhs.y ? lhs.y : rhs.y
		};
	}

	f32	vector2::Dot(const vector2& lhs, const vector2& rhs) noexcept {
		return (lhs.x * rhs.x) + (lhs.y * rhs.y);
	}

	f32	vector2::Angle(const vector2& lhs, const vector2& rhs) noexcept {
		vector2 l = lhs.Normalized(); vector2 r = rhs.Normalized();

		f32 d = vector2::Dot(l, r);

		return sml::acos(d);
	}

	f32	vector2::Cross(const vector2& lhs, const vector2& rhs) noexcept {
		return (lhs.x * rhs.y) - (lhs.y * rhs.x);
	}

	f32	vector2::Distance(const vector2& lhs, const vector2& rhs) noexcept {
		return sml::sqrt(
			SQUARE(lhs.x - rhs.x) +
			SQUARE(lhs.y - rhs.y)
		);
	}

	vector2	vector2::Normalize(const vector2& v) noexcept {
		f32 mag = v.Magnitude();
		if (mag == 0.f) return vector2();
		f32 invMag = 1.f / mag;

		return v * invMag;
	}

	vector2	vector2::Lerp(const vector2& lhs, const vector2& rhs, f32 t) noexcept {
		return (lhs * t) + (rhs * (1.f - t));
	}

	vector2	vector2::Scale(const vector2& lhs, const vector2& rhs) noexcept {
		return { lhs.x * rhs.x, lhs.y * rhs.y };
	}

	vector2 vector2::operator+(const vector2& other) const {
		return { x + other.x, y + other.y };
	}

	vector2 vector2::operator*(const f32 scalar) const {
		return { x * scalar, y * scalar };
	}

	vector2 vector2::operator*(const vector2& other) const {
		return { x * other.x, y * other.y };
	}

	vector2 vector2::operator-(const vector2& other) const {
		return { x - other.x, y - other.y };
	}

	vector2 vector2::operator-() const {
		return { -x, -y };
	}

	vector2 vector2::operator/(const f32 scalar) const {
		if (scalar == 0) return vector2();
		const f32 d = 1.f / scalar;
		return { x * d, y * d };
	}

	vector2 vector2::operator/(const vector2& other) const {
		return { x / other.x, y / other.y };
	}

	vector2& vector2::operator+=(const vector2& other) {
		x += other.x;
		y += other.y;
		return *this;
	}

	vector2& vector2::operator*=(const vector2& other) {
		x *= other.x;
		y *= other.y;
		return *this;
	}

	vector2& vector2::operator*=(const f32 scalar) {
		x *= scalar;
		y *= scalar;
		return *this;
	}

	vector2& vector2::operator-=(const vector2& other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}

	vector2& vector2::operator/=(const vector2& other) {
		x /= other.x;
		y /= other.y;
		return *this;
	}

	vector2& vector2::operator/=(const f32 scalar) {
		if (scalar == 0) {
			x = y = 0;
		}
		else {
			const f32 d = 1.f / scalar;
			x *= d;
			y *= d;
		}
		return *this;
	}

	bool vector2::operator>(const vector2& other) const {
		return Magnitude() > other.Magnitude();
	}

	bool vector2::operator<(const vector2& other) const {
		return Magnitude() < other.Magnitude();
	}

	bool vector2::operator>=(const vector2& other) const {
		return Magnitude() >= other.Magnitude();
	}

	bool vector2::operator<=(const vector2& other) const {
		return Magnitude() <= other.Magnitude();
	}

	bool vector2::operator==(const vector2& other) const {
		return x == other.x && y == other.y;
	}

	bool vector2::operator!=(const vector2& other) const {
		return x != other.x || y != other.y;
	}

	vector3 vector2::ToVector3XZ() const noexcept {
		return { x, 0.0f, y };
	}

	vector3 vector2::ToVector3XY() const noexcept {
		return { x, y, 0.0f };
	}

	string vector2::ToString() const noexcept {
		return "{" + std::to_string(x) + ", " + std::to_string(y) + "}";
	}
}