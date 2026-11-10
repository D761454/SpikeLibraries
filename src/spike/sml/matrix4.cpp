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

#include "spike/sml/matrix4.h"

#include <cassert>
#include <string>

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

#include "spike/sml/vector3.h"
#include "spike/sml/vector4.h"
#include "spike/sml/quaternion.h"
#include "spike/sml/frustumplane.h"
#include "spike/sml/sml_core.h"

namespace sml {
	matrix4 matrix4::identity = matrix4({
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	});

	matrix4 matrix4::zero = matrix4();

	sml::vector4 matrix4::GetColumn(const u32 index) const {
		assert(index < 4 && "Index Out of Range! Column in range 0-3.");
		const u32 base = index * 4;

		return { _data[base], _data[base+1], _data[base+2], _data[base+3] };
	}

	sml::vector4 matrix4::GetRow(const u32 index) const {
		assert(index < 4 && "Index Out of Range! Column in range 0-3.");

		return { _data[index], _data[index + 4], _data[index + 8], _data[index + 12] };
	}

	void matrix4::SetColumn(const u32 index, const sml::vector4& column) {
		assert(index < 4 && "Index Out of Range! Column in range 0-3.");
		const u32 base = index * 4;
		_data[base] = column.x;
		_data[base + 1] = column.y;
		_data[base + 2] = column.z;
		_data[base + 3] = column.w;
	}

	void matrix4::SetRow(const u32 index, const sml::vector4& row) {
		assert(index < 4 && "Index Out of Range! Column in range 0-3.");

		_data[index] = row.x;
		_data[index + 4] = row.y;
		_data[index + 8] = row.z;
		_data[index + 12] = row.w;
	}

	vector3	matrix4::GetPosition() const noexcept {
		return { _data[12], _data[13], _data[14] };
	}

	f32 matrix4::Determinant() const noexcept {
		// Determinant and Inverse use the standard 4x4 cofactor / adjugate method (robust double-precision internal)
		// Use the same intermediate cofactors as used in Inverse
		f64 m[16];
		for (u32 r = 0; r < 4; ++r) {
			for (u32 c = 0; c < 4; ++c) {
				m[r * 4 + c] = _data[c * 4 + r];
			}
		}

		f64 inv[16];
		inv[0] = m[5] * m[10] * m[15] -
			m[5] * m[11] * m[14] -
			m[9] * m[6] * m[15] +
			m[9] * m[7] * m[14] +
			m[13] * m[6] * m[11] -
			m[13] * m[7] * m[10];

		inv[4] = -m[4] * m[10] * m[15] +
			m[4] * m[11] * m[14] +
			m[8] * m[6] * m[15] -
			m[8] * m[7] * m[14] -
			m[12] * m[6] * m[11] +
			m[12] * m[7] * m[10];

		inv[8] = m[4] * m[9] * m[15] -
			m[4] * m[11] * m[13] -
			m[8] * m[5] * m[15] +
			m[8] * m[7] * m[13] +
			m[12] * m[5] * m[11] -
			m[12] * m[7] * m[9];

		inv[12] = -m[4] * m[9] * m[14] +
			m[4] * m[10] * m[13] +
			m[8] * m[5] * m[14] -
			m[8] * m[6] * m[13] -
			m[12] * m[5] * m[10] +
			m[12] * m[6] * m[9];

		f64 det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
		return (f32)det;
	}

