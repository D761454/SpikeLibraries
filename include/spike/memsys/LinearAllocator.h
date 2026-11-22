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

#include <memory>
#include <utility>
#include <cstddef>
#include <string>

#include "spike/def_aliases.h"
#include "spike/error.hpp"

#include "spike/memsys/Allocator.h"

namespace memsys {
	/// <summary>
	/// This is an arena allocator working in a linear fashion. You may only allocate memory, but not free it individually.
	/// You must free all memory at once by calling Reset().
	/// This is useful for temporary memory that is only needed for a short time, such as during a single frame.
	/// Object dtors must be handled manually as the stack can hold many different object types!
	/// </summary>
	class LinearAllocator : Allocator {
	private:
		u64 _allocationCount;
		usize _allocatedMemory;

		const void* _memoryBase;
		const usize _memorySize;

	public:
		LinearAllocator(const void* memoryBase, const usize memorySize) : _memoryBase(memoryBase), _memorySize(memorySize), _allocationCount(0), _allocatedMemory(0) {
			if (!memoryBase)
				Report("LinearAllocator: memory base is nullptr!");
		}

		~LinearAllocator() { this->Reset(); }

		/// <summary>
		/// Allocate a memory chunk of size with the specified alignment
		/// </summary>
		/// <param name="size">size of memory chunk to allocate</param>
		/// <param name="alignment">alignment of object within memory chunk</param>
		/// <returns>pointer to the memory chunk or nullptr if it could not be allocated</returns>
		[[nodiscard]] void* Allocate(usize size, usize alignment = alignof(std::max_align_t));

		/// <summary>
		/// Allocate a memory chunk of sizeof T, ensuring alignment of T is satisfied
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <returns>pointer to the allocated object or nulltpr if it could not be allocated</returns>
		template<typename T, typename... Args>
		[[nodiscard]] T* Allocate(Args&&... args);

		/// <summary>
		/// Allocate a memory chunk of size with the specified alignment
		/// </summary>
		/// <param name="size">size of memory chunk to allocate</param>
		/// <param name="alignment">alignment of object within memory chunk</param>
		/// <returns>pointer to the memory chunk or nullptr if it could not be allocated, paired with an error 
		/// output, allowing identification of the cause of an allocation failure</returns>
		[[nodiscard]] std::pair<void*, errlib::Error> CheckedAllocate(usize size, usize alignment = alignof(std::max_align_t));

		/// <summary>
		/// Allocate a memory chunk of sizeof T, ensuring alignment of T is satisfied
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <returns>pointer to the allocated object or nulltpr if it could not be allocated, paired with an error 
		/// output, allowing identification of the cause of an allocation failure</returns>
		template<typename T, typename... Args>
		[[nodiscard]] std::pair<T*, errlib::Error> CheckedAllocate(Args&&... args);

		/// <summary>
		/// Resets the allocators memory usage and allocation count pointers to 0
		/// </summary>
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

	using BumpAllocator = LinearAllocator;
	using ArenaAllocator = LinearAllocator;

#pragma region Implementation

	void* LinearAllocator::Allocate(usize size, usize alignment) {
		if (!_memoryBase || size <= 0)
			return nullptr;

		byte* currentAddress = reinterpret_cast<byte*>(const_cast<void*>(_memoryBase)) + _allocatedMemory;

		usize adjustment;
		if (!get_adjustment(reinterpret_cast<uptr>(currentAddress), alignment, adjustment).ok())
			return nullptr;

		size += adjustment;

		if (_allocatedMemory + size > _memorySize) return nullptr;

		currentAddress += adjustment;

		_allocatedMemory += size;
		_allocationCount++;

		return reinterpret_cast<void*>(currentAddress);
	}

	template<typename T, typename... Args>
	T* LinearAllocator::Allocate(Args&&... args) {
		if (!_memoryBase)
			return nullptr;

		i32 size = sizeof(T);

		byte* currentAddress = reinterpret_cast<byte*>(const_cast<void*>(_memoryBase)) + _allocatedMemory;

		usize adjustment;
		if (!get_adjustment(reinterpret_cast<uptr>(currentAddress), alignof(T), adjustment).ok())
			return nullptr;

		size += adjustment;

		if (_allocatedMemory + size > _memorySize) return nullptr;

		currentAddress += adjustment;

		_allocatedMemory += size;
		_allocationCount++;

		try {
			return std::construct_at(reinterpret_cast<T*>(currentAddress), std::forward<Args>(args)...);
		}
		catch (...) {
			--_allocationCount;
			_allocatedMemory -= size;

			throw;
		}
		return nullptr;
	}

	std::pair<void*, errlib::Error> LinearAllocator::CheckedAllocate(usize size, usize alignment) {
		if (!_memoryBase || size <= 0)
			return { nullptr, !_memoryBase ? errlib::Error(errlib::ErrorCode_attemptingUseOfNullptr, "LinearAllocator.CheckedAllocate: memory base is nullptr!") : errlib::Error(errlib::ErrorCode_invalidArgument, "LinearAllocator.CheckedAllocate: allocation size must be > 0!") };

		byte* currentAddress = reinterpret_cast<byte*>(const_cast<void*>(_memoryBase)) + _allocatedMemory;

		usize adjustment;
		if (auto ex = get_adjustment(reinterpret_cast<uptr>(currentAddress), alignment, adjustment); !ex.ok())
			return { nullptr, ex };

		size += adjustment;

		if (_allocatedMemory + size > _memorySize)
			return { nullptr, errlib::Error(errlib::ErrorCode_outOfMemory, "LinearAllocator.CheckedAllocate: attempting allocation of size: " + std::to_string(size) + " would overflow the allocators memory!") };

		currentAddress += adjustment;

		_allocatedMemory += size;
		_allocationCount++;

		return { reinterpret_cast<void*>(currentAddress), errlib::Error() };
	}

	template<typename T, typename... Args>
	std::pair<T*, errlib::Error> LinearAllocator::CheckedAllocate(Args&&... args) {
		if (!_memoryBase)
			return { nullptr, errlib::Error(errlib::ErrorCode_attemptingUseOfNullptr, "LinearAllocator.CheckedAllocate: memory base is nullptr!") };

		i32 size = sizeof(T);

		byte* currentAddress = reinterpret_cast<byte*>(const_cast<void*>(_memoryBase)) + _allocatedMemory;

		usize adjustment;
		if (auto ex = get_adjustment(reinterpret_cast<uptr>(currentAddress), alignof(T), adjustment); !ex.ok())
			return { nullptr, ex };

		size += adjustment;

		if (_allocatedMemory + size > _memorySize)
			return { nullptr, errlib::Error(errlib::ErrorCode_outOfMemory, "LinearAllocator.CheckedAllocate: attempting allocation of size: " + std::to_string(size) + " would overflow the allocators memory!") };

		currentAddress += adjustment;

		_allocatedMemory += size;
		_allocationCount++;

		try {
			return std::construct_at(reinterpret_cast<T*>(currentAddress), std::forward<Args>(args)...);
		}
		catch (...) {
			--_allocationCount;
			_allocatedMemory -= size;

			throw;
		}
		return { nullptr, errlib::Error(errlib::ErrorCode_unknown, "LinearAllocator.CheckedAllocate: objects construction failed! recommend implementing a catch to handle this!") };
	}

#pragma endregion

}

#endif