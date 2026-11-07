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

#include "spike/sml/vector3.h"

#include <string>

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

#include "spike/sml/vector2.h"
#include "spike/sml/sml_core.h"

namespace sml {
	const vector3 vector3::up = vector3(0.0f, 1.0f, 0.0f);
	const vector3 vector3::down = vector3(0.0f, -1.0f, 0.0f);
	const vector3 vector3::left = vector3(-1.0f, 0.0f, 0.0f);
	const vector3 vector3::right = vector3(1.0f, 0.0f, 0.0f);
	const vector3 vector3::front = vector3(0.0f, 0.0f, 1.0f);
	const vector3 vector3::back = vector3(0.0f, 0.0f, -1.0f);
	const vector3 vector3::one = vector3(1.0f, 1.0f, 1.0f);
	const vector3 vector3::zero = vector3(0.0f, 0.0f, 0.0f);

	vector3 vector3::Normalized() const noexcept {
		f32 mag = this->Magnitude();
		if (mag == 0.f) return vector3();
		f32 invMag = 1.0f / mag;
		return *this * invMag;
	}

	f32 vector3::Magnitude() const noexcept {
		return sml::sqrt(SQUARE(x) + SQUARE(y) + SQUARE(z));
	}

	vector3 vector3::Min(const vector3& lhs, const vector3& rhs) noexcept {
		return {
			lhs.x < rhs.x ? lhs.x : rhs.x,
			lhs.y < rhs.y ? lhs.y : rhs.y,
			lhs.z < rhs.z ? lhs.z : rhs.z
		};
	}

	vector3 vector3::Max(const vector3& lhs, const vector3& rhs) noexcept {
		return {
			lhs.x > rhs.x ? lhs.x : rhs.x,
			lhs.y > rhs.y ? lhs.y : rhs.y,
			lhs.z > rhs.z ? lhs.z : rhs.z
		};
	}

	f32 vector3::Angle(const vector3& lhs, const vector3& rhs) noexcept {
		vector3 l = lhs.Normalized();
		vector3 r = rhs.Normalized();

		f32 d = vector3::Dot(l, r);

		return sml::acos(d);
	}

	vector3 vector3::Cross(const vector3& lhs, const vector3& rhs) noexcept {
		return {
			lhs.y * rhs.z - lhs.z * rhs.y,
			lhs.z * rhs.x - lhs.x * rhs.z,
			lhs.x * rhs.y - lhs.y * rhs.x
		};
	}

	f32 vector3::Dot(const vector3& lhs, const vector3& rhs) noexcept {
		return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
	}

	f32 vector3::Distance(const vector3& lhs, const vector3& rhs) noexcept {
		return sml::sqrt(
			SQUARE(lhs.x - rhs.x) +
			SQUARE(lhs.y - rhs.y) +
			SQUARE(lhs.z - rhs.z)
		);
	}

	vector3 vector3::Normalize(const vector3& v) noexcept {
		f32 mag = v.Magnitude();
		if (mag == 0.f) return vector3();
		f32 invMag = 1.0f / mag;

		return v * invMag;
	}

	vector3 vector3::Lerp(const vector3& lhs, const vector3& rhs, f32 t) noexcept {
		return (lhs * t) + (rhs * (1.0f - t));
	}

	vector3 vector3::Scale(const vector3& lhs, const vector3& rhs) noexcept {
		return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z };
	}

	vector3 vector3::operator+(const vector3& other) const {
		return { x + other.x, y + other.y, z + other.z };
	}

	vector3 vector3::operator*(const f32 scalar) const {
		return { x * scalar, y * scalar, z * scalar };
	}

	vector3 vector3::operator*(const vector3& other) const {
		return { x * other.x, y * other.y, z * other.z };
	}

	vector3 vector3::operator-(const vector3& other) const {
		return { x - other.x, y - other.y, z - other.z };
	}

	vector3 vector3::operator-() const {
		return { -x, -y, -z };
	}

	vector3 vector3::operator/(const f32 scalar) const {
		if (scalar == 0) return vector3();
		const f32 d = 1.f / scalar;
		return { x * d, y * d, z * d };
	}

	vector3 vector3::operator/(const vector3& other) const {
		return { x / other.x, y / other.y, z / other.z };
	}

	vector3& vector3::operator+=(const vector3& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	vector3& vector3::operator*=(const vector3& other) {
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}

	vector3& vector3::operator*=(const f32 scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	vector3& vector3::operator-=(const vector3& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	vector3& vector3::operator/=(const vector3& other) {
		x /= other.x;
		y /= other.y;
		z /= other.z;
		return *this;
	}

	vector3& vector3::operator/=(const f32 scalar) {
		if (scalar == 0) {
			x = y = z = 0;
		}
		else {
			const f32 d = 1.f / scalar;
			x *= d;
			y *= d;
			z *= d;
		}
		return *this;
	}

	bool vector3::operator>(const vector3& other) const {
		return Magnitude() > other.Magnitude();
	}

	bool vector3::operator<(const vector3& other) const {
		return Magnitude() < other.Magnitude();
	}

	bool vector3::operator>=(const vector3& other) const {
		return Magnitude() >= other.Magnitude();
	}

	bool vector3::operator<=(const vector3& other) const {
		return Magnitude() <= other.Magnitude();
	}

	bool vector3::operator==(const vector3& other) const {
		return x == other.x && y == other.y && z == other.z;
	}

	bool vector3::operator!=(const vector3& other) const {
		return x != other.x || y != other.y || z != other.z;
	}

	vector2 vector3::XZ() const noexcept {
		return { x, z };
	}

	vector2 vector3::XY() const noexcept {
		return { x, y };
	}

	vector2 vector3::YZ() const noexcept {
		return { y, z };
	}

	string vector3::ToString() const noexcept {
		return "{" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + "}";
	}
}