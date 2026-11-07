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
#include <array>

#include "spike/def_aliases.h"

#include "spike/memsys/Allocator.h"

namespace memsys {

    /// <summary>
    /// This is an allocator that holds a pool of fixed-size memory blocks for fast allocation / freeing.
    /// This is useful for allocating many small objects of the same size, such as particles or game entities.
    /// </summary>
    template<class T, usize Capacity>
    class PoolAllocatorIso {
        static_assert(Capacity > 0);

    private:
        static constexpr usize kChunkStride = AlignUp(sizeof(T), alignof(T));

        union Chunk {
            Chunk* next;
            alignas(T) u8 storage[kChunkStride];

            Chunk() noexcept : next(nullptr) {}
            ~Chunk() = default;
        };

        Chunk* _free = nullptr;
        u64 _allocationCount = 0;
        usize _allocatedMemory = 0;

        std::array<Chunk, Capacity> _pool{};

        const uptr _start;
        const uptr _end;

    public:
        PoolAllocatorIso() noexcept :
            _start(reinterpret_cast<uptr>(&_pool)),
            _end(reinterpret_cast<uptr>(&_pool) + (Capacity * kChunkStride)) {

            for (usize i = 0; i + 1 < Capacity; ++i) _pool[i].next = &_pool[i + 1];
            _pool.back().next = nullptr;
            _free = &_pool.front();
        }

        ~PoolAllocatorIso() {
            Reset();
        }

        template<class... Args>
        T* Create(Args&&... args);

        /// <summary>
        /// Calls the destructor of T and adds the chunk to the free list.
        /// </summary>
        void Destroy(T* ptr);

        /// <summary>
        /// Resets the free list and allocator bookkeeping.
        /// This does NOT destroy outstanding objects; caller is responsible for destroying live objects first.
        /// </summary>
        void Reset();

        inline const u64 GetAllocationCount() const noexcept { return _allocationCount; }
        inline const usize GetAllocatedMemory() const noexcept { return _allocatedMemory; }
        inline const usize GetMemorySize() const noexcept { return Capacity * kChunkStride; }

        inline const uptr GetStart() const noexcept { return _start; }
        inline const uptr GetEnd() const noexcept { return _end; }

        PoolAllocator(const PoolAllocator&) = delete;
        PoolAllocator(PoolAllocator&&) = delete;
    };

#pragma region Implementation

    template<class T, usize Capacity> template<class... Args>
    T* PoolAllocatorIso<T, Capacity>::Create(Args&&... args) {
        if (!_free) return nullptr;

        Chunk* chunk = _free;
        _free = _free->next;

        ++_allocationCount;
        _allocatedMemory += kChunkStride;

        try {
            return std::construct_at(reinterpret_cast<T*>(chunk->storage), std::forward<Args>(args)...);
        }
        catch (...) {
            chunk->next = _free;
            _free = chunk;

            --_allocationCount;
            _allocatedMemory = (_allocatedMemory >= kChunkStride) ? (_allocatedMemory - kChunkStride) : 0;

            throw;
        }
    }

    template<class T, usize Capacity>
    void PoolAllocatorIso<T, Capacity>::Destroy(T* ptr) {
        assert(ptr != nullptr && "PoolAllocatorIso: Free: pointer is nullptr, cannot free!");

        if (!IsPointerOwned(ptr, _start, _end)) {
            assert(false && "PoolAllocatorIso: Free: pointer is not owned!");
            return;
        }

        std::destroy_at(ptr);

        Chunk* chunk = reinterpret_cast<Chunk*>(ptr);
        chunk->next = _free;
        _free = chunk;

        --_allocationCount;
        _allocatedMemory = (_allocatedMemory >= kChunkStride) ? (_allocatedMemory - kChunkStride) : 0;
    }

    template<class T, usize Capacity>
    void PoolAllocatorIso<T, Capacity>::Reset() {
        _free = &_pool.front();
        Chunk* cur = _free;

        for (usize i = 0; i + 1 < Capacity; ++i) _pool[i].next = &_pool[i + 1];
        _pool.back().next = nullptr;

        _allocationCount = 0;
        _allocatedMemory = 0;
    }

#pragma endregion

}

#endif