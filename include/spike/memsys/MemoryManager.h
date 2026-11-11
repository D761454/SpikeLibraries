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

#ifndef __MEMORY_SYSTEM_MEMORY_MANAGER_H
#define __MEMORY_SYSTEM_MEMORY_MANAGER_H

#include <list>
#include <utility>
#include <vector>

#include "spike/def_aliases.h"

namespace memsys {
	class StackAllocator;

	/// <summary>
	/// primary memory manager class, overseeing the primary allocated memory for the program.
	/// it can dish out memory chunks to users, allowing for these chunks to be managed by
	/// different allocator types. The manager uses a linear allocator for its memory management.
	/// </summary>
	class MemoryManager {
	private:
		inline static const usize kTotalSize = kMegaByte * 128;

		StackAllocator* _allocator;
		void* _globalMemoryBase;

		std::vector<std::pair<const char*, void*>> _allocatedChunks;
		std::list<void*> _freeChunks; // store memory to free that is not at the head of the stack

		MemoryManager(const MemoryManager&) = delete;
		MemoryManager& operator=(const MemoryManager&) = delete;

	public:
		MemoryManager();
		~MemoryManager();

		/// <summary>
		/// Returns a pointer in memory to an allocated memory chunk of size. If no chunk is found / available returns nullptr.
		/// </summary>
		/// <param name="size">memory size requested</param>
		/// <param name="id">user id to associate with the allocated chunk</param>
		/// <returns>pointer to memory if there is space available, otherwise nullptr</returns>
		void* AllocateChunk(const usize size, const char* id = nullptr);

		/// <summary>
		/// Frees a previously allocated memory chunk.
		/// </summary>
		/// <param name="ptr">pointer to the base of the allocated memory chunk to be freed</param>
		void FreeChunk(void* ptr);

		/// <summary>
		/// Logs the current memory usage using Debug::Log. Note: the memory logged is the total allocated, not the total used within those allocations!
		/// </summary>
		void LogMemoryUsage() const;

		/// <summary>
		/// Checks if there are nay unfreed chunks in memory. Recommended for use on shutdown as a security check.
		/// </summary>
		void IntegrityCheck() const;

		/// <summary>
		/// Used to get the total allocated/used memory from the managers stack allocator.
		/// </summary>
		/// <returns>the total memory currently used within the managers stack allocator</returns>
		usize GetTotalAllocatedMemory();
	};

	/// <summary>
	/// Retrieve the singular instance of the memory manager
	/// </summary>
	/// <returns>the memory manager instance</returns>
	MemoryManager& GetMemoryManager();
}

#endif