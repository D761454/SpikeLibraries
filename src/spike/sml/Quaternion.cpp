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

#include "spike/sml/Quaternion.h"

#include <string>

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

#include "spike/sml/Vector3.h"
#include "spike/sml/Matrix4.h"
#include "spike/sml/SMLCore.h"

// uses ZYX Formula

namespace sml {
	quaternion quaternion::identity = quaternion(0.0f, 0.0f, 0.0f, 1.0f);

	quaternion::quaternion(const vector3& euler, const bool degrees) noexcept {
		f32 rx = euler.x, ry = euler.y, rz = euler.z;

		if (degrees) {
			rx = (f32)(rx * sml::Deg2Rad);
			ry = (f32)(ry * sml::Deg2Rad);
			rz = (f32)(rz * sml::Deg2Rad);
		}

		f32 c1 = sml::cos(rz / 2.0f);
		f32 c2 = sml::cos(ry / 2.0f);
		f32 c3 = sml::cos(rx / 2.0f);

		f32 s1 = sml::sin(rz / 2.0f);
		f32 s2 = sml::sin(ry / 2.0f);
		f32 s3 = sml::sin(rx / 2.0f);

		x = (c1 * c2 * s3) - (s1 * s2 * c3);
		y = (c1 * s2 * c3) + (s1 * c2 * s3);
		z = (s1 * c2 * c3) - (c1 * s2 * s3);
		w = (c1 * c2 * c3) + (s1 * s2 * s3);
	}

	quaternion::quaternion(f32 angle, const vector3& axis) noexcept {
		f32 rad = angle * (f32)sml::Deg2Rad;

		vector3 n = axis;
		if (n.Magnitude() == 0.0f) {
			x = y = z = 0.0f; w = 1.0f;
			return;
		}
		n = n.Normalized();
		f32 s = sml::sin(rad * 0.5f);

		x = n.x * s;
		y = n.y * s;
		z = n.z * s;
		w = sml::cos(rad * 0.5f);
	}

	const f32 quaternion::Length() const noexcept {
		return sml::sqrt(SQUARE(x) + SQUARE(y) + SQUARE(z) + SQUARE(w));
	}

	quaternion quaternion::Normalized() const noexcept {
		f32 d = Length();
		if (d == 0.0f) return quaternion::identity;
		f32 invMag = 1.f / d;
		return { x * invMag, y * invMag, z * invMag, w * invMag };
	}

	vector3 quaternion::EulerAngles(const bool degrees) const noexcept {
		vector3 v;
		f32 x = this->x, y = this->y, z = this->z, w = this->w;

		f32 r11 = 2 * (x * y + w * z);
		f32 r12 = w * w + x * x - y * y - z * z;
		f32 r21 = -2 * (x * z - w * y);
		f32 r31 = 2 * (y * z + w * x);
		f32 r32 = w * w - x * x - y * y + z * z;

		v.x = sml::atan2(r31, r32);
		v.y = sml::asin(sml::clamp(r21, -1.0f, 1.0f));
		v.z = sml::atan2(r11, r12);

		if (degrees) {
			v.x = (f32)(v.x * sml::Rad2Deg);
			v.y = (f32)(v.y * sml::Rad2Deg);
			v.z = (f32)(v.z * sml::Rad2Deg);
		}

		return v;
	}

	static f32 nornmalize_angle(f32 angle) {
		while (angle > 180.f)  angle -= 360.f;
		while (angle < -180.f) angle += 360.f;
		return angle;
	}

	vector3 quaternion::EulerAnglesAlt(const bool degrees, const vector3* primaryEuler) const noexcept {
		vector3 alt;
		if (primaryEuler) {
			alt.y = 180.f - primaryEuler->y;

			alt.x = primaryEuler->x + 180.f;
			alt.z = primaryEuler->z + 180.f;

			alt.x = nornmalize_angle(alt.x);
			alt.z = nornmalize_angle(alt.z);
		}
		else {
			const vector3 primary = this->EulerAngles(degrees);

			alt.y = 180.f - primary.y;

			alt.x = primary.x + 180.f;
			alt.z = primary.z + 180.f;

			alt.x = nornmalize_angle(alt.x);
			alt.z = nornmalize_angle(alt.z);
		}
		return alt;
	}

	void quaternion::SetLookRotation(const vector3& view, const vector3& up) noexcept {
		this->Set(LookRotation(view, up));
	}

