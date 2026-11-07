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

#ifndef __SPIKE_ASSET_FILE_IO
#define __SPIKE_ASSET_FILE_IO

#include <fstream>

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"
#include "spike/nivector.h"

#include "spike/fileparser/commons.hpp"

namespace fileacc {
	struct ContextZSTD;

	class AssetFileIO {
		static inline ContextZSTD* _zstdCtx = nullptr;

	public:
		static void Initialize();
		static void Shutdown();

		static bool ZSTD() { return _zstdCtx; }

		static usize Compress(const void* src, const usize& srcSize, niVector<u8>& dst, const CompressionMethod& compressionMethod);

		static usize Compress(const void* src, const usize& srcSize, void* dst, const CompressionMethod& compressionMethod);
		static usize Decompress(const void* src, const usize& srcSize, void* dst, const usize& expectedSize, const u32& expectedCrc32, const CompressionMethod& compressionMethod);

		static usize DecompressToBuffer(std::ifstream& ifs, const u64& fileOffset, const usize& srcSize, void* dst, const usize& expectedSize, const u32& expectedCrc32, const CompressionMethod& compressionMethod);

		template<typename T>
		static usize Decompress(const void* src, const usize& srcSize, niVector<T>& dst, const usize& expectedSize, const u32& expectedCrc32, const CompressionMethod& compressionMethod) {
			usize realSize = expectedSize / sizeof(T);
			dst.resize(realSize);
			return Decompress(src, srcSize, reinterpret_cast<void*>(dst.data()), expectedSize, expectedCrc32, compressionMethod);
		}

		template<typename T, typename U>
		static usize Decompress(niVector<T>& src, niVector<U>& dst, const usize& expectedSize, const u32& expectedCrc32, const CompressionMethod& compressionMethod) {
			usize realSourceSize = expectedSize / sizeof(T);
			usize realDestSize = expectedSize / sizeof(U);
			dst.resize(realDestSize);
			return Decompress(reinterpret_cast<void*>(src.data()), realSourceSize, reinterpret_cast<void*>(dst.data()), expectedSize, expectedCrc32, compressionMethod);
		}

		static u32 CalcCRC32(const u8* data, size_t len);
		static const string CompMethodToString(const fileacc::CompressionMethod& cm);
	};
}

#endif