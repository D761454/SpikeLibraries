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

#ifndef __SPIKE_SML_CORE_H
#define __SPIKE_SML_CORE_H

/*
----------------------------------------------------------------------
File Summary:
	Core maths utilities providing abstractions for commonly used maths
	functions, alongside additional functions not provided by native C++
	standard library. Includes trigonometric, logarithmic, rounding,
	and interpolation functions with template support for multiple types.

----------------------------------------------------------------------
*/

#include <spike/def_aliases.h>

#define SQUARE(x) ((x) * (x))

namespace sml {
	/// <summary>
	/// Degrees-to-radians conversion constant.
	/// Multiply a degree value by this constant to convert to radians.
	/// </summary>
	inline constexpr f64 Deg2Rad = (kPi * 2) / 360;

	/// <summary>
	/// Radians-to-degrees conversion constant.
	/// Multiply a radian value by this constant to convert to degrees.
	/// </summary>
	inline constexpr f64 Rad2Deg = 360 / (kPi * 2);

	/// <summary>
	/// Uint8 to float conversion constant.
	/// Used to convert uint8_t values (0-255) to float (0.0-1.0),
	/// primarily for colour conversion in graphics applications.
	/// </summary>
	inline constexpr f32 k255to1 = 1.0f / 255.0f;

	/// <summary>
	/// Returns the absolute value of value.
	/// </summary>
	/// <param name="value">- the input value</param>
	/// <returns>absolute value of input</returns>
	template<typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
	inline T abs(T value) noexcept {
		return static_cast<T>(std::abs(value));
	}

	/// <summary>
	/// Compares two values and returns true if they are approximately equal.
	/// Uses a tolerance of 0.0001 for comparison.
	/// </summary>
	/// <param name="a">- first value to compare</param>
	/// <param name="b">- second value to compare</param>
	/// <returns>true if values are approximately equal, false otherwise</returns>
	template<typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
	inline bool approx(T a, T b) noexcept {
		return abs(a - b) < 0.0001;
	}

	/// <summary>
	/// Returns Euler's number (e) raised to the specified power.
	/// </summary>
	/// <param name="value">- the exponent</param>
	/// <returns>e raised to the power of value</returns>
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	inline T exp(T value) noexcept {
		return static_cast<T>(std::exp(value));
	}

	/// <summary>
	/// Returns the smallest integer value greater than or equal to value.
	/// </summary>
	/// <param name="value">- the input value</param>
	/// <returns>ceiling of value in original type</returns>
	template<typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
	inline T ceil(T value) noexcept {
		return static_cast<T>(std::ceil(value));
	}

	/// <summary>
	/// Returns the smallest integer value greater than or equal to value, cast as int32.
	/// </summary>
	/// <param name="value">- the input floating point value</param>
	/// <returns>ceiling of value as int32</returns>
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	inline i32 ceil_to_int32(T value) noexcept {
		return static_cast<i32>(std::ceil(value));
	}

	/// <summary>
	/// Returns the smallest integer value greater than or equal to value, cast as int64.
	/// </summary>
	/// <param name="value">- the input floating point value</param>
	/// <returns>ceiling of value as int64</returns>
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	inline i64 ceil_to_int64(T value) noexcept {
		return static_cast<i64>(std::ceil(value));
	}

	/// <summary>
	/// Returns the smallest integer value greater than or equal to value, cast as native integer (int64 on 64-bit systems, int32 on 32-bit systems).
	/// </summary>
	/// <param name="value">- the input floating point value</param>
	/// <returns>ceiling of value as native integer</returns>
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	inline i64 ceil_to_int(T value) noexcept {
		return static_cast<i64>(std::ceil(value));
	}

	/// <summary>
	/// Returns the greatest integer value smaller than or equal to value.
	/// </summary>
	/// <param name="value">- the input value</param>
	/// <returns>floor of value in original type</returns>
	template<typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
	inline T floor(T value) noexcept {
		return static_cast<T>(std::floor(value));
	}

	/// <summary>
	/// Returns the greatest integer value smaller than or equal to value, cast as int32.
	/// </summary>
	/// <param name="value">- the input floating point value</param>
	/// <returns>floor of value as int32</returns>
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	inline i32 floor_to_int32(T value) noexcept {
		return static_cast<i32>(std::floor(value));
	}

	/// <summary>
	/// Returns the greatest integer value smaller than or equal to value, cast as int64.
	/// </summary>
	/// <param name="value">- the input floating point value</param>
	/// <returns>floor of value as int64</returns>
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	inline i64 floor_to_int64(T value) noexcept {
		return static_cast<i64>(std::floor(value));
	}