	matrix4 matrix4::Inverse() const noexcept {
		// Build row-major from column-major storage
		f64 m[16];
		for (u32 r = 0; r < 4; ++r) {
			for (u32 c = 0; c < 4; ++c) {
				m[r * 4 + c] = _data[c * 4 + r];
			}
		}

		f64 inv[16];

		inv[0] = m[5] * m[10] * m[15] -
			m[5] * m[11] * m[14] -
			m[9] * m[6] * m[15] +
			m[9] * m[7] * m[14] +
			m[13] * m[6] * m[11] -
			m[13] * m[7] * m[10];

		inv[4] = -m[4] * m[10] * m[15] +
			m[4] * m[11] * m[14] +
			m[8] * m[6] * m[15] -
			m[8] * m[7] * m[14] -
			m[12] * m[6] * m[11] +
			m[12] * m[7] * m[10];

		inv[8] = m[4] * m[9] * m[15] -
			m[4] * m[11] * m[13] -
			m[8] * m[5] * m[15] +
			m[8] * m[7] * m[13] +
			m[12] * m[5] * m[11] -
			m[12] * m[7] * m[9];

		inv[12] = -m[4] * m[9] * m[14] +
			m[4] * m[10] * m[13] +
			m[8] * m[5] * m[14] -
			m[8] * m[6] * m[13] -
			m[12] * m[5] * m[10] +
			m[12] * m[6] * m[9];

		inv[1] = -m[1] * m[10] * m[15] +
			m[1] * m[11] * m[14] +
			m[9] * m[2] * m[15] -
			m[9] * m[3] * m[14] -
			m[13] * m[2] * m[11] +
			m[13] * m[3] * m[10];

		inv[5] = m[0] * m[10] * m[15] -
			m[0] * m[11] * m[14] -
			m[8] * m[2] * m[15] +
			m[8] * m[3] * m[14] +
			m[12] * m[2] * m[11] -
			m[12] * m[3] * m[10];

		inv[9] = -m[0] * m[9] * m[15] +
			m[0] * m[11] * m[13] +
			m[8] * m[1] * m[15] -
			m[8] * m[3] * m[13] -
			m[12] * m[1] * m[11] +
			m[12] * m[3] * m[9];

		inv[13] = m[0] * m[9] * m[14] -
			m[0] * m[10] * m[13] -
			m[8] * m[1] * m[14] +
			m[8] * m[2] * m[13] +
			m[12] * m[1] * m[10] -
			m[12] * m[2] * m[9];

		inv[2] = m[1] * m[6] * m[15] -
			m[1] * m[7] * m[14] -
			m[5] * m[2] * m[15] +
			m[5] * m[3] * m[14] +
			m[13] * m[2] * m[7] -
			m[13] * m[3] * m[6];

		inv[6] = -m[0] * m[6] * m[15] +
			m[0] * m[7] * m[14] +
			m[4] * m[2] * m[15] -
			m[4] * m[3] * m[14] -
			m[12] * m[2] * m[7] +
			m[12] * m[3] * m[6];

		inv[10] = m[0] * m[5] * m[15] -
			m[0] * m[7] * m[13] -
			m[4] * m[1] * m[15] +
			m[4] * m[3] * m[13] +
			m[12] * m[1] * m[7] -
			m[12] * m[3] * m[5];

		inv[14] = -m[0] * m[5] * m[14] +
			m[0] * m[6] * m[13] +
			m[4] * m[1] * m[14] -
			m[4] * m[2] * m[13] -
			m[12] * m[1] * m[6] +
			m[12] * m[2] * m[5];

		inv[3] = -m[1] * m[6] * m[11] +
			m[1] * m[7] * m[10] +
			m[5] * m[2] * m[11] -
			m[5] * m[3] * m[10] -
			m[9] * m[2] * m[7] +
			m[9] * m[3] * m[6];

		inv[7] = m[0] * m[6] * m[11] -
			m[0] * m[7] * m[10] -
			m[4] * m[2] * m[11] +
			m[4] * m[3] * m[10] +
			m[8] * m[2] * m[7] -
			m[8] * m[3] * m[6];

		inv[11] = -m[0] * m[5] * m[11] +
			m[0] * m[7] * m[9] +
			m[4] * m[1] * m[11] -
			m[4] * m[3] * m[9] -
			m[8] * m[1] * m[7] +
			m[8] * m[3] * m[5];

		inv[15] = m[0] * m[5] * m[10] -
			m[0] * m[6] * m[9] -
			m[4] * m[1] * m[10] +
			m[4] * m[2] * m[9] +
			m[8] * m[1] * m[6] -
			m[8] * m[2] * m[5];

		f64 det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

		if (sml::abs(det) < 1e-8) {
			return matrix4::zero;
		}

		f64 invDet = 1.0 / det;
		matrix4 out;
		// inv currently holds the adjugate in row-major order, convert back into column-major storage.
		for (i32 r = 0; r < 4; ++r) {
			for (i32 c = 0; c < 4; ++c) {
				out[c * 4 + r] = (f32)(inv[r * 4 + c] * invDet);
			}
		}
		return out;
	}

