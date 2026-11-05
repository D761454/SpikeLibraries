/*
def_aliases.h - spike libraries
----------------------------------------------------------------------

Copyright (c) 2026-2026, SpikeTooth, Cayden Andrews

All rights reserved.

* Re-distributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Re-distributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of SpikeTooth, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the SpikeTooth team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------
*/

#ifndef __DEF_ALIASES_H
#define __DEF_ALIASES_H

#include <cstdint>
#include <limits>
#include <cmath>

/*
----------------------------------------------------------------------

File Summary:
	This file holds default aliases for fixed size data types and
	constants for use in projects.

----------------------------------------------------------------------
*/

#pragma region HalfFloat

/*
----------------------------------------------------------------------

Region Summary:
	This code region houses half_float which is provided under the
	following notice. Neither Spike Tooth, spike libraries or its
	contributors claim ownership of half_float, this is merely here for
	convenience.

half_float copyright notice:
	Copyright (c) 2006-2008,
	Chris "Krishty" Maiwald, Alexander "Aramis" Gessler

	All rights reserved.

	Redistribution and use of this software in source and binary forms,
	with or without modification, are permitted provided that the
	following conditions are met:

	* Redistributions of source code must retain the above
	  copyright notice, this list of conditions and the
	  following disclaimer.

	* Redistributions in binary form must reproduce the above
	  copyright notice, this list of conditions and the
	  following disclaimer in the documentation and/or other
	  materials provided with the distribution.

	* Neither the name of the class, nor the names of its
	  contributors may be used to endorse or promote products
	  derived from this software without specific prior
	  written permission of the Development Team.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------
*/

// ============================================================================
// Declarations
// ============================================================================

struct float16_t {
#pragma warning(suppress: 26495)
	inline float16_t() noexcept {}

	inline float16_t(const float16_t& other) : bits(other.GetBits()) {}

	inline float16_t(uint16_t _m, uint16_t _e, uint16_t _s);

	inline float16_t(float other);
	inline float16_t(const double);

	inline operator float() const;
	inline operator double() const;

private:
	friend float16_t operator+ (float16_t, float16_t);
	friend float16_t operator- (float16_t, float16_t);
	friend float16_t operator* (float16_t, float16_t);
	friend float16_t operator/ (float16_t, float16_t);

	union {
		uint16_t bits;
		struct {
			uint16_t Frac : 10;		// mantissa
			uint16_t Exp : 5;		// exponent
			uint16_t Sign : 1;		// sign
		} IEEE;
	};


	union IEEESingle {
		float Float;
		struct {
			uint32_t Frac : 23;
			uint32_t Exp : 8;
			uint32_t Sign : 1;
		} IEEE;
	};

	union IEEEDouble {
		double Double;
		struct {
			uint64_t Frac : 52;
			uint64_t Exp : 11;
			uint64_t Sign : 1;
		} IEEE;
	};

public:

	static constexpr int32_t kBitsMantissa = 10;
	static constexpr int32_t kBitsExponent = 5;
	static constexpr int32_t kMaxExponentValue = 31;
	static constexpr int32_t kBias = kMaxExponentValue / 2;
	static constexpr int32_t kMaxExponent = kBias;
	static constexpr int32_t kMinExponent = -kBias;

	static constexpr int32_t kMaxExponent10 = 9;
	static constexpr int32_t kMinExponent10 = -9;

	static constexpr int64_t kIEEEDoubleMaxExponent = 0x7FF;
	static constexpr int64_t kIEEEDoubleExponentBias = kIEEEDoubleMaxExponent / 2;

	inline uint16_t GetBits() const;
	inline uint16_t& GetBits();

	inline bool IsNaN() const;
	inline bool IsInfinity() const;
	inline bool IsDenorm() const;

	inline bool GetSign() const;

	inline float16_t& operator=(float16_t other);
	inline float16_t& operator=(float other);
	inline float16_t& operator=(const double other);

	inline bool operator== (float16_t other) const;
	inline bool operator!= (float16_t other) const;

	inline bool operator<(float16_t other) const;
	inline bool operator>(float16_t other) const;
	inline bool operator<=(float16_t other) const;
	inline bool operator>=(float16_t other) const;

	inline bool operator<(float other) const;
	inline bool operator>(float other) const;
	inline bool operator<=(float other) const;
	inline bool operator>=(float other) const;

