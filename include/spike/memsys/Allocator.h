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

#ifndef __MEMORY_SYSTEM_ALLOCATOR_H
#define __MEMORY_SYSTEM_ALLOCATOR_H

/*
----------------------------------------------------------------------

File Summary:
	Holds generic memory functions used throughout memsys

----------------------------------------------------------------------
*/

#include <cassert>

#include "spike/def_aliases.h"

namespace memsys {
	/// <summary>
	/// Calculates the size required by an object of size 'value' with alignment 'alignment' 
	/// returning the total size needed to place objects of that size subsequently in memory while respecting alignment
	/// </summary>
	/// <param name="value">size of the object to align</param>
	/// <param name="alignment">alignemnt required by the object</param>
	/// <returns>memory space required to house the object while respecting alignment</returns>
	static inline constexpr const usize AlignUp(usize value, usize alignment) {
		assert(alignment != 0 && (alignment & (alignment - 1)) == 0 && "Alignment must be power of two");
		return (value + alignment - 1) & ~(alignment - 1);
	}

	/// <summary>
	/// Used to get the offset needed to align ptr in memory based on adjustment
	/// </summary>
	/// <param name="ptr">pointer to the memory location to align</param>
	/// <param name="alignment">desired alignment for an object at ptr</param>
	/// <returns>amount of bytes needed to shift ptr by to correctly align it</returns>
	static inline usize GetAdjustment(uptr ptr, usize alignment) {
		assert(alignment == 1 || (alignment & (alignment - 1)) == 0 && "Alignment must be a power of 2, or 1");

		usize adjustment = alignment - (ptr & (alignment - 1));
		if (adjustment == alignment) return 0;
		return adjustment;
	}

	/// <summary>
	/// Used to shift an address' position to ensure memory alignment
	/// </summary>
	/// <param name="ptr">pointer to the memory location to align</param>
	/// <param name="alignment">desired alignment for an object at ptr</param>
	/// <returns>aligned pointer</returns>
	static inline uptr AlignForward(uptr ptr, usize alignment) {
		assert(alignment == 1 || (alignment & (alignment - 1)) == 0 && "Alignment must be a power of 2, or 1");

		usize adjustment = alignment - (ptr & (alignment - 1));
		if (adjustment == alignment) return ptr;
		return ptr + adjustment;
	}

#if _DEBUG
	inline constexpr u32 kCheckVal = 0xDEADBEEF; // 3735928559U
#endif

	struct MemoryHeader {
		usize size;
#if _DEBUG
		const u32 checkVal = kCheckVal;
#endif

		MemoryHeader(usize size) noexcept : size(size) {}
		~MemoryHeader() = default;
	};

	/// <summary>
	/// Determines if a pointer is situated between 2 memory positions
	/// </summary>
	/// <param name="ptr">pointer to check</param>
	/// <param name="start">lower bound for the range check</param>
	/// <param name="end">upper bound for the range check</param>
	/// <returns>if the pointer is located between start and end</returns>
	inline bool IsPointerOwned(const void* ptr, const uptr start, const uptr end) {
		assert(ptr != nullptr && "Cannot check ownership of a nullptr!");
		if (!ptr) return false;

		const uptr objAddr = reinterpret_cast<uptr>(ptr);
		return (objAddr >= start && objAddr < end);
	}
}

#endif