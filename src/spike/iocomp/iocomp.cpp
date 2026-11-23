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

#include "spike/iocomp/iocomp.h"

#include <algorithm>
#include <cstring>
#include <limits>
#include <mutex>
#include <vector>

#include <zlib.h>
#define ZSTD_STATIC_LINKING_ONLY
#include <zstd.h>
#include <zstd_errors.h>

#include "spike/def_aliases.h"
#include "spike/nivector.h"

const u32 iocomp::crc32(const u8* data, usize len) {
	if (!data && len != 0) return 0;

	ulong crc = ::crc32(0L, Z_NULL, 0);
	while (len != 0) {
		const unsigned int chunkSize = static_cast<unsigned int>((std::min)(len, static_cast<usize>(std::numeric_limits<unsigned int>::max())));
		crc = ::crc32(crc, reinterpret_cast<const byte*>(data), chunkSize);
		data += chunkSize;
		len -= chunkSize;
	}
	return static_cast<u32>(crc);
}

struct iocomp::IOComp::ContextZSTD {
	ContextZSTD() : p_cctx(ZSTD_createCCtx()), p_dctx(ZSTD_createDCtx()) {
		if (p_cctx && p_dctx) {
			_ready = !ZSTD_isError(ZSTD_CCtx_setParameter(p_cctx, ZSTD_c_compressionLevel, 3))
				&& !ZSTD_isError(ZSTD_CCtx_setParameter(p_cctx, ZSTD_c_checksumFlag, 1));
		}
	}

	~ContextZSTD() {
		if (p_cctx) ZSTD_freeCCtx(p_cctx);
		if (p_dctx) ZSTD_freeDCtx(p_dctx);
	}

	usize Compress(const void* src, usize srcSize, void* dst, usize dstSize, i32 level) const {
		if (!_ready) return ZSTD_error_memory_allocation;
		std::lock_guard<std::mutex> lock(_mutex);
		return ZSTD_compressCCtx(p_cctx, dst, dstSize, src, srcSize, level);
	}

	usize Decompress(const void* src, usize srcSize, void* dst, usize dstSize) const {
		if (!_ready) return ZSTD_error_memory_allocation;
		std::lock_guard<std::mutex> lock(_mutex);
		return ZSTD_decompressDCtx(p_dctx, dst, dstSize, src, srcSize);
	}

	bool ok() const noexcept { return _ready; }

private:
	ZSTD_CCtx* const p_cctx;
	ZSTD_DCtx* const p_dctx;
	bool _ready = false;
	mutable std::mutex _mutex;
};

iocomp::IOComp::ContextZSTD iocomp::IOComp::_zstdCtx = iocomp::IOComp::ContextZSTD();

bool iocomp::IOComp::ZSTD() {
	return _zstdCtx.ok();
}

u64 iocomp::IOComp::Compress(const void* src, const u64& srcSize, niVector<u8>& dst, const CompressionMethod_& compressionMethod) {
	switch (compressionMethod) {
	case CompressionMethod_zlib: {
		uLong srcLen = static_cast<uLong>(srcSize);
		uLongf bound = compressBound(srcLen);
		dst.resize(static_cast<usize>(bound));
		int res = compress(dst.data(), &bound, reinterpret_cast<const Bytef*>(src), srcLen);
		if (res != Z_OK) {
			dst.clear();
			return 0;
		}
		dst.resize(static_cast<usize>(bound));
		return dst.size();
	}
	case CompressionMethod_zstd: {
		usize bound = ZSTD_compressBound(static_cast<usize>(srcSize));
		dst.resize(bound);
		usize compressedSize = _zstdCtx.Compress(src, static_cast<usize>(srcSize), dst.data(), bound, 3);
		if (ZSTD_isError(compressedSize)) {
			dst.clear();
			string err(ZSTD_getErrorName(compressedSize));
			return 0;
		}
		dst.resize(compressedSize);
		return dst.size();
	}
	case CompressionMethod_lz4:
	case CompressionMethod_none:
	default: {
		dst.resize(static_cast<usize>(srcSize));
		std::memcpy(dst.data(), src, static_cast<usize>(srcSize));
		return srcSize;
	}
	}
}

u64 iocomp::IOComp::Compress(const void* src, const u64& srcSize, void* dst, const CompressionMethod_& compressionMethod) {
	switch (compressionMethod) {
	case CompressionMethod_zlib: {
		uLong srcLen = static_cast<uLong>(srcSize);
		uLongf bound = compressBound(srcLen);
		int res = compress(reinterpret_cast<Bytef*>(dst), &bound, reinterpret_cast<const Bytef*>(src), srcLen);
		if (res != Z_OK) {
        	return 0;
        }
		return static_cast<usize>(bound);
	}
	case CompressionMethod_zstd: {
		usize bound = ZSTD_compressBound(static_cast<usize>(srcSize));
		usize compressedSize = _zstdCtx.Compress(src, static_cast<usize>(srcSize), dst, bound, 3);
		if (ZSTD_isError(compressedSize)) {
			string err(ZSTD_getErrorName(compressedSize));
			return 0;
		}
		return compressedSize;
	}
	case CompressionMethod_lz4:
	case CompressionMethod_none:
	default: {
		std::memcpy(dst, src, static_cast<usize>(srcSize));
		return srcSize;
	}
	}
}


u64 iocomp::IOComp::Decompress(const void* src, const u64& srcSize, void* dst, const u64& expectedSize, const u32& expectedCrc32, const CompressionMethod_& compressionMethod) {
	switch (compressionMethod) {
	case CompressionMethod_zlib: {
		uLongf destLen = static_cast<uLongf>(expectedSize);
		int res = uncompress(reinterpret_cast<Bytef*>(dst), &destLen, reinterpret_cast<const Bytef*>(src), static_cast<uLong>(srcSize));
		if (res != Z_OK || static_cast<usize>(destLen) != expectedSize) {
			return 0;
		}
		const u32 crc = crc32(reinterpret_cast<const u8*>(dst), static_cast<usize>(destLen));
		if (crc != expectedCrc32) {
			return 0;
		}
		return static_cast<usize>(destLen);
	}
	case CompressionMethod_zstd: {
		const usize rawSize = ZSTD_getFrameContentSize(src, static_cast<usize>(srcSize));
		if (ZSTD_isError(rawSize)) {
			string err(ZSTD_getErrorName(rawSize));
			return 0;
		}
		const usize accRawSize = _zstdCtx.Decompress(src, static_cast<usize>(srcSize), dst, rawSize);
		if (ZSTD_isError(accRawSize) || accRawSize != static_cast<usize>(expectedSize)) {
			string err(ZSTD_getErrorName(accRawSize));
			return false;
		}
		const u32 crc = crc32(reinterpret_cast<const u8*>(dst), accRawSize);
		if (crc != expectedCrc32) {
			return 0;
		}
		return accRawSize;
	}
	case CompressionMethod_lz4:
	case CompressionMethod_none:
	default: {
		if (srcSize != expectedSize) return 0;
		const u32 crc = crc32(reinterpret_cast<const u8*>(src), static_cast<usize>(srcSize));
		if (crc != expectedCrc32) {
			return 0;
		}
		std::memcpy(dst, src, static_cast<usize>(srcSize));
		return srcSize;
	}
	}
}