	inline float16_t& operator+=(float16_t other);
	inline float16_t& operator-=(float16_t other);
	inline float16_t& operator*=(float16_t other);
	inline float16_t& operator/=(float16_t other);

	inline float16_t& operator+=(float other);
	inline float16_t& operator-=(float other);
	inline float16_t& operator*=(float other);
	inline float16_t& operator/=(float other);

	inline float16_t& operator++();
	inline float16_t operator++(int);

	inline float16_t& operator--();
	inline float16_t operator--(int);

	inline float16_t operator-() const;
};

inline float16_t operator+ (float16_t one, float16_t two);
inline float16_t operator- (float16_t one, float16_t two);
inline float16_t operator* (float16_t one, float16_t two);
inline float16_t operator/ (float16_t one, float16_t two);

inline float operator+ (float16_t one, float two);
inline float operator- (float16_t one, float two);
inline float operator* (float16_t one, float two);
inline float operator/ (float16_t one, float two);

inline float operator+ (float one, float16_t two);
inline float operator- (float one, float16_t two);
inline float operator* (float one, float16_t two);
inline float operator/ (float one, float16_t two);

namespace std {
	template<>
	class numeric_limits<float16_t> {
	public:
		static const bool is_specialized = true;

		static const int radix = 2;
		static const int digits = 10;   // conservative assumption
		static const int digits10 = 2;  // conservative assumption
		static const bool is_signed = true;
		static const bool is_integer = true;
		static const bool is_exact = false;
		static const bool traps = false;
		static const bool is_modulo = false;
		static const bool is_bounded = true;

		static const int min_exponent10 = float16_t::kMinExponent10;
		static const int max_exponent10 = float16_t::kMaxExponent10;
		static const int min_exponent = float16_t::kMinExponent;
		static const int max_exponent = float16_t::kMaxExponent;

		static const bool has_infinity = true;
		static const bool has_quiet_NaN = true;
		static const bool has_signaling_NaN = true;
		static const bool is_iec559 = false;
		static const bool has_denorm = denorm_present;
		static const bool tinyness_before = false;
		static const float_round_style round_style = round_to_nearest;

		static float16_t min() {
			return float16_t(0, 1, 0);
		}

		static float16_t max() {
			return float16_t(~0, float16_t::kMaxExponentValue - 1, 0);
		}

		static float16_t epsilon() {
			return float16_t(0.00097656f);
		}

		static float16_t round_error() {
			return float16_t(0.00097656f / 2);
		}

		static float16_t denorm_min() {
			return float16_t(1, 0, 1);
		}

		static float16_t infinity() {
			return float16_t(0, float16_t::kMaxExponentValue, 0);
		}

		static float16_t quiet_NaN() {
			return float16_t(1, float16_t::kMaxExponentValue, 0);
		}

		static float16_t signaling_NaN() {
			return float16_t(1, float16_t::kMaxExponentValue, 0);
		}
	};
};

// ============================================================================
// Implementation
// ============================================================================

inline float16_t::float16_t(float other) {
	IEEESingle f;
	f.Float = other;

	IEEE.Sign = f.IEEE.Sign;

	if (!f.IEEE.Exp) {
		IEEE.Frac = 0;
		IEEE.Exp = 0;
	}
	else if (f.IEEE.Exp == 0xff) {
		// NaN or INF
		IEEE.Frac = (f.IEEE.Frac != 0) ? 1 : 0;
		IEEE.Exp = 31;
	}
	else {
		// regular number
		int new_exp = f.IEEE.Exp - 127;

		if (new_exp < -24) { // this maps to 0
			IEEE.Frac = 0;
			IEEE.Exp = 0;
		}

		else if (new_exp < -14) {
			// this maps to a denorm
			IEEE.Exp = 0;
			unsigned int exp_val = (unsigned int)(-14 - new_exp);  // 2^-exp_val
			switch (exp_val) {
			case 0:
				IEEE.Frac = 0;
				break;
			case 1: IEEE.Frac = 512 + (f.IEEE.Frac >> 14); break;
			case 2: IEEE.Frac = 256 + (f.IEEE.Frac >> 15); break;
			case 3: IEEE.Frac = 128 + (f.IEEE.Frac >> 16); break;
			case 4: IEEE.Frac = 64 + (f.IEEE.Frac >> 17); break;
			case 5: IEEE.Frac = 32 + (f.IEEE.Frac >> 18); break;
			case 6: IEEE.Frac = 16 + (f.IEEE.Frac >> 19); break;
			case 7: IEEE.Frac = 8 + (f.IEEE.Frac >> 20); break;
			case 8: IEEE.Frac = 4 + (f.IEEE.Frac >> 21); break;
			case 9: IEEE.Frac = 2 + (f.IEEE.Frac >> 22); break;
			case 10: IEEE.Frac = 1; break;
			}
		}
		else if (new_exp > 15) { // map this value to infinity
			IEEE.Frac = 0;
			IEEE.Exp = 31;
		}
		else {
			IEEE.Exp = new_exp + 15;
			IEEE.Frac = (f.IEEE.Frac >> 13);
		}
	}
}

