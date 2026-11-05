/*
engine_aliases.h - spike libraries
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

#ifndef __ENGINE_ALIASES_H
#define __ENGINE_ALIASES_H

#include <cstdint>
#include <filesystem>
#include <memory>

#define UUID_SYSTEM_GENERATOR
#include <UUID/uuid.h>

/*
----------------------------------------------------------------------

File Summary:
	This file holds SpikeEngine aliases for engine data types and
	constants. It also contains some helper Macros for repetitive tasks.

----------------------------------------------------------------------
*/

using filepath = std::filesystem::path;

template<typename T>
using ref = std::shared_ptr<T>;

using uuid = uuids::uuid;

inline constexpr uuid kInvalidUuid		= uuids::uuid_namespace_max;
inline constexpr uint64_t kUuidSize		= sizeof(uint8_t) * 16;

inline constexpr uint32_t kChunkSizeS = 64;
inline constexpr uint32_t kChunkSize = 256;
inline constexpr uint32_t kLutGrowth = 256;

inline constexpr uint64_t kMegaByte = 1048576;
inline constexpr uint64_t kGigaByte = 1073741824;

#define FIND_IN_MAP(var, map, val) auto var = map.find(val); var != map.end()

#define COPY_TO_BUFF(var, buff, pos) std::memcpy(buff + pos, &var, sizeof(var)); pos += sizeof(var);
#define COPY_FROM_BUFF(var, buff, pos) std::memcpy(&var, buff + pos, sizeof(var)); pos += sizeof(var);

#endif