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

#ifndef __MEMORY_SYSTEM_ARENA_ALLOCATOR_H
#define __MEMORY_SYSTEM_ARENA_ALLOCATOR_H

/*
----------------------------------------------------------------------

File Summary:
	LinearAllocator class - allocator used to provide / allocate memory
	in a linear fashion, whereby memory may not be freed individually.

----------------------------------------------------------------------
*/

#include <cassert>
#include <cstddef>

#include "spike/def_aliases.h"

namespace memsys {
	/// <summary>
	/// This is an arena allocator working in a linear fashion. You may only allocate memory, but not free it individually.
	/// You must free all memory at once by calling Reset().
	/// This is useful for temporary memory that is only needed for a short time, such as during a single frame.
	/// Object dtors must be handled manually as the stack can hold many different object types!
	/// </summary>
	class LinearAllocator {
	private:
		u64 _allocationCount;
		usize _allocatedMemory;

		const void* _memoryBase;
		const usize _memorySize;

		inline bool IsPowerOfTwo(uptr n) const { return (n & (n - 1)) == 0; }

		uptr AlignForward(uptr address, usize alignment) const {
			assert(this->IsPowerOfTwo(alignment) && "Alignment must be a power of two!");

			usize adjustment = alignment - (address & (alignment - 1));

			if (adjustment == alignment) {
				return address;
			}

			return address + adjustment;
		}

	public:
		LinearAllocator(const void* memoryBase, const usize memorySize) : _memoryBase(memoryBase), _memorySize(memorySize), _allocationCount(0), _allocatedMemory(0) {
			assert(memoryBase != nullptr && "LinearAllocator: Memory base is nullptr!");
		}

		~LinearAllocator() { this->Reset(); }

		void* Allocate(usize size, usize alignment = alignof(std::max_align_t)) {
			assert(size > 0 && "LinearAllocator: Memory allocation must have a size > 0!");
			
			// need to tweak to align forward 

			if (_allocatedMemory + size > _memorySize) return nullptr;

			byte* currentAddress = reinterpret_cast<byte*>(const_cast<void*>(_memoryBase)) + _allocatedMemory;

			_allocatedMemory += size;
			_allocationCount++;

			return reinterpret_cast<void*>(currentAddress);
		}

		void Free(void* ptr) {
			assert(0 && "LinearAllocator: Free: Cannot Free Memory Individually!");
		}

		void Reset() noexcept {
			_allocatedMemory = 0;
			_allocationCount = 0;
		}

		inline const void* GetMemoryBase() const { return _memoryBase; }
		inline const u64 GetAllocationCount() const noexcept { return _allocationCount; }
		inline const usize GetAllocatedMemory() const noexcept { return _allocatedMemory; }
		inline const usize GetMemorySize() const noexcept { return _memorySize; }

		LinearAllocator(const LinearAllocator&) = delete;
		LinearAllocator(LinearAllocator&&) = delete;
	};
}

#endif