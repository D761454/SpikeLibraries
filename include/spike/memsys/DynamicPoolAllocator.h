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

    This will likely be the most useful in the context of 3D real-time 
    applications.

----------------------------------------------------------------------
*/

#include <vector>
#include <list>
#include <algorithm>
#include <utility>
#include <cassert>

#include "spike/def_aliases.h"

#include "spike/memsys/Allocator.h"
#include "spike/memsys/PoolAllocator.h"
#include "spike/memsys/MemoryUser.h"

namespace memsys {
    /// <summary>
    /// This is an allocator that holds a dynamic amount of memory pools, generating a new pool on filling all available pools.
    /// </summary>
    /// <typeparam name="T">Object type held within the pools</typeparam>
    /// <typeparam name="Capacity">The size of each dynamically allocated pool</typeparam>
    template<class T, usize Capacity>
    class DynamicPoolAllocator : MemoryUser, Allocator {
        static_assert(Capacity > 0);

        using PoolAlloc = PoolAllocator<T, Capacity>;

        static constexpr usize poolSize = Capacity * align_up(sizeof(T), alignof(T));
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
            void* pa = RequestMemoryChunk(poolSize, id);
            if (!pa) {
                Report("DynamicPoolAllocator: could not get memory chunk for the first pool!");
                return;
            }
            PoolAlloc* newPoolAlloc = new PoolAlloc(pa, poolSize);
            pools.push_back(new Pool(newPoolAlloc));
        }

        ~DynamicPoolAllocator();

        /// <summary>
        /// Used to construct on object within the first empty slot in an available memory pool
        /// </summary>
        /// <param name="...args">constructor arguments to be passed for the created object of type T</param>
        /// <returns>pointer to the created object, or nullptr if the object could not be created</returns>
        template<class... Args>
        [[nodiscard]] T* Create(Args&&... args);

        /// <summary>
        /// Used to destroy an object, if the object is found within any of the memory pools
        /// </summary>
        /// <param name="obj">pointer to the object to delete</param>
        /// <returns>if destruction was successful</returns>
        bool Destroy(T* obj);

        /// <summary>
        /// returns a vector of pointers to all created objects within each memory pool
        /// </summary>
        /// <returns>a vector of object pointers</returns>
        std::vector<T*> GetAllocatedObjects();

        /// <summary>
        /// returns the first valid object found within the memory pools
        /// </summary>
        /// <returns>pointer to the object or nullptr if no objects of type T are present</returns>
        T* GetFirstAllocatedObject();
    };

#pragma region Implementation

    template<class T, usize Capacity>
    DynamicPoolAllocator<T, Capacity>::~DynamicPoolAllocator() {
        for (Pool* pool : pools) {
            if (!pool) continue;

            PoolAlloc* pa = pool->allocator;
            if (!pa) continue;

            auto objects = pool->allocatedObjects;
            for (T* obj : objects) {
                if (obj) pa->Destroy(obj);
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

                if (!obj) return nullptr;

                pool->allocatedObjects.push_back(obj);
                return obj;
            }
        }

        void* pa = RequestMemoryChunk(poolSize, id);
        if (!pa) {
            Report("DynamicPoolAllocator.Create: could not get memory chunk for a new pool!");
            return nullptr;
        }

        PoolAlloc* newPoolAlloc = new PoolAlloc(pa, poolSize);
        Pool* newPool = new Pool(newPoolAlloc);
        pools.push_front(newPool);

        T* obj = newPool->allocator->Create(std::forward<Args>(args)...);

        if (!obj) return nullptr;

        newPool->allocatedObjects.push_back(obj);
        return obj;
    }

    template<class T, usize Capacity>
    bool DynamicPoolAllocator<T, Capacity>::Destroy(T* obj) {
        if (!obj) return true; // not technically an error

        for (Pool* pool : pools) {
            if (!pool) continue;
            if (!pool->allocator) continue;

            if (is_pointer_owned(obj, pool->allocator->GetStart(), pool->allocator->GetEnd())) {
                auto it = std::find(pool->allocatedObjects.begin(), pool->allocatedObjects.end(), obj);
                assert(it != pool->allocatedObjects.end() && "DynamicPoolAllocator.Destroy: pointer is within a pools bounds but does not point to an allocated chunk! Double free or stale pointer!");

                if (it == pool->allocatedObjects.end()) {
                    Report("DynamicPoolAllocator.Destroy: pointer is within a pools bounds but does not point to an allocated chunk! Double free or stale pointer!");
                    return false;
                }

                pool->allocatedObjects.erase(it);
                pool->allocator->Destroy(obj);
                return true;
            }
        }
        Report("DynamicPoolAllocator.Destroy: pointer is valid but does not exist within any pools of this dpa!");
        return false;
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