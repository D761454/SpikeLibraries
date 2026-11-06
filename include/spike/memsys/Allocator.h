/*
Allocator.h - MemorySystem (memsys / Allocator)
----------------------------------------------------------------------

Copyright (c) 2026-2026, SpikeTooth, Cayden Andrews

All rights reserved.

* Re-distributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Re-distributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of SpikeTooth, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the SpikeTooth team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------
*/

#ifndef __MEMORY_SYSTEM_ALLOCATOR_H
#define __MEMORY_SYSTEM_ALLOCATOR_H

/*
----------------------------------------------------------------------

File Summary:


----------------------------------------------------------------------
*/

#include <cassert>
#include <spike/def_aliases.h>

namespace memsys {
	static inline constexpr const usize AlignUp(usize value, usize alignment) {
		assert(alignment != 0 && (alignment & (alignment - 1)) == 0 && "Alignment must be power of two");
		return (value + alignment - 1) & ~(alignment - 1);
	}

#if _DEBUG
	inline constexpr u32 kCheckVal = 0xDEADBEEF; // 3735928559U
#endif

	struct MemoryHeader {
		usize size;
#if _DEBUG
		const u32 checkVal = kCheckVal;
#endif

		MemoryHeader(usize size) noexcept : size(size) {}
		~MemoryHeader() = default;
	};

	inline bool IsPointerOwned(const void* ptr, const uptr start, const uptr end) {
		assert(ptr != nullptr && "Cannot check ownership of a nullptr!");
		if (!ptr) return false;

		const uptr objAddr = reinterpret_cast<uptr>(ptr);
		return (objAddr >= start && objAddr < end);
	}
}

#endif