inline float16_t::float16_t(const double p_Reference) {
	const IEEEDouble& l_Reference = reinterpret_cast<const IEEEDouble&>(p_Reference);

	// Copy the sign bit.
	this->IEEE.Sign = l_Reference.IEEE.Sign;

	// Check for special values: Is the exponent zero?
	if (0 == l_Reference.IEEE.Exp) {
		// A zero exponent indicates either a zero or a subnormal number. A subnormal float can not
		//	be represented as a half, so either one will be saved as a zero.
		this->IEEE.Exp = 0;
		this->IEEE.Frac = 0;
	}
	// Is the exponent all one?
	else if (kIEEEDoubleMaxExponent == l_Reference.IEEE.Exp) {
		this->IEEE.Exp = kMaxExponentValue;
		// A zero fraction indicates an Infinite value.
		if (0 == l_Reference.IEEE.Frac) this->IEEE.Frac = 0;
		// A nonzero fraction indicates NaN. Such a fraction contains further information, e.g. to
		//	distinguish a QNaN from a SNaN. However, we can not just shift-copy the fraction:
		//	if the first five bits were zero we would save an infinite value, so we abandon the
		//	fraction information and set it to a nonzero value.
		else this->IEEE.Frac = 1;
	}
	// A usual value?
	else {
		// First, we have to adjust the exponent. It is stored as an unsigned int, to reconstruct
		//	its original value we have to subtract its bias (half of its range).
		const int64_t l_AdjustedExponent = l_Reference.IEEE.Exp - kIEEEDoubleExponentBias;

		// Very small values will be rounded to zero.
		if (-24 > l_AdjustedExponent) {
			this->IEEE.Frac = 0;
			this->IEEE.Exp = 0;
		}
		// Some small values can be stored as subnormal values.
		else if (-14 > l_AdjustedExponent) {
			// The exponent of subnormal values is always be zero.
			this->IEEE.Exp = 0;
			// The exponent will now be stored in the fraction.
			const int16_t l_NewExponent = int16_t(-14 - l_AdjustedExponent);  // 2 ^ -l_NewExponent
			this->IEEE.Frac = (1024 >> l_NewExponent) + int16_t(l_Reference.IEEE.Frac >> (42 + l_NewExponent));
		}
		// Very large numbers will be rounded to infinity.
		else if (15 < l_AdjustedExponent) {
			// Exponent all one, fraction zero.
			this->IEEE.Exp = kMaxExponentValue;
			this->IEEE.Frac = 0;
		}
		// All remaining numbers can be converted directly.
		else {
			// We reconstructed the exponent by subtracting the bias. To store it as an unsigned
			//	int, we need to add the bias again.
			this->IEEE.Exp = l_AdjustedExponent + kBias;
			// When storing the fraction, we abandon its least significant bits by right-shifting.
			//	The fraction of a double is 42 bits wider than that of a half, so we shift 42 bits.
			this->IEEE.Frac = (l_Reference.IEEE.Frac >> 42);
		};
	}; // else usual number
}

inline float16_t::float16_t(uint16_t _m, uint16_t _e, uint16_t _s) {
	IEEE.Frac = _m;
	IEEE.Exp = _e;
	IEEE.Sign = _s;
}

float16_t::operator float() const {
	IEEESingle sng;
	sng.IEEE.Sign = IEEE.Sign;

	if (!IEEE.Exp) {
		if (!IEEE.Frac) {
			sng.IEEE.Frac = 0;
			sng.IEEE.Exp = 0;
		}
		else {
			const float half_denorm = (1.0f / 16384.0f);
			float mantissa = ((float)(IEEE.Frac)) / 1024.0f;
			float sgn = (IEEE.Sign) ? -1.0f : 1.0f;
			sng.Float = sgn * mantissa * half_denorm;
		}
	}
	else if (31 == IEEE.Exp) {
		sng.IEEE.Exp = 0xff;
		sng.IEEE.Frac = (IEEE.Frac != 0) ? 1 : 0;
	}
	else {
		sng.IEEE.Exp = IEEE.Exp + 112;
		sng.IEEE.Frac = (IEEE.Frac << 13);
	}
	return sng.Float;
}

