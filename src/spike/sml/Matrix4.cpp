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

#include "Matrix4.h"

#include <cassert>
#include <string>

#include <spike/def_aliases.h>
#include <spike/stl_aliases.h>

#include "Vector3.h"
#include "Quaternion.h"
#include "FrustumPlane.h"
#include "SMLcore.h"

namespace sml {
	matrix4::MatrixRow matrix4::MatrixRow::zero = MatrixRow();

	matrix4 matrix4::identity = matrix4(
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	);

	matrix4 matrix4::zero = matrix4();

	matrix4::MatrixRow matrix4::GetColumn(const u16 index) const {
		assert(index < 4 && "Index Out of Range! Column in range 0-3.");

		return data[index];
	}

	matrix4::MatrixRow matrix4::GetRow(const u16 index) const {
		assert(index < 4 && "Index Out of Range! Column in range 0-3.");

		return { data[0][index], data[1][index], data[2][index], data[3][index] };
	}

	void matrix4::SetColumn(const u16 index, const matrix4::MatrixRow& column) {
		assert(index < 4 && "Index Out of Range! Column in range 0-3.");

		data[index] = column;
	}

	void matrix4::SetRow(const u16 index, const matrix4::MatrixRow& row) {
		assert(index < 4 && "Index Out of Range! Column in range 0-3.");

		data[0][index] = row[0];
		data[1][index] = row[1];
		data[2][index] = row[2];
		data[3][index] = row[3];
	}

	vector3	matrix4::GetPosition() const noexcept {
		MatrixRow pos = GetColumn(3);
		return { pos[0], pos[1], pos[2] };
	}

	f32 matrix4::Determinant() const noexcept {
		// Determinant and Inverse use the standard 4x4 cofactor / adjugate method (robust double-precision internal)
		// Use the same intermediate cofactors as used in Inverse
		f64 m[16];
		for (u32 r = 0; r < 4; ++r) {
			for (u32 c = 0; c < 4; ++c) {
				m[r * 4 + c] = data[c][r];
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
				m[r * 4 + c] = data[c][r];
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
				out[c][r] = (f32)(inv[r * 4 + c] * invDet);
			}
		}
		return out;
	}

	bool matrix4::IsIdentity() const noexcept {
		return *this == matrix4::identity;
	}

	quaternion matrix4::Rotation() const noexcept {
		// Extract quaternion from the upper-left 3x3 rotation
		f32 m00 = data[0][0], m01 = data[1][0], m02 = data[2][0];
		f32 m10 = data[0][1], m11 = data[1][1], m12 = data[2][1];
		f32 m20 = data[0][2], m21 = data[1][2], m22 = data[2][2];

		f32 trace = m00 + m11 + m22;
		f32 x, y, z, w;

		if (trace > 0.0f) {
			f32 s = (f32)sml::sqrt(trace + 1.0f) * 2.0f; // s = 4*w
			w = 0.25f * s;
			x = (m21 - m12) / s;
			y = (m02 - m20) / s;
			z = (m10 - m01) / s;
		}
		else if ((m00 > m11) && (m00 > m22)) {
			f32 s = (f32)sml::sqrt(1.0f + m00 - m11 - m22) * 2.0f; // s = 4*x
			w = (m21 - m12) / s;
			x = 0.25f * s;
			y = (m01 + m10) / s;
			z = (m02 + m20) / s;
		}
		else if (m11 > m22) {
			f32 s = (f32)sml::sqrt(1.0f + m11 - m00 - m22) * 2.0f; // s = 4*y
			w = (m02 - m20) / s;
			x = (m01 + m10) / s;
			y = 0.25f * s;
			z = (m12 + m21) / s;
		}
		else {
			f32 s = (f32)sml::sqrt(1.0f + m22 - m00 - m11) * 2.0f; // s = 4*z
			w = (m10 - m01) / s;
			x = (m02 + m20) / s;
			y = (m12 + m21) / s;
			z = 0.25f * s;
		}

		return quaternion(x, y, z, w).Normalized();
	}

