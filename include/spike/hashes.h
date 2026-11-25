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