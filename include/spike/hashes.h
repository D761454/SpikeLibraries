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

#ifndef __SPIKE_HASHES_H
#define __SPIKE_HASHES_H

#include <cstdint>

/*
----------------------------------------------------------------------

File Summary:
	This file holds hash methods for use during runtime and compile 
	time.

----------------------------------------------------------------------
*/

namespace hash {
	inline constexpr uint32_t k32fnvOffset = 2166136261U;
	inline constexpr uint32_t k32fnvPrime = 16777619U;

	inline constexpr uint64_t k64fnvOffset = 14695981039346656037ULL;
	inline constexpr uint64_t k64fnvPrime = 1099511628211ULL;

	/// <summary>
	/// Generates a hash from a buffer of size len
	/// </summary>
	/// <param name="data">pointer to the buffer to hash</param>
	/// <param name="len">length of the buffer</param>
	/// <returns>hash value corresponding to the buffer</returns>
	[[nodiscard]] uint32_t fnv1a32(const char* data, size_t len) noexcept {
		uint32_t hash = k32fnvOffset;
		const uint8_t* ptr = reinterpret_cast<const uint8_t*>(data);

		for (size_t i = 0; i < len; i++) {
			hash ^= static_cast<uint32_t>(ptr[i]);
			hash *= k32fnvPrime;
		}

		return hash;
	}

	/// <summary>
	/// Generates a hash from a null-terminated string at compile time
	/// </summary>
	/// <param name="data">string to hash</param>
	/// <param name="value">offset value</param>
	/// <returns>hash value corresponding to the string</returns>
	[[nodiscard]] constexpr uint32_t fnv1a32_C(const char* data, uint32_t value = k32fnvOffset) noexcept {
		return (!data || *data == '\0') ? value : fnv1a32_C(data + 1, (value ^ static_cast<uint8_t>(*data)) * k32fnvPrime);
	}

	/// <summary>
	/// Generates a hash from a buffer of size len
	/// </summary>
	/// <param name="data">pointer to the buffer to hash</param>
	/// <param name="len">length of the buffer</param>
	/// <returns>hash value corresponding to the buffer</returns>
	[[nodiscard]] uint64_t fnv1a64(const char* data, size_t len) noexcept {
		uint64_t hash = k64fnvOffset;
		const uint8_t* ptr = reinterpret_cast<const uint8_t*>(data);
		for (size_t i = 0; i < len; i++) {
			hash ^= static_cast<uint64_t>(ptr[i]);
			hash *= k64fnvPrime;
		}
		return hash;
	}

	/// <summary>
	/// Generates a hash from a null-terminated string at compile time
	/// </summary>
	/// <param name="data">string to hash</param>
	/// <param name="value">offset value</param>
	/// <returns>hash value corresponding to the string</returns>
	[[nodiscard]] constexpr uint64_t fnv1a64_C(const char* data, uint64_t value = k64fnvOffset) noexcept {
		return (!data || *data == '\0') ? value : fnv1a64_C(data + 1, (value ^ static_cast<uint8_t>(*data)) * k64fnvPrime);
	}
}

#endif