	vector3 matrix4::Scale() const noexcept {
		auto c0 = GetColumn(0);
		auto c1 = GetColumn(1);
		auto c2 = GetColumn(2);
		return {
			sml::sqrt(c0[0] * c0[0] + c0[1] * c0[1] + c0[2] * c0[2]),
			sml::sqrt(c1[0] * c1[0] + c1[1] * c1[1] + c1[2] * c1[2]),
			sml::sqrt(c2[0] * c2[0] + c2[1] * c2[1] + c2[2] * c2[2])
		};
	}

	matrix4 matrix4::Transpose() const noexcept {
		matrix4 out;
		for (u32 r = 0; r < 4; ++r) {
			for (u32 c = 0; c < 4; ++c) {
				out[c][r] = data[r][c];
			}
		}
		return out;
	}

	vector3 matrix4::MultiplyPoint(const vector3& point) const noexcept {
		f32 x = (data[0][0] * point.x) + (data[1][0] * point.y) + (data[2][0] * point.z) + (data[3][0] * 1.0f);
		f32 y = (data[0][1] * point.x) + (data[1][1] * point.y) + (data[2][1] * point.z) + (data[3][1] * 1.0f);
		f32 z = (data[0][2] * point.x) + (data[1][2] * point.y) + (data[2][2] * point.z) + (data[3][2] * 1.0f);
		f32 w = (data[0][3] * point.x) + (data[1][3] * point.y) + (data[2][3] * point.z) + (data[3][3] * 1.0f);

		if (!sml::approx<f32>(w, 0.0f) && !sml::approx<f32>(w, 1.0f)) {
			x /= w; y /= w; z /= w;
		}
		return { x, y, z };
	}

	vector3 matrix4::MultiplyVector(const vector3& point) const noexcept {
		// Treat as direction (w = 0) - no translation
		return {
			(data[0][0] * point.x) + (data[1][0] * point.y) + (data[2][0] * point.z),
			(data[0][1] * point.x) + (data[1][1] * point.y) + (data[2][1] * point.z),
			(data[0][2] * point.x) + (data[1][2] * point.y) + (data[2][2] * point.z)
		};
	}

	void matrix4::SetTRS(const vector3& pos, const quaternion& q, const vector3& s) noexcept {
		*this = matrix4::TRS(pos, q, s);
	}

