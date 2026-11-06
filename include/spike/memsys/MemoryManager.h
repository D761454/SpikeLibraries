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
		StackAllocator* _allocator;
		void* _globalMemoryBase;

		std::vector<std::pair<const char*, void*>> _allocatedChunks;
		std::list<void*> _freeChunks; // store memory to free that is not at the head of the stack

		MemoryManager(const MemoryManager&) = delete;
		MemoryManager& operator=(MemoryManager&) = delete;

	public:
		MemoryManager();
		~MemoryManager();

		void* AllocateChunk(const usize size, const char* id = nullptr);

		void FreeChunk(void* ptr);

		void LogMemoryUsage() const;

		void IntegrityCheck() const;

		usize GetTotalAllocatedMemory();
	};

	extern MemoryManager* coreMemoryManager;
}

#endif