inline float16_t::operator double(void) const {
	IEEEDouble l_Result;

	// Copy the sign bit.
	l_Result.IEEE.Sign = this->IEEE.Sign;

	// In a zero, both the exponent and the fraction are zero.
	if ((0 == this->IEEE.Exp) && (0 == this->IEEE.Frac)) {
		l_Result.IEEE.Exp = 0;
		l_Result.IEEE.Frac = 0;
	}
	// If the exponent is zero and the fraction is nonzero, the number is subnormal.
	else if ((0 == this->IEEE.Exp) && (0 != this->IEEE.Frac)) {
		// sign * 2^-14 * fraction
		l_Result.Double = (this->IEEE.Sign ? -1.0 : +1.0) / 16384.0 * (double(this->IEEE.Frac) / 1024.0);
	}
	// Is the exponent all one?
	else if (kMaxExponentValue == this->IEEE.Exp) {
		l_Result.IEEE.Exp = kIEEEDoubleMaxExponent;
		// A zero fraction indicates an infinite value.
		if (0 == this->IEEE.Frac) l_Result.IEEE.Frac = 0;
		// A nonzero fraction indicates a NaN. We can re-use the fraction information: a double
		//	fraction is 42 bits wider than a half fraction, so we can just left-shift it. Any
		//	information on QNaNs or SNaNs will be preserved.
		else l_Result.IEEE.Frac = uint64_t(this->IEEE.Frac) << 42;
	}
	// A usual value?
	else {
		// The exponent is stored as an unsigned int. To reconstruct its original value, we have to
		//	subtract its bias. To re-store it in a wider bit field, we must add the bias of the new
		//	bit field.
		l_Result.IEEE.Exp = uint64_t(this->IEEE.Exp) - kBias + kIEEEDoubleExponentBias;
		// A double fraction is 42 bits wider than a half fraction, so we can just left-shift it.
		l_Result.IEEE.Frac = uint64_t(this->IEEE.Frac) << 42;
	}
	return l_Result.Double;
}

inline bool float16_t::IsNaN() const {
	return IEEE.Frac != 0 && IEEE.Exp == kMaxExponentValue;
}

inline bool float16_t::IsInfinity() const {
	return IEEE.Frac == 0 && IEEE.Exp == kMaxExponentValue;
}

inline bool float16_t::IsDenorm() const {
	return IEEE.Exp == 0;
}

inline bool float16_t::GetSign() const {
	return IEEE.Sign == 0;
}

inline uint16_t float16_t::GetBits() const {
	return bits;
}

inline uint16_t& float16_t::GetBits() {
	return bits;
}

#pragma region Operators

inline float16_t& float16_t::operator= (float16_t other) {
	bits = other.GetBits();
	return *this;
}

inline float16_t& float16_t::operator= (float other) {
	*this = (float16_t)other;
	return *this;
}

inline float16_t& float16_t::operator= (const double p_Reference) {
	return (*this) = float16_t(p_Reference);
}

inline bool float16_t::operator== (float16_t other) const {
	// +0 and -0 are considered to be equal
	if (!(bits << 1u) && !(other.bits << 1u))return true;

	return bits == other.bits && !this->IsNaN();
}

inline bool float16_t::operator!= (float16_t other) const {
	// +0 and -0 are considered to be equal
	if (!(bits << 1u) && !(other.bits << 1u))return false;

	return bits != other.bits || this->IsNaN();
}

inline bool float16_t::operator<  (float16_t other) const {
	// NaN comparisons are always false
	if (this->IsNaN() || other.IsNaN())
		return false;

	// this works since the segment oder is s,e,m.
	return (int16_t)this->bits < (int16_t)other.GetBits();
}

inline bool float16_t::operator>  (float16_t other) const {
	// NaN comparisons are always false
	if (this->IsNaN() || other.IsNaN())
		return false;

	// this works since the segment oder is s,e,m.
	return (int16_t)this->bits > (int16_t)other.GetBits();
}