	bool matrix4::IsIdentity() const noexcept {
		return *this == matrix4::identity;
	}

	quaternion matrix4::Rotation() const noexcept {
		// Extract quaternion from the upper-left 3x3 rotation
		f32 m00 = _data[0], m01 = _data[4], m02 = _data[8];
		f32 m10 = _data[1], m11 = _data[5], m12 = _data[9];
		f32 m20 = _data[2], m21 = _data[6], m22 = _data[10];

		f32 trace = m00 + m11 + m22;
		f32 x, y, z, w;

		if (trace > 0.0f) {
			f32 s = (f32)sml::sqrt(trace + 1.f) * 2.f; // s = 4*w
			w = 0.25f * s;
			x = (m21 - m12) / s;
			y = (m02 - m20) / s;
			z = (m10 - m01) / s;
		}
		else if ((m00 > m11) && (m00 > m22)) {
			f32 s = (f32)sml::sqrt(1.0f + m00 - m11 - m22) * 2.f; // s = 4*x
			w = (m21 - m12) / s;
			x = 0.25f * s;
			y = (m01 + m10) / s;
			z = (m02 + m20) / s;
		}
		else if (m11 > m22) {
			f32 s = (f32)sml::sqrt(1.0f + m11 - m00 - m22) * 2.f; // s = 4*y
			w = (m02 - m20) / s;
			x = (m01 + m10) / s;
			y = 0.25f * s;
			z = (m12 + m21) / s;
		}
		else {
			f32 s = (f32)sml::sqrt(1.0f + m22 - m00 - m11) * 2.f; // s = 4*z
			w = (m10 - m01) / s;
			x = (m02 + m20) / s;
			y = (m12 + m21) / s;
			z = 0.25f * s;
		}

		return quaternion(x, y, z, w).Normalized();
	}

	vector3 matrix4::Scale() const noexcept {
		return {
			sml::sqrt(_data[0] * _data[0] + _data[1] * _data[1] + _data[2] * _data[2]),
			sml::sqrt(_data[4] * _data[4] + _data[5] * _data[5] + _data[6] * _data[6]),
			sml::sqrt(_data[8] * _data[8] + _data[9] * _data[9] + _data[10] * _data[10])
		};
	}

	matrix4 matrix4::Transpose() const noexcept {
		matrix4 out;
		for (u32 r = 0; r < 4; ++r) {
			for (u32 c = 0; c < 4; ++c) {
				out[c * 4 + r] = _data[r * 4 + c];
			}
		}
		return out;
	}

	vector3 matrix4::MultiplyPoint(const vector3& point) const noexcept {
		f32 x = (_data[0] * point.x) + (_data[4] * point.y) + (_data[8] * point.z) + (_data[12] * 1.0f);
		f32 y = (_data[1] * point.x) + (_data[5] * point.y) + (_data[9] * point.z) + (_data[13] * 1.0f);
		f32 z = (_data[2] * point.x) + (_data[6] * point.y) + (_data[10] * point.z) + (_data[14] * 1.0f);
		f32 w = (_data[3] * point.x) + (_data[7] * point.y) + (_data[11] * point.z) + (_data[15] * 1.0f);

		if (!sml::approx<f32>(w, 0.f) && !sml::approx<f32>(w, 1.f)) {
			x /= w; y /= w; z /= w;
		}
		return { x, y, z };
	}