	/// <summary>
	/// Returns the greatest integer value smaller than or equal to value, cast as native integer (int64 on 64-bit systems, int32 on 32-bit systems).
	/// </summary>
	/// <param name="value">- the input floating point value</param>
	/// <returns>floor of value as native integer</returns>
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	inline i64 floor_to_int(T value) noexcept {
		return static_cast<i64>(std::floor(value));
	}

	/// <summary>
	/// Returns the sine of the input angle value in radians.
	/// </summary>
	/// <param name="value">- angle in radians</param>
	/// <returns>sine of the angle</returns>
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	inline T sin(T value) noexcept {
		return static_cast<T>(std::sin(value));
	}

	/// <summary>
	/// Returns the cosine of the input angle value in radians.
	/// </summary>
	/// <param name="value">- angle in radians</param>
	/// <returns>cosine of the angle</returns>
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	inline T cos(T value) noexcept {
		return static_cast<T>(std::cos(value));
	}

	/// <summary>
	/// Returns the tangent of the input angle value in radians.
	/// </summary>
	/// <param name="value">- angle in radians</param>
	/// <returns>tangent of the angle</returns>
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	inline T tan(T value) noexcept {
		return static_cast<T>(std::tan(value));
	}

	/// <summary>
	/// Returns the arc-sine (inverse sine) of value.
	/// Result is in radians in the range [-π/2, π/2].
	/// </summary>
	/// <param name="value">- input value in range [-1, 1]</param>
	/// <returns>arc-sine of value in radians</returns>
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	inline T asin(T value) noexcept {
		return static_cast<T>(std::asin(value));
	}

	/// <summary>
	/// Returns the arc-cosine (inverse cosine) of value.
	/// Result is in radians in the range [0, π].
	/// </summary>
	/// <param name="value">- input value in range [-1, 1]</param>
	/// <returns>arc-cosine of value in radians</returns>
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	inline T acos(T value) noexcept {
		return static_cast<T>(std::acos(value));
	}

	/// <summary>
	/// Returns the arc-tangent (inverse tangent) of value.
	/// Result is in radians in the range [-π/2, π/2].
	/// </summary>
	/// <param name="value">- input value</param>
	/// <returns>arc-tangent of value in radians</returns>
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	inline T atan(T value) noexcept {
		return static_cast<T>(std::atan(value));
	}

	/// <summary>
	/// Returns the angle in radians whose tangent is (a/b).
	/// Uses both arguments to determine the correct quadrant of the result.
	/// </summary>
	/// <param name="a">- numerator (y-coordinate)</param>
	/// <param name="b">- denominator (x-coordinate)</param>
	/// <returns>angle in radians in the range [-π, π]</returns>
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	inline T atan2(T a, T b) noexcept {
		return static_cast<T>(std::atan2(a, b));
	}

	/// <summary>
	/// Returns the mathematical sign of value.
	/// </summary>
	/// <param name="value">- the input value</param>
	/// <returns>-1 if value is negative, 1 if positive, 0 if zero</returns>
	template<typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
	inline T sign(T value) noexcept {
		return static_cast<T>(value < 0.0 ? -1.0 : (value >= 0.0 ? 1.0 : 0.0));
	}

	/// <summary>
	/// Clamps value between min and max bounds.
	/// Returns value if it is within the range, otherwise returns the nearest bound.
	/// </summary>
	/// <param name="value">- the value to clamp</param>
	/// <param name="min">- minimum bound</param>
	/// <param name="max">- maximum bound</param>
	/// <returns>clamped value within [min, max] range</returns>
	template<typename T, typename T2, typename T3, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr, std::enable_if_t<std::is_arithmetic_v<T2>>* = nullptr, std::enable_if_t<std::is_arithmetic_v<T3>>* = nullptr>
	inline T clamp(T value, T2 min, T3 max) noexcept {
#pragma warning(suppress: 4018)
		return value < min ? min : (value > max ? max : value);
	}

	/// <summary>
	/// Clamps value between 0 and 1 (normalized range).
	/// </summary>
	/// <param name="value">- the value to clamp</param>
	/// <returns>value clamped within [0, 1] range</returns>
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	inline T clamp01(T value) noexcept {
		return static_cast<T>(value < 0 ? 0 : (value > 1 ? 1 : value));
	}