inline bool float16_t::operator<= (float16_t other) const {
	return !(*this > other);
}

inline bool float16_t::operator>= (float16_t other) const {
	return !(*this < other);
}

inline float16_t& float16_t::operator += (float16_t other) {
	*this = (*this) + other;
	return *this;
}

inline float16_t& float16_t::operator -= (float16_t other) {
	*this = (*this) - other;
	return *this;
}

inline float16_t& float16_t::operator *= (float16_t other) {
	*this = (float)(*this) * (float)other;
	return *this;
}

inline float16_t& float16_t::operator /= (float16_t other) {
	*this = (float)(*this) / (float)other;
	return *this;
}

inline float16_t& float16_t::operator += (float other) {
	*this = (*this) + (float16_t)other;
	return *this;
}

inline float16_t& float16_t::operator -= (float other) {
	*this = (*this) - (float16_t)other;
	return *this;
}

inline float16_t& float16_t::operator *= (float other) {
	*this = (float)(*this) * other;
	return *this;
}

inline float16_t& float16_t::operator /= (float other) {
	*this = (float)(*this) / other;
	return *this;
}

inline float16_t& float16_t::operator++() {
	// setting the exponent to bias means using 0 as exponent - thus we
	// can set the mantissa to any value we like, we'll always get 1.0
	return this->operator+=(float16_t(0, kBias, 0));
}

inline float16_t float16_t::operator++(int) {
	float16_t f = *this;
	this->operator+=(float16_t(0, kBias, 0));
	return f;
}

inline float16_t& float16_t::operator--() {
	return this->operator-=(float16_t(0, kBias, 0));
}

inline float16_t float16_t::operator--(int) {
	float16_t f = *this;
	this->operator-=(float16_t(0, kBias, 0));
	return f;
}

inline float16_t float16_t::operator-() const {
	return float16_t(IEEE.Frac, IEEE.Exp, ~IEEE.Sign);
}

inline float16_t operator+(float16_t one, float16_t two)
{
#if (!defined HALFFLOAT_NO_CUSTOM_IMPLEMENTATIONS)

	if (one.IEEE.Exp == float16_t::kMaxExponentValue) {
		// if one of the components is NaN the result becomes NaN, too.
		if (0 != one.IEEE.Frac || two.IsNaN())
			return float16_t(1, float16_t::kMaxExponentValue, 0);

		// otherwise this must be infinity
		return float16_t(0, float16_t::kMaxExponentValue, one.IEEE.Sign | two.IEEE.Sign);
	}
	else if (two.IEEE.Exp == float16_t::kMaxExponentValue) {
		if (one.IsNaN() || 0 != two.IEEE.Frac) return float16_t(1, float16_t::kMaxExponentValue, 0);

		return float16_t(0, float16_t::kMaxExponentValue, one.IEEE.Sign | two.IEEE.Sign);
	}

	float16_t out;
	long m1, m2, temp;

	// compute the difference between the two exponents. shifts with negative
	// numbers are undefined, thus we need two code paths
	int expDiff = one.IEEE.Exp - two.IEEE.Exp;

	if (0 == expDiff) {
		// the exponents are equal, thus we must just add the hidden bit
		temp = two.IEEE.Exp;

		if (0 == one.IEEE.Exp)m1 = one.IEEE.Frac;
		else m1 = (int)one.IEEE.Frac | (1 << float16_t::kBitsMantissa);

		if (0 == two.IEEE.Exp)m2 = two.IEEE.Frac;
		else m2 = (int)two.IEEE.Frac | (1 << float16_t::kBitsMantissa);
	}
	else {
		if (expDiff < 0)
		{
			expDiff = -expDiff;
			std::swap(one, two);
		}

		m1 = (int)one.IEEE.Frac | (1 << float16_t::kBitsMantissa);

		if (0 == two.IEEE.Exp)m2 = two.IEEE.Frac;
		else m2 = (int)two.IEEE.Frac | (1 << float16_t::kBitsMantissa);

		if (expDiff < ((sizeof(long) << 3) - (float16_t::kBitsMantissa + 1))) {
			m1 <<= expDiff;
			temp = two.IEEE.Exp;
		}
		else {
			if (0 != two.IEEE.Exp) {
				// arithmetic underflow
				if (expDiff > float16_t::kBitsMantissa)return float16_t(0, 0, 0);
				else m2 >>= expDiff;
			}
			temp = one.IEEE.Exp;
		}
	}

	// convert from sign-bit to two's complement representation
	if (one.IEEE.Sign)m1 = -m1;
	if (two.IEEE.Sign)m2 = -m2;
	m1 += m2;
	if (m1 < 0) {
		out.IEEE.Sign = 1;
		m1 = -m1;
	}
	else out.IEEE.Sign = 0;

	// and renormalize the result to fit in a half
	if (0 == m1)return float16_t(0, 0, 0);

#ifdef _MSC_VER
	_BitScanReverse((unsigned long*)&m2, m1);
#else
	m2 = __builtin_clz(m1);
#endif
	expDiff = m2 - float16_t::kBitsMantissa;
	temp += expDiff;
	if (expDiff >= float16_t::kMaxExponentValue) {
		// arithmetic overflow. return INF and keep the sign
		return float16_t(0, float16_t::kMaxExponentValue, out.IEEE.Sign);
	}
	else if (temp <= 0) {
		// this maps to a denorm
		m1 <<= (-expDiff - 1);
		temp = 0;
	}
	else {
		// rebuild the normalized representation, take care of the hidden bit
		if (expDiff < 0)m1 <<= (-expDiff);
		else m1 >>= expDiff; // m1 >= 0
	}
	out.IEEE.Frac = m1;
	out.IEEE.Exp = temp;
	return out;

#else
	return HalfFloat((float)one + (float)two);
#endif
}

