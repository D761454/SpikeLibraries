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

#ifndef __SPIKE_ASSET_FILE_PARSER_STREAM
#define __SPIKE_ASSET_FILE_PARSER_STREAM

#include <cstdint>
#include <limits>
#include <vector>
#include <algorithm>
#include <fstream>
#include <istream>
#include <ostream>
#include <string>
#include <type_traits>

namespace fileacc {
	// Compile-time validation for integer types
	template<typename T>
	concept IntegralType = std::is_integral_v<T> && !std::is_same_v<T, bool>;

	template<typename T>
	inline void write(std::ostream& ofs, const T& obj) {
		ofs.write(reinterpret_cast<const char*>(&obj), static_cast<std::streamsize>(sizeof(obj)));
	}

	template<typename T, IntegralType U>
	inline void write(std::ostream& ofs, const T& obj, U size) {
		ofs.write(reinterpret_cast<const char*>(&obj), static_cast<std::streamsize>(size));
	}

	template<typename T, IntegralType U>
	inline void write(std::ostream& ofs, const T* obj, U size) {
		ofs.write(reinterpret_cast<const char*>(obj), static_cast<std::streamsize>(size));
	}

	template<typename T>
	inline void read(std::istream& ifs, T& obj) {
		ifs.read(reinterpret_cast<char*>(&obj), static_cast<std::streamsize>(sizeof(obj)));
	}

	template<typename T, IntegralType U>
	inline void read(std::istream& ifs, T& obj, U size) {
		ifs.read(reinterpret_cast<char*>(&obj), static_cast<std::streamsize>(size));
	}

	template<typename T, IntegralType U>
	inline void read(std::istream& ifs, T* obj, U size) {
		ifs.read(reinterpret_cast<char*>(obj), static_cast<std::streamsize>(size));
	}

	inline void write_string(std::string& str, std::ofstream& ofs) {
		uint16_t len = std::min(static_cast<uint16_t>(str.size()), std::numeric_limits<uint16_t>::max());
		ofs.write(reinterpret_cast<const char*>(&len), sizeof(len));
		ofs.write(str.data(), len);
	}

	inline void parse_string(std::string& str, std::istream& ifs) {
		uint16_t len = 0;
		ifs.read(reinterpret_cast<char*>(&len), sizeof(len));
		str.resize(len);
		ifs.read(str.data(), len);
	}

	inline uint64_t align_to_boundary(uint64_t value, uint64_t boundary) noexcept {
		return (value + boundary - 1) & ~(boundary - 1);
	}

	inline uint64_t padding_needed(uint64_t currentPosition, uint64_t boundary) noexcept {
		uint64_t aligned = align_to_boundary(currentPosition, boundary);
		return static_cast<uint64_t>(aligned - currentPosition);
	}

	inline void pad_to_boundary(std::ostream& ofs, uint64_t alignment = 64) {
		uint64_t currentPos = ofs.tellp();
		uint64_t namePadding = padding_needed(currentPos, alignment);
		if (namePadding > 0) {
			std::vector<uint8_t> pad(namePadding, 0);
			ofs.write(reinterpret_cast<const char*>(pad.data()), namePadding);
		}
	}

	inline void align_self(std::istream& ifs, uint64_t alignment = 64) {
		ifs.seekg(align_to_boundary(ifs.tellg(), alignment), std::ios::beg);
	}
}

#endif