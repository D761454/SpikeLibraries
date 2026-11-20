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

#ifndef __SPIKE_TOGGLE_SET_H
#define __SPIKE_TOGGLE_SET_H

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <vector>

/*
----------------------------------------------------------------------

File Summary:
	This file holds the toggle set data type, storing an array of 
	bools keyed by a name value. On changing one value, all others are 
	toggled to false.

----------------------------------------------------------------------
*/

struct toggle_set {
private:
	const std::vector<std::string_view>* _strArr;
	std::uint64_t _data;

public:
	static constexpr std::size_t capacity = 64;

	toggle_set() noexcept
		: _strArr(nullptr), _data(0) {
	}

	explicit toggle_set(const std::vector<std::string_view>& strArr) : _strArr(&strArr), _data(0) {
		if (strArr.size() > capacity) {
			throw std::length_error("A toggle_set supports at most 64 flags");
		}
	}

	bool set(std::uint32_t index) noexcept {
		if (index >= capacity || (_strArr != nullptr && index >= _strArr->size())) {
			return false;
		}

		_data = std::uint64_t{1} << index;
		return true;
	}

	bool set(std::string_view key) noexcept {
		if (_strArr == nullptr) {
			return false;
		}

		for (std::size_t i = 0; i < _strArr->size() && i < capacity; ++i) {
			if ((*_strArr)[i] == key) {
				_data = std::uint64_t{1} << i;
				return true;
			}
		}

		return false;
	}

	[[nodiscard]] bool get(std::uint32_t index) const noexcept {
		if (index >= capacity || (_strArr != nullptr && index >= _strArr->size())) {
			return false;
		}

		return (_data & (std::uint64_t{1} << index)) != 0;
	}

	[[nodiscard]] bool get(std::string_view key) const noexcept {
		if (_strArr == nullptr) {
			return false;
		}

		for (std::size_t i = 0; i < _strArr->size() && i < capacity; ++i) {
			if ((*_strArr)[i] == key) {
				return (_data & (std::uint64_t{1} << i)) != 0;
			}
		}

		return false;
	}

	void reset() noexcept {
		_data = 0;
	}

	[[nodiscard]] std::size_t size() const noexcept {
		return _strArr == nullptr ? 0 : _strArr->size();
	}
};

#endif