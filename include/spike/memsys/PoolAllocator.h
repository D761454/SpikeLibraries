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
#include <type_traits>
#include <limits>

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
        static_assert(std::is_nothrow_destructible_v<T>, "PoolAllocator requires a non-throwing T destructor");

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
        static_assert(Capacity <= std::numeric_limits<usize>::max() / kChunkSize, "PoolAllocator pool size overflows usize");

        PoolAllocator(const void* memoryBase, const usize memorySize) :
            _memoryBase(memoryBase),
            _memorySize(memorySize) {

            if (!memoryBase) {
                Report("PoolAllocator: memory base is nullptr!");
                _start = 0;
                _end = 0;
                return;
            }

            const uptr baseAddress = reinterpret_cast<uptr>(memoryBase);
            if (baseAddress % alignof(Chunk) != 0) {
                Report("PoolAllocator: memory base does not satisfy the pool chunk alignment requirement!");
                _start = 0;
                _end = 0;
                return;
            }

            if (memorySize > static_cast<usize>(-1) - baseAddress) {
                Report("PoolAllocator: memory range overflows the address space!");
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
        /// Destroys all live objects and resets the free list and allocator bookkeeping.
        /// </summary>
        void Reset() noexcept;

        inline const void* GetMemoryBase() const { return _memoryBase; }
        inline const u64 GetAllocationCount() const noexcept { return _allocationCount; }
        inline const usize GetAllocatedMemory() const noexcept { return _allocatedMemory; }
        inline const usize GetMemorySize() const noexcept { return _memorySize; }

        inline const uptr GetStart() const noexcept { return _start; }
        inline const uptr GetEnd() const noexcept { return _end; }
        inline bool IsValid() const noexcept { return _start != _end; }

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
        if (!IsValid() || !is_pointer_owned(ptr, _start, _end)) return false;

        Chunk* chunk = nullptr;
        Chunk* base = reinterpret_cast<Chunk*>(const_cast<void*>(_memoryBase));
        for (usize i = 0; i < Capacity; ++i) {
            Chunk* candidate = base + i;
            if (reinterpret_cast<T*>(candidate->storage) == ptr) {
                chunk = candidate;
                break;
            }
        }
        if (!chunk || _allocationCount == 0) return false;

        for (Chunk* freeChunk = _free; freeChunk; freeChunk = freeChunk->next) {
            assert(freeChunk != chunk && "chunk is already free during destroy, double free or stale pointer!");
            if (freeChunk == chunk) return false;
        }

        std::destroy_at(ptr);
        chunk->next = _free;
        _free = chunk;

        --_allocationCount;
        _allocatedMemory -= kChunkSize;
        return true;
    }

    template<class T, usize Capacity>
    void PoolAllocator<T, Capacity>::Reset() noexcept {
        if (!IsValid()) return;

        Chunk* base = reinterpret_cast<Chunk*>(const_cast<void*>(_memoryBase));
        for (usize i = 0; i < Capacity; ++i) {
            Chunk* candidate = base + i;
            bool isFree = false;
            for (Chunk* freeChunk = _free; freeChunk; freeChunk = freeChunk->next) {
                if (freeChunk == candidate) {
                    isFree = true;
                    break;
                }
            }
            if (!isFree)
                std::destroy_at(reinterpret_cast<T*>(candidate->storage));
        }

        _free = base;
        for (usize i = 0; i + 1 < Capacity; ++i)
            (base + i)->next = base + i + 1;
        (base + Capacity - 1)->next = nullptr;

        _allocationCount = 0;
        _allocatedMemory = 0;
    }

#pragma endregion

}

#endif