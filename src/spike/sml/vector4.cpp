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

#include "spike/sml/vector4.h"

#include <string>

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

#include "spike/sml/vector3.h"
#include "spike/sml/sml_core.h"

namespace sml {
	const vector4 vector4::up = vector4(0.0f, 1.0f, 0.0f, 0.f);
	const vector4 vector4::down = vector4(0.0f, -1.0f, 0.0f, 0.f);
	const vector4 vector4::left = vector4(-1.0f, 0.0f, 0.0f, 0.f);
	const vector4 vector4::right = vector4(1.0f, 0.0f, 0.0f, 0.f);
	const vector4 vector4::front = vector4(0.0f, 0.0f, 1.0f, 0.f);
	const vector4 vector4::back = vector4(0.0f, 0.0f, -1.0f, 0.f);
	const vector4 vector4::one = vector4(1.0f, 1.0f, 1.0f, 0.f);
	const vector4 vector4::zero = vector4(0.0f, 0.0f, 0.0f, 0.f);

	f32 vector4::Magnitude() const noexcept {
		return sml::sqrt(SQUARE(x) + SQUARE(y) + SQUARE(z) + SQUARE(w));
	}

	vector4 vector4::Normalized() const noexcept {
		f32 mag = this->Magnitude();
		if (mag == 0.f) return vector4();
		f32 invMag = 1.f / mag;
		return *this * invMag;
	}

	vector4 vector4::Min(const vector4& lhs, const vector4& rhs) noexcept {
		return {
			lhs.x < rhs.x ? lhs.x : rhs.x,
			lhs.y < rhs.y ? lhs.y : rhs.y,
			lhs.z < rhs.z ? lhs.z : rhs.z,
			lhs.w < rhs.w ? lhs.w : rhs.w
		};
	}

	vector4 vector4::Max(const vector4& lhs, const vector4& rhs) noexcept {
		return {
			lhs.x > rhs.x ? lhs.x : rhs.x,
			lhs.y > rhs.y ? lhs.y : rhs.y,
			lhs.z > rhs.z ? lhs.z : rhs.z,
			lhs.w < rhs.w ? lhs.w : rhs.w
		};
	}

	f32 vector4::Dot(const vector4& lhs, const vector4& rhs) noexcept {
		return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z) + (lhs.w * rhs.w);
	}

	f32 vector4::Angle(const vector4& lhs, const vector4& rhs) noexcept {
		vector4 l = lhs.Normalized();
		vector4 r = rhs.Normalized();

		f32 d = vector4::Dot(l, r);

		return sml::acos(d);
	}

	vector4 vector4::Cross(const vector4& lhs, const vector4& rhs) noexcept {
		return {
			lhs.y * rhs.z - lhs.z * rhs.y,
			lhs.z * rhs.x - lhs.x * rhs.z,
			lhs.x * rhs.y - lhs.y * rhs.x,
			1.f
		};
	}

	f32 vector4::Distance(const vector4& lhs, const vector4& rhs) noexcept {
		return sml::sqrt(
			SQUARE(lhs.x - rhs.x) +
			SQUARE(lhs.y - rhs.y) +
			SQUARE(lhs.z - rhs.z) +
			SQUARE(lhs.w - rhs.w)
		);
	}

	vector4 vector4::Normalize(const vector4& v) noexcept {
		f32 mag = v.Magnitude();
		if (mag == 0.f) return vector4();
		f32 invMag = 1.f / mag;

		return v * invMag;
	}

	vector4 vector4::Lerp(const vector4& lhs, const vector4& rhs, f32 t) noexcept {
		return (lhs * t) + (rhs * (1.f - t));
	}

	vector4 vector4::Scale(const vector4& lhs, const vector4& rhs) noexcept {
		return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w };
	}

	vector4 vector4::operator+(const vector4& other) const {
		return { x + other.x, y + other.y, z + other.z, w + other.w };
	}

	vector4 vector4::operator*(const f32 scalar) const {
		return { x * scalar, y * scalar, z * scalar, w * scalar };
	}

	vector4 vector4::operator*(const vector4& other) const {
		return { x * other.x, y * other.y, z * other.z, w * other.w };
	}

	vector4 vector4::operator-(const vector4& other) const {
		return { x - other.x, y - other.y, z - other.z, w - other.w };
	}

	vector4 vector4::operator-() const {
		return { -x, -y, -z, -w };
	}

	vector4 vector4::operator/(const f32 scalar) const {
		if (scalar == 0) return vector4();
		const f32 d = 1.f / scalar;
		return { x * d, y * d, z * d, w * d };
	}

	vector4 vector4::operator/(const vector4& other) const {
		return { x / other.x, y / other.y, z / other.z, w / other.w };
	}

	vector4& vector4::operator+=(const vector4& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;
		return *this;
	}

	vector4& vector4::operator*=(const vector4& other) {
		x *= other.x;
		y *= other.y;
		z *= other.z;
		w *= other.w;
		return *this;
	}

	vector4& vector4::operator*=(const f32 scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		w *= scalar;
		return *this;
	}

	vector4& vector4::operator-=(const vector4& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		w -= other.w;
		return *this;
	}

	vector4& vector4::operator/=(const vector4& other) {
		x /= other.x;
		y /= other.y;
		z /= other.z;
		w /= other.w;
		return *this;
	}

	vector4& vector4::operator/=(const f32 scalar) {
		if (scalar == 0) {
			x = y = z = w = 0;
		}
		else {
			const f32 d = 1.f / scalar;
			x *= d;
			y *= d;
			z *= d;
			w *= d;
		}
		return *this;
	}

	bool vector4::operator>(const vector4& other) const {
		return Magnitude() > other.Magnitude();
	}

	bool vector4::operator<(const vector4& other) const {
		return Magnitude() < other.Magnitude();
	}

	bool vector4::operator>=(const vector4& other) const {
		return Magnitude() >= other.Magnitude();
	}

	bool vector4::operator<=(const vector4& other) const {
		return Magnitude() <= other.Magnitude();
	}

	bool vector4::operator==(const vector4& other) const {
		return x == other.x && y == other.y && z == other.z && w == other.w;
	}

	bool vector4::operator!=(const vector4& other) const {
		return x != other.x || y != other.y || z != other.z || w != other.w;
	}

	vector3 vector4::ToVector3() const noexcept {
		return { x, y, z };
	}

	string vector4::ToString() const noexcept {
		return "{" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w) + "}";
	}
}