	vector3 matrix4::MultiplyVector(const vector3& point) const noexcept {
		// Treat as direction (w = 0) - no translation
		return {
			(_data[0] * point.x) + (_data[4] * point.y) + (_data[8] * point.z),
			(_data[1] * point.x) + (_data[5] * point.y) + (_data[9] * point.z),
			(_data[2] * point.x) + (_data[6] * point.y) + (_data[10] * point.z)
		};
	}

	void matrix4::SetTRS(const vector3& pos, const quaternion& q, const vector3& s) noexcept {
		*this = matrix4::TRS(pos, q, s);
	}

	bool matrix4::ValidTRS() const noexcept {
		// Basic check: last row must be {0,0,0,1}
		if (!sml::approx<f32>(_data[3], 0.f)) return false;
		if (!sml::approx<f32>(_data[7], 0.f)) return false;
		if (!sml::approx<f32>(_data[11], 0.f)) return false;
		if (!sml::approx<f32>(_data[15], 1.f)) return false;
		return true;
	}

	void matrix4::Multiply(const matrix4& other) {
		*this = (*this) * other;
	}

	void matrix4::PreMultiply(const matrix4& other) {
		*this = other * (*this);
	}

	void matrix4::ApplyRotation(const quaternion& q, bool local) {
		matrix4 R = Rotate(q);
		if (local) {
			Multiply(R);
		}
		else {
			PreMultiply(R);
		}
	}

	void matrix4::ApplyTranslation(const vector3& t, bool local) {
		matrix4 T = Translate(t);
		if (local) {
			Multiply(T);
		}
		else {
			PreMultiply(T);
		}
	}

	void matrix4::ApplyScale(const vector3& s, bool local) {
		matrix4 S = Scale(s);
		if (local) {
			Multiply(S);
		}
		else {
			PreMultiply(S);
		}
	}

	matrix4 matrix4::Frustum(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar) noexcept {
		matrix4 m;
		f32 A = (2.f * zNear) / (right - left);
		f32 B = (2.f * zNear) / (top - bottom);
		f32 C = (right + left) / (right - left);
		f32 D = (top + bottom) / (top - bottom);
		f32 E = -(zFar + zNear) / (zFar - zNear);
		f32 F = -(2.f * zFar * zNear) / (zFar - zNear);

		// set elements by (row, col) -> stored as data[col][row]
		m[0] = A; m[4] = 0; m[8] = C; m[12] = 0;
		m[1] = 0; m[5] = B; m[9] = D; m[13] = 0;
		m[2] = 0; m[6] = 0; m[10] = E; m[14] = F;
		m[3] = 0; m[7] = 0; m[11] = -1; m[15] = 0;
		return m;
	}

	matrix4 matrix4::Frustum(const FrustumPlane& frustum) noexcept {
		return Frustum(frustum.left, frustum.right, frustum.bottom, frustum.top, frustum.zNear, frustum.zFar);
	}

	matrix4 matrix4::LookAt(const vector3& from, const vector3& to, const vector3& up) noexcept {
		vector3 f = vector3::Normalize(vector3{ to.x - from.x, to.y - from.y, to.z - from.z });
		vector3 s = vector3::Normalize(vector3::Cross(f, up));
		vector3 u = vector3::Cross(s, f);

		matrix4 m;
		// rows written into column-major storage: data[col][row]
		m[0] = s.x; m[4] = s.y; m[8] = s.z; m[12] = -vector3::Dot(s, from);
		m[1] = u.x; m[5] = u.y; m[9] = u.z; m[13] = -vector3::Dot(u, from);
		m[2] = -f.x; m[6] = -f.y; m[10] = -f.z; m[14] = vector3::Dot(f, from);
		m[3] = 0; m[7] = 0; m[11] = 0; m[15] = 1;
		return m;
	}

