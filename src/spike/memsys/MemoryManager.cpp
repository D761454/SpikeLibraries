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

#include "spike/memsys/MemoryManager.h"

#if defined (_WIN32) || defined(__linux__)
#include <malloc.h>
#elif defined(__APPLE__)
#include <stdlib.h>
#endif
#include <cassert>
#include <utility>

#include "spike/def_aliases.h"
#include "spike/debugl/debug.hpp"

#include "spike/memsys/StackAllocator.h"

namespace memsys {
	MemoryManager::MemoryManager() {
		_globalMemoryBase = malloc(kTotalSize);

		assert(_globalMemoryBase != nullptr && "Failed to allocate global memory for MemoryManager!");

		_allocator = new StackAllocator(_globalMemoryBase, kTotalSize);
		assert(_allocator != nullptr && "Failed to create primary allocator for MemoryManager!");

		_allocatedChunks.clear();
		_freeChunks.clear();
	}

	MemoryManager::~MemoryManager() {
		_allocator->Reset();

		delete _allocator;
		_allocator = nullptr;

		free(_globalMemoryBase);
		_globalMemoryBase = nullptr;

		_allocatedChunks.clear();
		_freeChunks.clear();
	}

	void* MemoryManager::AllocateChunk(const usize size, const char* id) {
		void* chunk = _allocator->Allocate(size);
		_allocatedChunks.push_back(std::pair<const char*, void*>(id, chunk));

		return chunk;
	}

	void MemoryManager::FreeChunk(void* ptr) {
		if (!ptr) return;
		if (ptr == _allocatedChunks.back().second) {
			_allocator->Free(ptr);
			_allocatedChunks.pop_back();

			bool toFree = true;
			while (toFree) {
				toFree = false;

				for (void* freeChunk : _freeChunks) {
					if (freeChunk == _allocatedChunks.back().second) {
						_allocator->Free(freeChunk);
						_allocatedChunks.pop_back();
						_freeChunks.remove(freeChunk);

						toFree = true;
						break;
					}
				}
			}
		}
		else {
			_freeChunks.push_back(ptr);
		}
	}

	void MemoryManager::LogMemoryUsage() const {
		if (_allocator) {
			Debug::Log("Current Memory Usage: {} / {}", _allocator->GetAllocatedMemory(), _allocator->GetMemorySize());
		}
		else Debug::LogError("WARNING! No primary allocator for memory manager!");
	}

	void MemoryManager::IntegrityCheck() const {
		if (_allocatedChunks.size() > 0) {
			Debug::LogError("!!! MEMORY LEAK !!!");

			for (std::pair<const char*, const void*> chunk : _allocatedChunks) {
				bool isFree = false;

				for (const void* freechunk : _freeChunks) {
					if (freechunk == chunk.second) {
						isFree = true;
						break;
					}
				}

				if (!isFree) {
					Debug::LogError("Leaked Chunk {} at address {}", chunk.first, chunk.second);
				}
			}
		}
	}

	const usize MemoryManager::GetTotalAllocatedMemory() const noexcept {
		return _allocator ? _allocator->GetAllocatedMemory() : 0;
	}

	MemoryManager& GetMemoryManager() {
		static MemoryManager instance;
		return instance;
	}
}