	bool matrix4::ValidTRS() const noexcept {
		// Basic check: last row must be {0,0,0,1}
		if (!sml::approx<f32>(data[0][3], 0.0f)) return false;
		if (!sml::approx<f32>(data[1][3], 0.0f)) return false;
		if (!sml::approx<f32>(data[2][3], 0.0f)) return false;
		if (!sml::approx<f32>(data[3][3], 1.0f)) return false;
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

	const f32* matrix4::DataPtr() noexcept {
		return &data[0][0];
	}

	matrix4 matrix4::Frustum(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar) noexcept {
		matrix4 m;
		f32 A = (2.0f * zNear) / (right - left);
		f32 B = (2.0f * zNear) / (top - bottom);
		f32 C = (right + left) / (right - left);
		f32 D = (top + bottom) / (top - bottom);
		f32 E = -(zFar + zNear) / (zFar - zNear);
		f32 F = -(2.0f * zFar * zNear) / (zFar - zNear);

		// set elements by (row, col) -> stored as data[col][row]
		m[0][0] = A; m[1][0] = 0; m[2][0] = C; m[3][0] = 0;
		m[0][1] = 0; m[1][1] = B; m[2][1] = D; m[3][1] = 0;
		m[0][2] = 0; m[1][2] = 0; m[2][2] = E; m[3][2] = F;
		m[0][3] = 0; m[1][3] = 0; m[2][3] = -1; m[3][3] = 0;
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
		m[0][0] = s.x; m[1][0] = s.y; m[2][0] = s.z; m[3][0] = -vector3::Dot(s, from);
		m[0][1] = u.x; m[1][1] = u.y; m[2][1] = u.z; m[3][1] = -vector3::Dot(u, from);
		m[0][2] = -f.x; m[1][2] = -f.y; m[2][2] = -f.z; m[3][2] = vector3::Dot(f, from);
		m[0][3] = 0; m[1][3] = 0; m[2][3] = 0; m[3][3] = 1;
		return m;
	}

	matrix4 matrix4::Ortho(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar) noexcept {
		matrix4 m = matrix4::identity;
		f32 tx = -(right + left) / (right - left);
		f32 ty = -(top + bottom) / (top - bottom);
		f32 tz = -(zFar + zNear) / (zFar - zNear);

		m[0][0] = 2.0f / (right - left); m[1][0] = 0; m[2][0] = 0; m[3][0] = tx;
		m[0][1] = 0; m[1][1] = 2.0f / (top - bottom); m[2][1] = 0; m[3][1] = ty;
		m[0][2] = 0; m[1][2] = 0; m[2][2] = -2.0f / (zFar - zNear); m[3][2] = tz;
		m[0][3] = 0; m[1][3] = 0; m[2][3] = 0; m[3][3] = 1;
		return m;
	}

	matrix4 matrix4::Ortho(FrustumPlane fPlane) noexcept {
		return Ortho(fPlane.left, fPlane.right, fPlane.bottom, fPlane.top, fPlane.zNear, fPlane.zFar);
	}

	matrix4 matrix4::Perspective(f32 fov, f32 aspect, f32 zNear, f32 zFar) noexcept {
		f32 rad = (f32)(fov * sml::Deg2Rad);
		f32 f = 1.0f / (f32)sml::tan(rad / 2.0f);

		matrix4 m = matrix4::identity;
		m[0][0] = f / aspect; m[1][0] = 0; m[2][0] = 0; m[3][0] = 0;
		m[0][1] = 0; m[1][1] = f; m[2][1] = 0; m[3][1] = 0;
		m[0][2] = 0; m[1][2] = 0; m[2][2] = (zFar + zNear) / (zNear - zFar); m[3][2] = (2.0f * zFar * zNear) / (zNear - zFar);
		m[0][3] = 0; m[1][3] = 0; m[2][3] = -1.0f; m[3][3] = 0;
		return m;
	}

	matrix4 matrix4::Rotate(const quaternion& q) noexcept {
		f32 x = q[0], y = q[1], z = q[2], w = q[3];
		f32 xx = x * x, yy = y * y, zz = z * z;
		f32 xy = x * y, xz = x * z, yz = y * z;
		f32 wx = w * x, wy = w * y, wz = w * z;

		matrix4 m;
		m[0][0] = 1.0f - 2.0f * (yy + zz);	m[1][0] = 2.0f * (xy - wz);			m[2][0] = 2.0f * (xz + wy);			m[3][0] = 0;
		m[0][1] = 2.0f * (xy + wz);         m[1][1] = 1.0f - 2.0f * (xx + zz);	m[2][1] = 2.0f * (yz - wx);			m[3][1] = 0;
		m[0][2] = 2.0f * (xz - wy);         m[1][2] = 2.0f * (yz + wx);			m[2][2] = 1.0f - 2.0f * (xx + yy);	m[3][2] = 0;
		m[0][3] = 0;                        m[1][3] = 0;						m[2][3] = 0;						m[3][3] = 1;
		return m;
	}

	void matrix4::Rotate(matrix4& m, const quaternion& q) noexcept {
		m = Rotate(q);
	}

	matrix4 matrix4::Scale(const vector3& vector) noexcept {
		matrix4 m;
		m[0][0] = vector.x; m[1][0] = 0;        m[2][0] = 0;        m[3][0] = 0;
		m[0][1] = 0;        m[1][1] = vector.y; m[2][1] = 0;        m[3][1] = 0;
		m[0][2] = 0;        m[1][2] = 0;        m[2][2] = vector.z; m[3][2] = 0;
		m[0][3] = 0;        m[1][3] = 0;        m[2][3] = 0;        m[3][3] = 1;
		return m;
	}

	void matrix4::Scale(matrix4& m, const vector3& vector) noexcept {
		m = Scale(vector);
	}

	matrix4 matrix4::Translate(const vector3& vector) noexcept {
		matrix4 m = matrix4::identity;
		m[3][0] = vector.x;
		m[3][1] = vector.y;
		m[3][2] = vector.z;
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

	matrix4::MatrixRow matrix4::MatrixRow::operator+(const MatrixRow& other) const {
		return { data[0] + other[0], data[1] + other[1], data[2] + other[2], data[3] + other[3] };
	}

	matrix4::MatrixRow matrix4::MatrixRow::operator-(const MatrixRow& other) const {
		return { data[0] - other[0], data[1] - other[1], data[2] - other[2], data[3] - other[3] };
	}

	matrix4::MatrixRow matrix4::MatrixRow::operator*(const f32& scalar) const {
		return { data[0] * scalar, data[1] * scalar, data[2] * scalar, data[3] * scalar };
	}

	f32 matrix4::MatrixRow::operator*(const MatrixRow& other) const {
		return (data[0] * other[0]) + (data[1] * other[1]) + (data[2] * other[2]) + (data[3] * other[3]);
	}

	bool matrix4::MatrixRow::operator==(const MatrixRow& other) const {
		return data[0] == other[0] &&
			data[1] == other[1] &&
			data[2] == other[2] &&
			data[3] == other[3];
	}

	bool matrix4::MatrixRow::operator!=(const MatrixRow& other) const {
		return data[0] != other[0] ||
			data[1] != other[1] ||
			data[2] != other[2] ||
			data[3] != other[3];
	}

	matrix4 matrix4::operator+(const matrix4& other) const {
		return { data[0] + other[0], data[1] + other[1], data[2] + other[2], data[3] + other[3] };
	}

	matrix4 matrix4::operator-(const matrix4& other) const {
		return { data[0] - other[0], data[1] - other[1], data[2] - other[2], data[3] - other[3] };
	}

	matrix4 matrix4::operator*(const f32& scalar) const {
		return { data[0] * scalar, data[1] * scalar, data[2] * scalar, data[3] * scalar };
	}

	matrix4 matrix4::operator*(const matrix4& other) const {
		matrix4 out;
		for (u32 c = 0; c < 4; ++c) {
			for (u32 r = 0; r < 4; ++r) {
				f32 sum = 0.0f;
				for (u32 k = 0; k < 4; ++k) {
					sum += data[k][r] * other.data[c][k];
				}
				out[c][r] = sum;
			}
		}
		return out;
	}

	vector3 matrix4::operator*(const vector3& other) const {
		return {
			(data[0][0] * other.x) + (data[1][0] * other.y) + (data[2][0] * other.z) + (data[3][0] * 1.0f),
			(data[0][1] * other.x) + (data[1][1] * other.y) + (data[2][1] * other.z) + (data[3][1] * 1.0f),
			(data[0][2] * other.x) + (data[1][2] * other.y) + (data[2][2] * other.z) + (data[3][2] * 1.0f)
		};
	}

	bool matrix4::operator==(const matrix4& other) const {
		return data[0] == other[0] &&
			data[1] == other[1] &&
			data[2] == other[2] &&
			data[3] == other[3];
	}

	bool matrix4::operator!=(const matrix4& other) const {
		return data[0] != other[0] ||
			data[1] != other[1] ||
			data[2] != other[2] ||
			data[3] != other[3];
	}

	string matrix4::MatrixRow::ToString() const noexcept {
		return "{" + std::to_string(data[0]) + ", " + std::to_string(data[1]) + ", " + std::to_string(data[2]) + ", " + std::to_string(data[3]) + "}";
	}

	string matrix4::ToString() const noexcept {
		return "{" + data[0].ToString() + ", " + data[1].ToString() + ", " + data[2].ToString() + ", " + data[3].ToString() + "}";
	}
}