	matrix4 matrix4::Ortho(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar) noexcept {
		matrix4 m = matrix4::identity;
		f32 tx = -(right + left) / (right - left);
		f32 ty = -(top + bottom) / (top - bottom);
		f32 tz = -(zFar + zNear) / (zFar - zNear);

		m[0] = 2.0f / (right - left); m[4] = 0; m[8] = 0; m[12] = tx;
		m[1] = 0; m[5] = 2.0f / (top - bottom); m[9] = 0; m[13] = ty;
		m[2] = 0; m[6] = 0; m[10] = -2.0f / (zFar - zNear); m[14] = tz;
		m[3] = 0; m[7] = 0; m[11] = 0; m[15] = 1;
		return m;
	}

	matrix4 matrix4::Ortho(FrustumPlane fPlane) noexcept {
		return Ortho(fPlane.left, fPlane.right, fPlane.bottom, fPlane.top, fPlane.zNear, fPlane.zFar);
	}

	matrix4 matrix4::Perspective(f32 fov, f32 aspect, f32 zNear, f32 zFar) noexcept {
		f32 rad = (f32)(fov * sml::Deg2Rad);
		f32 f = 1.0f / (f32)sml::tan(rad / 2.0f);

		matrix4 m = matrix4::identity;
		m[0] = f / aspect; m[4] = 0; m[8] = 0; m[12] = 0;
		m[1] = 0; m[5] = f; m[9] = 0; m[13] = 0;
		m[2] = 0; m[6] = 0; m[10] = (zFar + zNear) / (zNear - zFar); m[14] = (2.0f * zFar * zNear) / (zNear - zFar);
		m[3] = 0; m[7] = 0; m[11] = -1.0f; m[15] = 0;
		return m;
	}

	matrix4 matrix4::Rotate(const quaternion& q) noexcept {
		f32 x = q[0], y = q[1], z = q[2], w = q[3];
		f32 xx = x * x, yy = y * y, zz = z * z;
		f32 xy = x * y, xz = x * z, yz = y * z;
		f32 wx = w * x, wy = w * y, wz = w * z;

		matrix4 m;
		m[0] = 1.0f - 2.0f * (yy + zz);	 m[4] = 2.0f * (xy - wz);			m[8] = 2.0f * (xz + wy);			m[12] = 0;
		m[1] = 2.0f * (xy + wz);         m[5] = 1.0f - 2.0f * (xx + zz);	m[9] = 2.0f * (yz - wx);			m[13] = 0;
		m[2] = 2.0f * (xz - wy);         m[6] = 2.0f * (yz + wx);			m[10] = 1.0f - 2.0f * (xx + yy);	m[14] = 0;
		m[3] = 0;                        m[7] = 0;							m[11] = 0;							m[15] = 1;
		return m;
	}

	void matrix4::Rotate(matrix4& m, const quaternion& q) noexcept {
		m = Rotate(q);
	}

	matrix4 matrix4::Scale(const vector3& vector) noexcept {
		matrix4 m;
		m[0] = vector.x; m[4] = 0;        m[8] = 0;			m[12] = 0;
		m[1] = 0;        m[5] = vector.y; m[9] = 0;			m[13] = 0;
		m[2] = 0;        m[6] = 0;        m[10] = vector.z; m[14] = 0;
		m[3] = 0;        m[7] = 0;        m[11] = 0;        m[15] = 1;
		return m;
	}

	void matrix4::Scale(matrix4& m, const vector3& vector) noexcept {
		m = Scale(vector);
	}

	matrix4 matrix4::Translate(const vector3& vector) noexcept {
		matrix4 m = matrix4::identity;
		m[12] = vector.x;
		m[13] = vector.y;
		m[14] = vector.z;
		return m;
	}

	void matrix4::Translate(matrix4& m, const vector3& vector) noexcept {
		m = Translate(vector);
	}