	/// <summary>
	/// Linearly interpolates between a and b by factor t.
	/// When t=0, returns a. When t=1, returns b. When t=0.5, returns midpoint.
	/// </summary>
	/// <param name="a">- start value</param>
	/// <param name="b">- end value</param>
	/// <param name="t">- interpolation factor (typically in range [0, 1])</param>
	/// <returns>interpolated value between a and b</returns>
	template<typename T, typename T2, typename T3, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr, std::enable_if_t<std::is_floating_point_v<T2>>* = nullptr, std::enable_if_t<std::is_floating_point_v<T3>>* = nullptr>
	inline T lerp(T a, T2 b, T3 t) noexcept {
		return static_cast<T>(std::lerp(a, b, t));
	}

	/// <summary>
	/// Returns the logarithm of value in the specified base.
	/// </summary>
	/// <param name="value">- the input value (must be positive)</param>
	/// <param name="base">- the logarithm base (must be positive and not equal to 1)</param>
	/// <returns>logarithm of value in the specified base</returns>
	template<typename T, typename T2, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr, std::enable_if_t<std::is_arithmetic_v<T2>>* = nullptr>
	inline T log(T value, T2 base) noexcept {
		return static_cast<T>(std::log(value)) / static_cast<T>(std::log(base));
	}

	/// <summary>
	/// Returns the base-10 logarithm of value.
	/// </summary>
	/// <param name="value">- the input value (must be positive)</param>
	/// <returns>base-10 logarithm of value</returns>
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	inline T log10(T value) noexcept {
		return static_cast<T>(std::log10(value));
	}

	/// <summary>
	/// Returns value raised to the power p.
	/// </summary>
	/// <param name="value">- the base value</param>
	/// <param name="p">- the exponent</param>
	/// <returns>value raised to power p</returns>
	template<typename T, typename T2, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr, std::enable_if_t<std::is_arithmetic_v<T2>>* = nullptr>
	inline T pow(T value, T2 p) noexcept {
		return static_cast<T>(std::pow(value, p));
	}

	/// <summary>
	/// Returns the square root of value.
	/// </summary>
	/// <param name="value">- the input value (must be non-negative)</param>
	/// <returns>square root of value</returns>
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	inline T sqrt(T value) noexcept {
		return static_cast<T>(std::sqrt(value));
	}

	/// <summary>
	/// Returns the largest value among all provided numeric arguments.
	/// </summary>
	/// <param name="args">- variable number of arguments to compare</param>
	/// <returns>largest value among arguments</returns>
	template<typename... T>
	std::common_type_t<T...> max(T... args) {
		using CT = std::common_type_t<T...>;
		CT result = {};
		result = std::max({ static_cast<std::common_type_t<T...>>(args)... });
		return result;
	}

	/// <summary>
	/// Returns the smallest value among all provided numeric arguments.
	/// </summary>
	/// <param name="args">- variable number of arguments to compare</param>
	/// <returns>smallest value among arguments</returns>
	template<typename... T>
	std::common_type_t<T...> min(T... args) {
		using CT = std::common_type_t<T...>;
		CT result = {};
		result = std::min({ static_cast<std::common_type_t<T...>>(args)... });
		return result;
	}

	/// <summary>
	/// Returns value rounded to the nearest integer value.
	/// </summary>
	/// <param name="value">- the input value</param>
	/// <returns>value rounded to nearest integer in original type</returns>
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	inline T round(T value) noexcept {
		return static_cast<T>(std::round(value));
	}

	/// <summary>
	/// Returns value rounded to the nearest integer value, cast as int32.
	/// </summary>
	/// <param name="value">- the input floating point value</param>
	/// <returns>value rounded to nearest integer as int32</returns>
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	inline i32 round_to_int32(T value) noexcept {
		return static_cast<i32>(std::round(value));
	}

	/// <summary>
	/// Returns value rounded to the nearest integer value, cast as int64.
	/// </summary>
	/// <param name="value">- the input floating point value</param>
	/// <returns>value rounded to nearest integer as int64</returns>
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	inline i64 round_to_int64(T value) noexcept {
		return static_cast<i64>(std::round(value));
	}

	/// <summary>
	/// Returns value rounded to the nearest integer value, cast as native integer (int64 on 64-bit systems, int32 on 32-bit systems).
	/// </summary>
	/// <param name="value">- the input floating point value</param>
	/// <returns>value rounded to nearest integer as native integer</returns>
	template<typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	inline i64 round_to_int(T value) noexcept {
		return static_cast<i64>(std::round(value));
	}
}

#endif