inline float16_t operator-(float16_t one, float16_t two) {
	return float16_t(one + (-two));
}

inline float16_t operator*(float16_t one, float16_t two) {
	return float16_t((float)one * (float)two);
}

inline float16_t operator/(float16_t one, float16_t two) {
	return float16_t((float)one / (float)two);
}

inline float operator+(float16_t one, float two) {
	return (float)one + two;
}

inline float operator-(float16_t one, float two) {
	return (float)one - two;
}

inline float operator*(float16_t one, float two) {
	return (float)one * two;
}

inline float operator/(float16_t one, float two) {
	return (float)one / two;
}

inline float operator+(float one, float16_t two) {
	return two + one;
}

inline float operator-(float one, float16_t two) {
	return two - one;
}

inline float operator*(float one, float16_t two) {
	return two * one;
}

inline float operator/(float one, float16_t two) {
	return two / one;
}

#pragma endregion

#pragma endregion

using i8	= int8_t;
using i16	= int16_t;
using i32	= int32_t;
using i64	= int64_t;

using u8	= uint8_t;
using u16	= uint16_t;
using u32	= uint32_t;
using u64	= uint64_t;

using iptr	= intptr_t;
using uptr	= uintptr_t;

using byte = unsigned char;

using f16	= float16_t;
using f32	= float;
using f64	= double;

using uchar		= unsigned char;
using ushort	= unsigned short;
using ll		= long long;
using ull		= unsigned long long;
using usize		= size_t;

inline constexpr u16 kMaxU16 = std::numeric_limits<u16>::max();
inline constexpr u32 kMaxU32 = std::numeric_limits<u32>::max();
inline constexpr u64 kMaxU64 = std::numeric_limits<u64>::max();
inline constexpr ushort kMaxUshort = std::numeric_limits<ushort>::max();
inline constexpr ull kMaxUll = std::numeric_limits<ull>::max();
inline constexpr usize kMaxUsize = std::numeric_limits<usize>::max();

inline constexpr i32 kMaxI32 = std::numeric_limits<i32>::max();
inline constexpr i32 kMinI32 = std::numeric_limits<i32>::min();

inline constexpr i64 kMaxI64 = std::numeric_limits<i64>::max();
inline constexpr i64 kMinI64 = std::numeric_limits<i64>::min();

inline constexpr f32 kMaxF32 = std::numeric_limits<f32>::max();
inline constexpr f32 kMinF32 = std::numeric_limits<f32>::min();

inline constexpr f64 kMaxF64 = std::numeric_limits<f64>::max();
inline constexpr f64 kMinF64 = std::numeric_limits<f64>::min();

inline constexpr ll kMaxLl = std::numeric_limits<ll>::max();
inline constexpr ll kMinLl = std::numeric_limits<ll>::min();

inline constexpr f64 kPi = 3.141592653589793238462643383279;
inline constexpr f64 kEpsilon = 1e-6;

inline constexpr f32 kU8toF32 = 1.f / 255.f;

#endif