	matrix4 matrix4::TRS(const vector3& pos, const quaternion& q, const vector3& s) noexcept {
		// Apply scale then rotation then translation: T * R * S
		matrix4 S = Scale(s);
		matrix4 R = Rotate(q);
		matrix4 T = Translate(pos);
		return T * (R * S);
	}

	matrix4 matrix4::operator+(const matrix4& other) const {
		return matrix4({
			_data[0] + other[0], _data[4] + other[4], _data[8] + other[8], _data[12] + other[12],
			_data[1] + other[1], _data[5] + other[5], _data[9] + other[9], _data[13] + other[13],
			_data[2] + other[2], _data[6] + other[6], _data[10] + other[10], _data[14] + other[14],
			_data[3] + other[3], _data[7] + other[7], _data[11] + other[11], _data[15] + other[15]
		});
	}

	matrix4 matrix4::operator-(const matrix4& other) const {
		return matrix4({
			_data[0] - other[0], _data[4] - other[4], _data[8] - other[8], _data[12] - other[12],
			_data[1] - other[1], _data[5] - other[5], _data[9] - other[9], _data[13] - other[13],
			_data[2] - other[2], _data[6] - other[6], _data[10] - other[10], _data[14] - other[14],
			_data[3] - other[3], _data[7] - other[7], _data[11] - other[11], _data[15] - other[15]
		});
	}

	matrix4 matrix4::operator*(const f32& scalar) const {
		return matrix4({
			_data[0] * scalar, _data[4] * scalar, _data[8] * scalar, _data[12] * scalar,
			_data[1] * scalar, _data[5] * scalar, _data[9] * scalar, _data[13] * scalar,
			_data[2] * scalar, _data[6] * scalar, _data[10] * scalar, _data[14] * scalar,
			_data[3] * scalar, _data[7] * scalar, _data[11] * scalar, _data[15] * scalar
		});
	}

	matrix4 matrix4::operator*(const matrix4& other) const {
		matrix4 out;
		for (u32 c = 0; c < 4; ++c) {
			for (u32 r = 0; r < 4; ++r) {
				f32 sum = 0.0f;
				for (u32 k = 0; k < 4; ++k) {
					sum += _data[k * 4 + r] * other[c * 4 + k];
				}
				out[c * 4 + r] = sum;
			}
		}
		return out;
	}

	vector3 matrix4::operator*(const vector3& other) const {
		return {
			(_data[0] * other.x) + (_data[4] * other.y) + (_data[8] * other.z) + (_data[12] * 1.0f),
			(_data[1] * other.x) + (_data[5] * other.y) + (_data[9] * other.z) + (_data[13] * 1.0f),
			(_data[2] * other.x) + (_data[6] * other.y) + (_data[10] * other.z) + (_data[14] * 1.0f)
		};
	}

	bool matrix4::operator==(const matrix4& other) const {
		return {
			_data[0] == other[0] && _data[4] == other[4] && _data[8] == other[8] && _data[12] == other[12] &&
			_data[1] == other[1] && _data[5] == other[5] && _data[9] == other[9] && _data[13] == other[13] &&
			_data[2] == other[2] && _data[6] == other[6] && _data[10] == other[10] && _data[14] == other[14] &&
			_data[3] == other[3] && _data[7] == other[7] && _data[11] == other[11] && _data[15] == other[15]
		};
	}

	bool matrix4::operator!=(const matrix4& other) const {
		return {
			_data[0] != other[0] || _data[4] != other[4] || _data[8] != other[8] || _data[12] != other[12] ||
			_data[1] != other[1] || _data[5] != other[5] || _data[9] != other[9] || _data[13] != other[13] ||
			_data[2] != other[2] || _data[6] != other[6] || _data[10] != other[10] || _data[14] != other[14] ||
			_data[3] != other[3] || _data[7] != other[7] || _data[11] != other[11] || _data[15] != other[15]
		};
	}
}