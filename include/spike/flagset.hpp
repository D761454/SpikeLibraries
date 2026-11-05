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

#ifndef __FLAGSET_HPP
#define __FLAGSET_HPP

#include <cstdint>
#include <limits>
#include <type_traits>

/*
----------------------------------------------------------------------

File Summary:
	This file holds the flag_set data type. The flag_set is used as a
	wrapper around a value of data type T, whereby T is split into SPLITS,
	where each split may be used as an individual mask / value.

----------------------------------------------------------------------
*/

// Compile-time validation for integer types
template<typename T>
concept IntegralType = std::is_integral_v<T> && !std::is_same_v<T, bool>;

// Compile-time validation for split count
template<typename T, uint8_t N>
concept ValidSplitCount = (N > 0 && N <= (std::numeric_limits<T>::digits));

/// <summary>
/// Type-safe flag wrapper for packing multiple data pieces into a single integral
/// </summary>
/// <typeparam name="T">- Data type stored by the flagset, this is also the size of the flag</typeparam>
/// <typeparam name="SPLITS">- how many flags to split T into</typeparam>
template<IntegralType T, uint8_t SPLITS>
	requires ValidSplitCount<T, SPLITS>
struct flag_set {
private:
	static constexpr uint8_t BIT_COUNT = std::numeric_limits<T>::digits;
	static constexpr uint8_t CHUNK_SIZE = BIT_COUNT / SPLITS;
	static constexpr T CHUNK_MASK = (T(1) << CHUNK_SIZE) - 1;

	T _data;

public:
	flag_set() noexcept : _data(0) {}
	explicit flag_set(T initialValue) noexcept : _data(initialValue) {}

	/// <summary>
	/// Get the entire flag value held by the flag_set
	/// </summary>
	/// <returns>value held by the flag_set</returns>
	inline T GetRaw() const noexcept { return _data; }

	/// <summary>
	/// Sets the entire flag value held by the flag_set
	/// </summary>
	/// <param name="value">- value to set the entire flag_set to</param>
	inline void SetRaw(T value) noexcept { _data = value; }

	/// <summary>
	/// Set a flag chunk by index
	/// </summary>
	/// <param name="index">- chunk index (0 to SPLITS-1)</param>
	/// <param name="value">- value to set in this chunk</param>
	inline void Set(uint8_t index, T value) noexcept {
		if constexpr (SPLITS > 1) {
			// Only validate in debug builds for performance
#ifdef _DEBUG
			if (index >= SPLITS) return;
#endif
		}
		const T shift = index * CHUNK_SIZE;
		_data = (_data & ~(CHUNK_MASK << shift)) | ((value & CHUNK_MASK) << shift);
	}

	/// <summary>
	/// Get a flag chunk by index, optionally cast to smaller type
	/// </summary>
	/// <param name="index">- chunk index (0 to SPLITS-1)</param>
	/// <returns>value extracted from this chunk</returns>
	inline T Get(uint8_t index) const noexcept {
		if constexpr (SPLITS > 1) {
#ifdef _DEBUG
			if (index >= SPLITS) return 0;
#endif
		}
		const T shift = index * CHUNK_SIZE;
		return (_data >> shift) & CHUNK_MASK;
	}

	/// <summary>
	/// Get a flag chunk and cast to a specific type. Usage: uint8_t val = flags.Get<uint8_t>(0);
	/// </summary>
	/// <typeparam name="U">- type to cast the result to</typeparam>
	/// <param name="index">- chunk index (0 to SPLITS-1)</param>
	/// <returns>value extracted from this chunk, cast to U</returns>
	template<typename U>
		requires std::is_integral_v<U>
	inline U Get(uint8_t index) const noexcept {
		return static_cast<U>(Get(index));
	}

	/// <summary>
	/// Toggle a single bit flag
	/// </summary>
	/// <param name="bitIndex">- bit index (0 to BIT_COUNT-1)</param>
	inline void ToggleBit(uint8_t bitIndex) noexcept {
		if constexpr (BIT_COUNT > 1) {
#ifdef _DEBUG
			if (bitIndex >= BIT_COUNT) return;
#endif
		}
		_data ^= (T(1) << bitIndex);
	}

	/// <summary>
	/// Set a single bit flag
	/// </summary>
	/// <param name="bitIndex">- bit index (0 to BIT_COUNT-1)</param>
	inline void SetBit(uint8_t bitIndex) noexcept {
		if constexpr (BIT_COUNT > 1) {
#ifdef _DEBUG
			if (bitIndex >= BIT_COUNT) return;
#endif
		}
		_data |= (T(1) << bitIndex);
	}

	/// <summary>
	/// Clear a single bit flag
	/// </summary>
	/// <param name="bitIndex">- bit index (0 to BIT_COUNT-1)</param>
	inline void ClearBit(uint8_t bitIndex) noexcept {
		if constexpr (BIT_COUNT > 1) {
#ifdef _DEBUG
			if (bitIndex >= BIT_COUNT) return;
#endif
		}
		_data &= ~(T(1) << bitIndex);
	}

	/// <summary>
	/// Test if a single bit is set
	/// </summary>
	/// <param name="bitIndex">- bit index (0 to BIT_COUNT-1)</param>
	/// <returns>if the bit is set</returns>
	inline bool TestBit(uint8_t bitIndex) const noexcept {
		if constexpr (BIT_COUNT > 1) {
#ifdef _DEBUG
			if (bitIndex >= BIT_COUNT) return false;
#endif
		}
		return (_data & (T(1) << bitIndex)) != 0;
	}

	inline flag_set operator|(const flag_set& other) const noexcept {
		return FlagSet(_data | other._data);
	}

	inline flag_set operator&(const flag_set& other) const noexcept {
		return FlagSet(_data & other._data);
	}

	inline flag_set operator^(const flag_set& other) const noexcept {
		return FlagSet(_data ^ other._data);
	}

	inline flag_set operator~() const noexcept {
		return FlagSet(~_data);
	}

	inline flag_set& operator|=(const flag_set& other) noexcept {
		_data |= other._data;
		return *this;
	}

	inline flag_set& operator&=(const flag_set& other) noexcept {
		_data &= other._data;
		return *this;
	}

	inline flag_set& operator^=(const flag_set& other) noexcept {
		_data ^= other._data;
		return *this;
	}

	inline bool operator==(const flag_set& other) const noexcept {
		return _data == other._data;
	}

	inline bool operator!=(const flag_set& other) const noexcept {
		return _data != other._data;
	}

	explicit operator T() const noexcept { return _data; }

	static constexpr uint8_t GetChunkCount() noexcept { return SPLITS; }
	static constexpr uint8_t GetChunkSize() noexcept { return CHUNK_SIZE; }
	static constexpr uint8_t GetBitCount() noexcept { return BIT_COUNT; }
	static constexpr T GetChunkMask() noexcept { return CHUNK_MASK; }
};

template<uint8_t N> requires (N > 0 && N <= 8)
using flag_set8_t = flag_set<uint8_t, N>;

template<uint8_t N> requires (N > 0 && N <= 16)
using flag_set16_t = flag_set<uint16_t, N>;

template<uint8_t N> requires (N > 0 && N <= 32)
using flag_set32_t = flag_set<uint32_t, N>;

template<uint8_t N> requires (N > 0 && N <= 64)
using flag_set64_t = flag_set<uint64_t, N>;

#endif