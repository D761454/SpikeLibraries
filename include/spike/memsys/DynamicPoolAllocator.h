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

#ifndef __MEMORY_SYSTEM_DYN_POOL_ALLOCATOR_H
#define __MEMORY_SYSTEM_DYN_POOL_ALLOCATOR_H

/*
----------------------------------------------------------------------

File Summary:
    DynamicPoolAllocator class - allocator used to provide / allocate
    memory within pre allocated pools. When a pool fills, another
    pool of the same size is allocated. The allocator maintains a list
    of pools and the active object list per pool.

----------------------------------------------------------------------
*/

#include <vector>
#include <list>
#include <algorithm>
#include <utility>
#include <cassert>

#include "spike/def_aliases.h"
#include "spike/debugl/debug.hpp"

#include "spike/memsys/Allocator.h"
#include "spike/memsys/PoolAllocator.h"
#include "spike/memsys/MemoryUser.h"

// need to edit allocatprs to allow self governing memory versions

namespace memsys {

    template<class T, usize Capacity>
    class DynamicPoolAllocator : MemoryUser {
        static_assert(Capacity > 0);

        using PoolAlloc = PoolAllocator<T, Capacity>;

        static constexpr usize poolSize = Capacity * AlignUp(sizeof(T), alignof(T));
        const char* id = nullptr;

    public:
        class Pool {
        public:
            PoolAlloc* allocator = nullptr;
            std::vector<T*> allocatedObjects;

            explicit Pool(PoolAlloc* alloc) : allocator(alloc) {
                allocatedObjects.clear();
                allocatedObjects.reserve(Capacity);
            }
        };

    protected:
        std::list<Pool*> pools;

    public:
        explicit DynamicPoolAllocator(const char* id = nullptr) : id(id) {
            PoolAlloc* newPoolAlloc = new PoolAlloc(RequestMemoryChunk(poolSize, id), poolSize);
            pools.push_back(new Pool(newPoolAlloc));
        }

        ~DynamicPoolAllocator();

        template<class... Args>
        T* Create(Args&&... args);

        void Destroy(T* obj);

        [[deprecated("Work In Progress. Likely broken or has faulty logic / lack of edge case handling.")]] void* DuplicateObject(void* src) {
            // WIP - Need to modify and test
            return nullptr;
        }

        std::vector<T*> GetAllocatedObjects();

        T* GetFirstAllocatedObject();
    };

#pragma region Implementation

    template<class T, usize Capacity>
    DynamicPoolAllocator<T, Capacity>::~DynamicPoolAllocator() {
        for (Pool* pool : pools) {
            if (!pool) continue;

            PoolAlloc* pa = pool->allocator;
            if (!pa) {
                Debug::LogWarning("FATAL: DPA: PoolAllocator is nullptr, cannot free memory chunk!");
                assert(0 && "FATAL: DPA: PoolAllocator is nullptr, cannot free memory chunk!");
                continue;
            }

            auto objects = pool->allocatedObjects;
            for (T* obj : objects) {
                if (obj) {
                    pa->Destroy(obj);
                }
            }

            pool->allocatedObjects.clear();

            void* base = const_cast<void*>(pa->GetMemoryBase());

            delete pa;
            pool->allocator = nullptr;

            FreeMemoryChunk(base);

            delete pool;
        }

        pools.clear();
    }

    template<class T, usize Capacity> template<class... Args>
    T* DynamicPoolAllocator<T, Capacity>::Create(Args&&... args) {
        for (Pool* pool : pools) {
            if (pool && pool->allocatedObjects.size() < Capacity) {
                T* obj = pool->allocator->Create(std::forward<Args>(args)...);

                if (!obj) {
                    Debug::LogError("DynamicPoolAllocator: Failed to allocate memory for new object!");
                    assert(obj != nullptr && "DynamicPoolAllocator: Failed to allocate memory for new object!");
                    return nullptr;
                }

                pool->allocatedObjects.push_back(obj);
                return obj;
            }
        }

        PoolAlloc* newPoolAlloc = new PoolAlloc(RequestMemoryChunk(poolSize, id), poolSize);
        Pool* newPool = new Pool(newPoolAlloc);
        pools.push_front(newPool);

        T* obj = newPool->allocator->Create(std::forward<Args>(args)...);

        if (!obj) {
            Debug::LogError("DynamicPoolAllocator: Failed to allocate memory for new object!");
            assert(obj != nullptr && "DynamicPoolAllocator: Failed to allocate memory for new object!");
            return nullptr;
        }

        newPool->allocatedObjects.push_back(obj);
        return obj;
    }

    template<class T, usize Capacity>
    void DynamicPoolAllocator<T, Capacity>::Destroy(T* obj) {
        if (!obj) {
            Debug::LogWarning("DynamicPoolAllocator: DestroyObject called with nullptr.");
            return;
        }

        for (Pool* pool : pools) {
            if (!pool) continue;
            if (!pool->allocator) continue;

            if (IsPointerOwned(obj, pool->allocator->GetStart(), pool->allocator->GetEnd())) {
                auto it = std::find(pool->allocatedObjects.begin(), pool->allocatedObjects.end(), obj);
                if (it == pool->allocatedObjects.end()) {
                    Debug::LogError("DynamicPoolAllocator: Attempted to free stale pointer or double-free detected.");
                    return;
                }

                pool->allocatedObjects.erase(it);
                pool->allocator->Destroy(obj);
                return;
            }
        }

        Debug::LogError("DynamicPoolAllocator: Failed to find pool for object to destroy. Pointer is likely stale or corrupted.");
    }

    template<class T, usize Capacity>
    std::vector<T*> DynamicPoolAllocator<T, Capacity>::GetAllocatedObjects() {
        std::vector<T*> v;

        for (Pool* pool : pools) {
            if (!pool) continue;

            v.insert(v.end(), pool->allocatedObjects.begin(), pool->allocatedObjects.end());
        }

        return v;
    }

    template<class T, usize Capacity>
    T* DynamicPoolAllocator<T, Capacity>::GetFirstAllocatedObject() {
        for (Pool* pool : pools) {
            if (!pool) continue;

            if (!pool->allocatedObjects.empty()) return pool->allocatedObjects[0];
        }

        return nullptr;
    }

#pragma endregion
}

#endif