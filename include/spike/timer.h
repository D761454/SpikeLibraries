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

#ifndef __TIMER_H
#define __TIMER_H

#include <cstdint>
#include <chrono>

/*
----------------------------------------------------------------------

File Summary:
	This file holds the timer class. Useful for timing code portions for
	profiling.

----------------------------------------------------------------------
*/

namespace tmr {
	/// <summary>
	/// Simple timer class for getting the time between start and stop calls
	/// </summary>
	class timer {
	private:
		using timePoint = std::chrono::time_point<std::chrono::steady_clock>;
		static inline timePoint start_t;

	public:
		enum time_format : uint8_t { NANOSECONDS, MICROSECONDS, MILLISECONDS, SECONDS };

		/// <summary>
		/// Log the current time within the timer class as a start time
		/// </summary>
		static void Start() noexcept {
			start_t = std::chrono::steady_clock::now();
		}

		/// <summary>
		/// Stop the timer (logs current time), and calculates the time passed since the last Start call
		/// </summary>
		/// <typeparam name="TF">time format to convert the result to</typeparam>
		/// <returns>time since last start call in TF notation</returns>
		template<time_format TF = time_format::MILLISECONDS>
		static uint64_t Stop() noexcept {
			const auto end_t = std::chrono::steady_clock::now();

			const auto elapsed = end_t - start_t;
			const uint64_t ns = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();

			switch (TF) {
			case time_format::NANOSECONDS: {
				return ns;
			}
			case time_format::MICROSECONDS: {
				uint64_t u = ((ns % 1000) >= 500) ? 1 : 0;
				const auto mcs = (ns / 1000) + u;
				return mcs;
			}
			case time_format::MILLISECONDS: {
				uint64_t u = ((ns % 1000000) >= 500000) ? 1 : 0;
				const auto ms = (ns / 1000000) + u;
				return ms;
			}
			case time_format::SECONDS: {
				uint64_t u = ((ns % 1000000000) >= 500000000) ? 1 : 0;
				const auto s = (ns / 1000000000) + u;
				return s;
			}
			default:
				return ns;
			}
		}

		/// <summary>
		/// Stop the timer (logs current time), and calculates the time passed since the last Start call
		/// </summary>
		/// <typeparam name="TF">time format to convert the result to</typeparam>
		/// <returns>time since last start call in TF notation</returns>
		template<time_format TF = time_format::MILLISECONDS>
		static double Stopf() noexcept {
			const auto end_t = std::chrono::steady_clock::now();

			const auto elapsed = end_t - start_t;
			const uint64_t ns = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();

			switch (TF) {
			case time_format::NANOSECONDS: {
				return static_cast<double>(ns);
			}
			case time_format::MICROSECONDS: {
				int u = ((ns % 1000) >= 500) ? 1 : 0;
				return (ns / 1000.0) + u;
			}
			case time_format::MILLISECONDS: {
				int u = ((ns % 1000000) >= 500000) ? 1 : 0;
				return (ns / 1000000.0) + u;
			}
			case time_format::SECONDS: {
				int u = ((ns % 1000000000) >= 500000000) ? 1 : 0;
				return (ns / 1000000000.0) + u;
			}
			default:
				return static_cast<double>(ns);
			}
		}
	};

	constexpr timer::time_format nanoseconds		= timer::time_format::NANOSECONDS;
	constexpr timer::time_format microseconds		= timer::time_format::MICROSECONDS;
	constexpr timer::time_format milliseconds		= timer::time_format::MILLISECONDS;
	constexpr timer::time_format seconds			= timer::time_format::SECONDS;
}

#endif