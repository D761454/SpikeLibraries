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

#ifndef __SPIKE_TIMER_H
#define __SPIKE_TIMER_H

#include <cstdint>
#include <chrono>

/*
----------------------------------------------------------------------

File Summary:
	This file holds the timer class. Useful for timing code portions for
	profiling.

----------------------------------------------------------------------
*/

namespace stime {
	/// <summary>
	/// Simple timer class for getting the time between start and stop calls
	/// </summary>
	class timer {
	private:
		using clock = std::chrono::steady_clock;
		using timePoint = std::chrono::time_point<clock>;
		static inline timePoint start_t;

	public:
		enum time_format : uint8_t { NANOSECONDS, MICROSECONDS, MILLISECONDS, SECONDS };

		/// <summary>
		/// Log the current time within the timer class as a start time
		/// </summary>
		static inline void Start() noexcept {
			start_t = clock::now();
		}

		/// <summary>
		/// Stop the timer (logs current time), and calculates the time passed since the last Start call
		/// </summary>
		/// <typeparam name="TF">time format to convert the result to</typeparam>
		/// <returns>time since last start call in TF notation</returns>
		template<time_format TF>
		static uint64_t Stop() noexcept {
			const auto end_t = clock::now();
			const auto elapsed = end_t - start_t;
			return std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
		}

		/// <summary>
		/// Stop the timer (logs current time), and calculates the time passed since the last Start call
		/// </summary>
		/// <typeparam name="TF">time format to convert the result to</typeparam>
		/// <returns>time since last start call in TF notation</returns>
		template<time_format TF>
		static double Stopf() noexcept {
			const auto end_t = clock::now();
			const auto elapsed = end_t - start_t;
			return static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count());
		}
	};

	inline constexpr timer::time_format nanoseconds		= timer::time_format::NANOSECONDS;
	inline constexpr timer::time_format microseconds		= timer::time_format::MICROSECONDS;
	inline constexpr timer::time_format milliseconds		= timer::time_format::MILLISECONDS;
	inline constexpr timer::time_format seconds			= timer::time_format::SECONDS;
}

#pragma region Implementation

template<>
uint64_t stime::timer::Stop<stime::nanoseconds>() {
	const auto end_t = clock::now();
	const auto elapsed = end_t - start_t;
	return std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
}

template<>
uint64_t stime::timer::Stop<stime::microseconds>() {
	const auto end_t = clock::now();
	const auto elapsed = end_t - start_t;
	return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
}

template<>
uint64_t stime::timer::Stop<stime::milliseconds>() {
	const auto end_t = clock::now();
	const auto elapsed = end_t - start_t;
	return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
}

template<>
uint64_t stime::timer::Stop<stime::seconds>() {
	const auto end_t = clock::now();
	const auto elapsed = end_t - start_t;
	return std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
}

template<>
double stime::timer::Stopf<stime::nanoseconds>() {
	const auto end_t = clock::now();
	const auto elapsed = end_t - start_t;
	return static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count());
}

template<>
double stime::timer::Stopf<stime::microseconds>() {
	const auto end_t = clock::now();
	const auto elapsed = end_t - start_t;
	const uint64_t ns = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
	return ns * 0.001;
}

template<>
double stime::timer::Stopf<stime::milliseconds>() {
	const auto end_t = clock::now();
	const auto elapsed = end_t - start_t;
	const uint64_t ns = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
	return ns * (1.0E-6);
}

template<>
double stime::timer::Stopf<stime::seconds>() {
	const auto end_t = clock::now();
	const auto elapsed = end_t - start_t;
	const uint64_t ns = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
	return ns * (1.0E-9);
}

#pragma endregion


#endif