	void quaternion::ToAngleAxis(float& angle, vector3& axis) const noexcept {
		quaternion q = this->Normalized();
		f32 w = q[3];
		f32 s = sml::sqrt(1.0f - w * w);
		if (s < 1e-6f) {
			angle = 0.0f;
			axis = vector3::front;
		}
		else {
			angle = 2.0f * sml::acos(w);
			axis = { q[0] / s, q[1] / s, q[2] / s };
		}
		angle = angle * (f32)sml::Rad2Deg;
	}

	matrix4 quaternion::ToMatrix4() const noexcept {
		return matrix4::Rotate(*this);
	}

	f32 quaternion::Angle(const quaternion& lhs, const quaternion& rhs) noexcept {
		return (f32)(sml::acos(sml::clamp(Dot(lhs.Normalized(), rhs.Normalized()), -1.0f, 1.0f)) * sml::Rad2Deg);
	}

	f32 quaternion::Dot(const quaternion& a, const quaternion& b) noexcept {
		return ((a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]) + (a[3] * b[3]));
	}

	quaternion quaternion::Conjugate(const quaternion& quat) noexcept {
		return { -quat[0], -quat[1], -quat[2], quat[3] };
	}

	quaternion quaternion::Negate(const quaternion& quat) noexcept {
		return { -quat[0], -quat[1], -quat[2], -quat[3] };
	}

	quaternion quaternion::Inverse(const quaternion& rotation) noexcept {
		return Conjugate(rotation.Normalized());
	}

	quaternion quaternion::Euler(f32 ix, f32 iy, f32 iz, const bool degrees) noexcept {
		// ZYX format
		f32 rx = ix, ry = iy, rz = iz;

		if (degrees) {
			rx = (f32)(rx * sml::Deg2Rad);
			ry = (f32)(ry * sml::Deg2Rad);
			rz = (f32)(rz * sml::Deg2Rad);
		}

		f32 c1 = sml::cos(rz / 2.0f);
		f32 c2 = sml::cos(ry / 2.0f);
		f32 c3 = sml::cos(rx / 2.0f);

		f32 s1 = sml::sin(rz / 2.0f);
		f32 s2 = sml::sin(ry / 2.0f);
		f32 s3 = sml::sin(rx / 2.0f);

		return {
			(c1 * c2 * s3) - (s1 * s2 * c3),
			(c1 * s2 * c3) + (s1 * c2 * s3),
			(s1 * c2 * c3) - (c1 * s2 * s3),
			(c1 * c2 * c3) + (s1 * s2 * s3)
		};
	}

	quaternion quaternion::Euler(const vector3& euler, const bool degrees) noexcept {
		return Euler(euler.x, euler.y, euler.z, degrees);
	}

	quaternion quaternion::Lerp(const quaternion& a, const quaternion& b, f32 t) noexcept {
		const f32 dot = Dot(a, b);
		quaternion b2 = b;
		if (dot < 0.0f) b2 = Negate(b);
		return Normalize(
			{
				a[0] - t * (a[0] - b2[0]),
				a[1] - t * (a[1] - b2[1]),
				a[2] - t * (a[2] - b2[2]),
				a[3] - t * (a[3] - b2[3])
			}
		);
	}

	quaternion quaternion::Slerp(const quaternion& a, const quaternion& b, f32 t) noexcept {
		if (t <= 0.0f) return a;
		if (t >= 1.0f) return b;

		f32 cosTheta = Dot(a, b);
		quaternion end = b;

		// if dot < 0, use shortest path
		if (cosTheta < 0.0f) {
			cosTheta = -cosTheta;
			end = Negate(b);
		}

		// If quaternions are close, use linear interpolation
		if (cosTheta > 0.9995f) return Lerp(a, end, t);

		f32 theta = sml::acos(cosTheta);
		f32 sinTheta = sml::sin(theta);
		f32 w1 = sml::sin((1.0f - t) * theta) / sinTheta;
		f32 w2 = sml::sin(t * theta) / sinTheta;

		quaternion ret(
			a[0] * w1 + end[0] * w2,
			a[1] * w1 + end[1] * w2,
			a[2] * w1 + end[2] * w2,
			a[3] * w1 + end[3] * w2
		);

		return ret.Normalized();
	}

	quaternion quaternion::LookRotation(const vector3& forward, const vector3& up) noexcept {
		// Build orthonormal basis
		vector3 f = forward.Normalized();
		vector3 r = vector3::Normalize(vector3::Cross(up, f));
		vector3 u = vector3::Cross(f, r);

		// Rotation matrix with columns [r u f]
		f32 m00 = r.x, m01 = u.x, m02 = f.x;
		f32 m10 = r.y, m11 = u.y, m12 = f.y;
		f32 m20 = r.z, m21 = u.z, m22 = f.z;

		f32 trace = m00 + m11 + m22;
		f32 x, y, z, w;
		if (trace > 0.0f) {
			f32 s = sml::sqrt(trace + 1.0f) * 2.0f; // s = 4*w
			w = 0.25f * s;
			x = (m21 - m12) / s;
			y = (m02 - m20) / s;
			z = (m10 - m01) / s;
		}
		else if ((m00 > m11) && (m00 > m22)) {
			f32 s = sml::sqrt(1.0f + m00 - m11 - m22) * 2.0f;
			w = (m21 - m12) / s;
			x = 0.25f * s;
			y = (m01 + m10) / s;
			z = (m02 + m20) / s;
		}
		else if (m11 > m22) {
			f32 s = sml::sqrt(1.0f + m11 - m00 - m22) * 2.0f;
			w = (m02 - m20) / s;
			x = (m01 + m10) / s;
			y = 0.25f * s;
			z = (m12 + m21) / s;
		}
		else {
			f32 s = sml::sqrt(1.0f + m22 - m00 - m11) * 2.0f;
			w = (m10 - m01) / s;
			x = (m02 + m20) / s;
			y = (m12 + m21) / s;
			z = 0.25f * s;
		}
		return quaternion(x, y, z, w).Normalized();
	}

	quaternion quaternion::Normalize(const quaternion& v) noexcept {
		f32 d = v.Length();
		if (d == 0.0f) return quaternion::identity;
		return { v[0] / d, v[1] / d, v[2] / d, v[3] / d };
	}

	quaternion quaternion::FromToRotation(const vector3& from, const vector3& to) noexcept {
		vector3 f = from.Normalized();
		vector3 t = to.Normalized();

		f32 cosTheta = vector3::Dot(f, t);
		if (cosTheta > 0.999999f) return quaternion::identity;

		if (cosTheta < -0.999999f) {
			// find orthogonal axis
			vector3 axis = vector3::Cross(vector3::right, f);
			if (axis.Magnitude() < 1e-6f) axis = vector3::Cross(vector3::up, f);
			axis = axis.Normalized();
			return quaternion(180.0f, axis);
		}

		vector3 axis = vector3::Cross(f, t);
		quaternion q(axis.x, axis.y, axis.z, 1.0f + cosTheta);
		return q.Normalized();
	}

	void quaternion::Interpolate(quaternion& out, const quaternion& a, const quaternion& b, f32 d) noexcept {
		f32 cosom = Dot(a, b);

		quaternion end = b;
		if (cosom < static_cast<f32>(0.0)) {
			cosom = -cosom;
			end = Negate(end);
		}

		f32 sclp, sclq;

		if ((static_cast<f32>(1.0) - cosom) > kEpsilon) {
			f32 omega, sinom;
			omega = sml::acos(cosom);
			sinom = sml::sin(omega);
			sclp = sml::sin((static_cast<f32>(1.0) - d) * omega) / sinom;
			sclq = sml::sin(d * omega) / sinom;
		}
		else {
			sclp = static_cast<f32>(1.0) - d;
			sclq = d;
		}

		out[0] = sclp * a[0] + sclq * b[0];
		out[1] = sclp * a[1] + sclq * b[1];
		out[2] = sclp * a[2] + sclq * b[2];
		out[3] = sclp * a[3] + sclq * b[3];
	}

	vector3 quaternion::operator*(const vector3& v) const {
		// Efficient quaternion-vector multiplication (q * v * q^-1)
		// t = 2 * cross(q.xyz, v)
		// v' = v + q.w * t + cross(q.xyz, t)
		vector3 qv = { x, y, z };
		vector3 t = vector3::Cross(qv, v) * 2.0f;
		vector3 result = v + (t * w) + vector3::Cross(qv, t);
		return result;
	}

	quaternion quaternion::operator*(const quaternion& other) const {
		f32 x1 = x, y1 = y, z1 = z, w1 = w;
		f32 x2 = other.x, y2 = other.y, z2 = other.z, w2 = other.w;

		return {
			w1 * x2 + x1 * w2 + y1 * z2 - z1 * y2,
			w1 * y2 - x1 * z2 + y1 * w2 + z1 * x2,
			w1 * z2 + x1 * y2 - y1 * x2 + z1 * w2,
			w1 * w2 - x1 * x2 - y1 * y2 - z1 * z2
		};
	}

	bool quaternion::operator==(const quaternion& other) const {
		return x == other.x && y == other.y && z == other.z && w == other.w;
	}

	bool quaternion::operator!=(const quaternion& other) const {
		return x != other.x || y != other.y || z != other.z || w != other.w;
	}

	string quaternion::ToString() const noexcept {
		return "{" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w) + "}";
	}
}