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

#ifndef __MEMORY_SYSTEM_STACK_ALLOCATOR_H
#define __MEMORY_SYSTEM_STACK_ALLOCATOR_H

/*
----------------------------------------------------------------------

File Summary:
	StackAllocator class - allocator used to provide / allocate memory
	in a stack like manner. Memory can be allocated and freed in a
	last-in, first-out manner.

----------------------------------------------------------------------
*/

#if defined (_WIN32) || defined(__linux__)
#include <malloc.h>
#elif defined(__APPLE__)
#include <stdlib.h>
#endif
#include <memory>
#include <utility>
#include <cassert>
#include <vector>
#include <cstddef>
#include <algorithm>

#include "spike/def_aliases.h"
#include "spike/debugl/debug.hpp"

#include "spike/memsys/Allocator.h"

namespace memsys {
	/// <summary>
	/// This is an allocator that behaves like a stack.
	/// You can push (allocate) and pop (free) memory in LIFO (Last-In, First-Out) order.
	/// Object dtors must be handled manually as the stack can hold many different object types!
	/// </summary>
	class StackAllocatorIso {
	private:
		u64 _allocationCount;
		usize _allocatedMemory;

		void* _memoryBase;
		const usize _memorySize;

		std::vector<MemoryHeader*> _stack;
		std::vector<MemoryHeader*> _toFree;

	public:
		StackAllocatorIso(const usize memorySize) : _memoryBase(malloc(memorySize)), _memorySize(memorySize), _allocationCount(0), _allocatedMemory(0) {
			assert(_memoryBase != nullptr && "StackAllocatorIso: Memory base is nullptr!");
		}

		~StackAllocatorIso() { 
			Reset();

			free(_memoryBase);
			_memoryBase = nullptr;
		}

		void* Allocate(usize size, usize alignment = alignof(std::max_align_t));

		void Free(void* ptr);

		void Reset() noexcept;

		inline const void* GetMemoryBase() const { return _memoryBase; }
		inline const u64 GetAllocationCount() const noexcept { return _allocationCount; }
		inline const usize GetAllocatedMemory() const noexcept { return _allocatedMemory; }
		inline const usize GetMemorySize() const noexcept { return _memorySize; }

		StackAllocatorIso(const StackAllocatorIso&) = delete;
		StackAllocatorIso(StackAllocatorIso&&) = delete;
	};

#pragma region Implementation

	void* StackAllocatorIso::Allocate(usize size, usize alignment) {
		assert(size > 0 && "StackAllocator: Memory allocation must have a size > 0!");

		usize newAlignment = std::max(alignof(MemoryHeader), alignment);
		size += sizeof(MemoryHeader);

		byte* currentAddress = reinterpret_cast<byte*>(const_cast<void*>(_memoryBase)) + _allocatedMemory;

		usize adjustment = GetAdjustment(reinterpret_cast<uptr>(currentAddress), newAlignment);
		size += adjustment;

		if (_allocatedMemory + size > _memorySize) {
			Debug::LogError("Cannot Allocate to stack allocator, no free space to fit object of size: ", size);
			return nullptr;
		}

		currentAddress += adjustment;

		MemoryHeader* basePtr = reinterpret_cast<MemoryHeader*>(currentAddress);
		MemoryHeader* mH = std::construct_at(basePtr, size);

		void* vP = reinterpret_cast<void*>(mH + 1);

		_allocatedMemory += size;
		_allocationCount++;

		_stack.push_back(mH);

		return vP;
	}

	void StackAllocatorIso::Free(void* ptr) {
		assert(ptr && "StackAllocator: Free: Cannot Free a nullptr!");

		MemoryHeader* mH = reinterpret_cast<MemoryHeader*>(reinterpret_cast<byte*>(ptr) - sizeof(MemoryHeader));

#if _DEBUG
		assert(mH->checkVal == kCheckVal && "StackAllocatorIso: Free: Memory corruption detected! CheckVal does not match expected value.");
#endif

		if (mH == _stack.back()) {
			_stack.pop_back();

			_allocatedMemory -= mH->size;
			_allocationCount--;
		}
		else {
			_toFree.push_back(mH);
			return;
		}

		// this method allows freeing of suballocations without wiping 
		// live allocations if the suballocation to be deleted came before

		bool loop = true;
		while (loop) {
			bool inner = false;
			for (auto it : _toFree) {
				if (it == _stack.back()) {
					_allocatedMemory -= it->size;
					_allocationCount--;
					inner = true;
				}
			}
			if (!inner) {
				loop = false;
			}
		}
	}

	void StackAllocatorIso::Reset() noexcept {
		_allocatedMemory = 0;
		_allocationCount = 0;

		_toFree.clear();
		_stack.clear();
	}

#pragma endregion

}

#endif