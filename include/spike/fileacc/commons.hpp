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

#ifndef __SPIKE_FILE_ACCESSOR_COMMONS
#define __SPIKE_FILE_ACCESSOR_COMMONS

#include <set>

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"
#include "spike/warning.h"

namespace fileacc {
	DISABLE_WARNING_PUSH
	DISABLE_WARNING_UNINITIALIZED_VARIABLE

	/* file header data used to verify file validity and type */
	struct FileCheck {
		/* denotes content structure for the file */
		u32 magic;
		/* determines fileacc version used for file creation */
		u32 version;

		FileCheck() noexcept : magic(0), version(0) {}
		FileCheck(const u32 magic, const u32 version) noexcept : magic(magic), version(version) {}
	};

	/// <summary>
	/// Checks and compares the file header with the expected values to determine if the file is an expected type
	/// </summary>
	/// <param name="header">file header read from the file</param>
	/// <param name="expectedMagic">magic value that should be held within header</param>
	/// <param name="expectedVer">version header that should be held in the header</param>
	/// <returns>if the header values match the expected values: 0 = good, -1 = magic error, -2 = version error</returns>
	inline i32 valid_file(FileCheck& header, const u32 expectedMagic, const u32 expectedVer) noexcept {
		if (header.magic != expectedMagic) return -1;
		if (header.version != expectedVer) return -2;
		return 0;
	}

	/// <summary>
	/// Checks and compares the file header with the expected values to determine if the file is one of the expected types
	/// </summary>
	/// <param name="header">file header read from the file</param>
	/// <param name="expectedMagicSet">a set of valid file magics that the header should use</param>
	/// <param name="expectedVer">version header that should be held in the header</param>
	/// <returns>if the header values match the expected values: 0 = good, -1 = magic error, -2 = version error</returns>
	inline i32 valid_file(FileCheck& header, std::set<u32> expectedMagicSet, const u32 expectedVer) noexcept {
		if (!expectedMagicSet.contains(header.magic)) return -1;
		if (header.version != expectedVer) return -2;
		return 0;
	}

	/// <summary>
	/// Checks if a file exists and, if so, that the header data matches the expected data
	/// </summary>
	/// <param name="path">path to the file to check</param>
	/// <param name="expectedMagic">magic value that should be held within header</param>
	/// <param name="expectedVer">version header that should be held in the header</param>
	/// <returns>if the file exists and the headers match: 0 = good, -1 = magic error, -2 = version error, -3 = file error</returns>
	i32 check_file(const string& path, const u32 expectedMagic, const u32 expectedVer) noexcept;

	/// <summary>
	/// Checks if a file exists and, if so, that the header data matches the expected data
	/// </summary>
	/// <param name="path">path to the file to check</param>
	/// <param name="expectedMagicSet">a set of valid file magics that the header should use</param>
	/// <param name="expectedVer">version header that should be held in the header</param>
	/// <returns>if the file exists and the headers match: 0 = good, -1 = magic error, -2 = version error, -3 = file error</returns>
	i32 check_file(const string& path, std::set<u32> expectedMagicSet, const u32 expectedVer) noexcept;

	/// <summary>
	/// Generates a hash from a buffer of size len
	/// </summary>
	/// <param name="data">pointer to the buffer to hash</param>
	/// <param name="len">length of the buffer</param>
	/// <returns>hash value corresponding to the buffer</returns>
	inline u64 fnv1a64(const char* data, usize len) {
		const u64 fnvOffset = 14695981039346656037ULL;
		const u64 fnvPrime = 1099511628211ULL;
		u64 hash = fnvOffset;
		const u8* ptr = reinterpret_cast<const u8*>(data);
		for (usize i = 0; i < len; i++) {
			hash ^= static_cast<u64>(ptr[i]);
			hash *= fnvPrime;
		}
		return hash;
	}

	DISABLE_WARNING_POP
}

#endif