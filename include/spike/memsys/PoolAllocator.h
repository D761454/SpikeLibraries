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

#ifndef __MEMORY_SYSTEM_POOL_ALLOCATOR_H
#define __MEMORY_SYSTEM_POOL_ALLOCATOR_H

/*
----------------------------------------------------------------------

File Summary:
    PoolAllocator class - allocator used to provide memory
    within pre allocated pools. This allows for the removal of memory
    acquisition overhead. Equally, the use of memory pools can provide
    cache line optimization.

----------------------------------------------------------------------
*/

#include <memory>
#include <utility>
#include <cassert>
#include <unordered_set>
#include <algorithm>

#include "spike/def_aliases.h"

#include "spike/memsys/Allocator.h"

namespace memsys {
    /// <summary>
    /// This is an allocator that holds a pool of fixed-size memory blocks for fast allocation / freeing.
    /// This is useful for allocating many small objects of the same size, such as particles or game entities.
    /// </summary>
    template<class T, usize Capacity>
    class PoolAllocator : Allocator {
        static_assert(Capacity > 0);

    private:
        union Chunk {
            Chunk* next;
            alignas(T) u8 storage[align_up(sizeof(T), alignof(T))];

            Chunk() noexcept : next(nullptr) {}
            ~Chunk() = default;
        };

        u64 _allocationCount = 0;
        usize _allocatedMemory = 0;

        Chunk* _free = nullptr;

        const void* _memoryBase = nullptr;
        const usize _memorySize = 0;

        uptr _start;
        uptr _end;

    public:
        inline static constexpr usize kChunkSize = sizeof(Chunk);

        PoolAllocator(const void* memoryBase, const usize memorySize) : 
            _memoryBase(memoryBase),
            _memorySize(memorySize) {

            if (!memoryBase) {
                Report("PoolAllocator: memory base is nullptr!");
                _start = 0;
                _end = 0;
                return;
            }

            assert(memorySize == Capacity * kChunkSize && "PoolAllocator: incorrect memory size to house 'Capacity' objects within the pool!");
            if (memorySize != Capacity * kChunkSize) {
                _start = 0;
                _end = 0;
                return;
            }

            _start = reinterpret_cast<uptr>(memoryBase);
            _end = reinterpret_cast<uptr>(memoryBase) + memorySize;

            _free = reinterpret_cast<Chunk*>(const_cast<void*>(_memoryBase));
            Chunk* cur = _free;

            for (usize i = 1; i < Capacity; ++i) {
                cur->next = reinterpret_cast<Chunk*>(const_cast<void*>(_memoryBase)) + i;
                cur = cur->next;
            }

            cur->next = nullptr;
        }

        ~PoolAllocator() {
            Reset();
        }

        /// <summary>
        /// Used to construct an object of type T within the next free slot in the memory pool
        /// </summary>
        /// <param name="...args">constructor arguments to be passed for the created object of type T</param>
        /// <returns>pointer to the created object, or nullptr if the object could not be created</returns>
        template<typename... Args>
        [[nodiscard]] T* Create(Args&&... args);

        /// <summary>
        /// Calls the destructor of T and adds the chunk to the free list.
        /// </summary>
        /// <returns>if the destroy was successful</returns>
        bool Destroy(T* ptr);

        /// <summary>
        /// Resets the free list and allocator bookkeeping.
        /// </summary>
        void Reset();

        inline const void* GetMemoryBase() const { return _memoryBase; }
        inline const u64 GetAllocationCount() const noexcept { return _allocationCount; }
        inline const usize GetAllocatedMemory() const noexcept { return _allocatedMemory; }
        inline const usize GetMemorySize() const noexcept { return _memorySize; }

        inline const uptr GetStart() const noexcept { return _start; }
        inline const uptr GetEnd() const noexcept { return _end; }

        PoolAllocator(const PoolAllocator&) = delete;
        PoolAllocator(PoolAllocator&&) = delete;
    };

#pragma region Implementation

    template<class T, usize Capacity> template<typename... Args>
    T* PoolAllocator<T, Capacity>::Create(Args&&... args) {
        if (!_free) return nullptr;

        Chunk* chunk = _free;
        _free = _free->next;

        ++_allocationCount;
        _allocatedMemory += sizeof(Chunk);

        try {
            return std::construct_at(reinterpret_cast<T*>(chunk->storage), std::forward<Args>(args)...);
        }
        catch (...) {
            chunk->next = _free;
            _free = chunk;

            --_allocationCount;
            _allocatedMemory = (_allocatedMemory >= kChunkSize) ? (_allocatedMemory - kChunkSize) : 0;

            throw;
        }
        return nullptr;
    }

    template<class T, usize Capacity>
    bool PoolAllocator<T, Capacity>::Destroy(T* ptr) {
        if (!ptr) return true; // not technically an error
        if (!is_pointer_owned(ptr, _start, _end)) return false;

        Chunk* chunk = reinterpret_cast<Chunk*>(ptr);

        bool aligned = false;
        for (usize i = 0; i < Capacity; ++i) {
            if (reinterpret_cast<Chunk*>(const_cast<void*>(_memoryBase)) + i == ptr) aligned = true;
        }
        if (!aligned) return false;

        bool freed = false;
        Chunk* freeC = _free;

        while (freeC) {
            assert(freeC != chunk && "chunk is already free during destroy, double free or stale pointer!");
            if (freeC == chunk) {
                freed = true;
                break;
            }
            freeC = freeC->next;
        }
        if (freed) return false;

        std::destroy_at(ptr);

        chunk->next = _free;
        _free = chunk;

        --_allocationCount;
        _allocatedMemory = (_allocatedMemory >= kChunkSize) ? (_allocatedMemory - kChunkSize) : 0;
        return true;
    }

    template<class T, usize Capacity>
    void PoolAllocator<T, Capacity>::Reset() {
        if (!_memoryBase) return;
        if (_start == _end) return;

        std::unordered_set<Chunk*> free;

        Chunk* f = _free;
        while (f) {
            free.insert(f);
            f = f->next;
        }

        _free = reinterpret_cast<Chunk*>(const_cast<void*>(_memoryBase));
        Chunk* cur = _free;

        for (usize i = 1; i < Capacity; ++i) {
            if (!free.contains(cur)) { // live object
                std::destroy_at(reinterpret_cast<T*>(cur));
            }

            cur->next = reinterpret_cast<Chunk*>(const_cast<void*>(_memoryBase)) + i;
            cur = cur->next;
        }

        if (!free.contains(cur)) { // live object
            std::destroy_at(reinterpret_cast<T*>(cur));
        }

        cur->next = nullptr;

        _allocationCount = 0;
        _allocatedMemory = 0;
    }

#pragma endregion

}

#endif