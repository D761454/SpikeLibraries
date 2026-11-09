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

#include <string.h>
#include <fstream>

#include <zlib.h>
#include <zconf.h>
#define ZSTD_STATIC_LINKING_ONLY
#include <zstd.h>

#include "spike/def_aliases.h"
#include "spike/nivector.h"
#include "spike/stl_aliases.h"
#include "spike/debugl/debug.hpp"

u32 iocomp::IOComp::CalcCRC32(const u8* data, usize len) {
	return static_cast<u32>(crc32(0UL, reinterpret_cast<const Bytef*>(data), static_cast<uInt>(len)));
}

const string iocomp::IOComp::CompMethodToString(const CompressionMethod_& cm) {
	switch (cm) {
	case CompressionMethod_none:  return "0: NONE";
	case CompressionMethod_zlib:  return "1: ZLIB";
	case CompressionMethod_lz4:   return "2: LZ4";
	case CompressionMethod_zstd:  return "3: ZSTD";
	default: return "UNKNOWN";
	}
}

struct iocomp::IOComp::ContextZSTD {
	ContextZSTD() : p_cctx(ZSTD_createCCtx()), p_dctx(ZSTD_createDCtx()) {
		if (!p_cctx) return;
		ZSTD_CCtx_setParameter(p_cctx, ZSTD_c_compressionLevel, 3);
		ZSTD_CCtx_setParameter(p_cctx, ZSTD_c_checksumFlag, 1);
	}

	~ContextZSTD() {
		if (p_cctx) ZSTD_freeCCtx(p_cctx);
		if (p_dctx) ZSTD_freeDCtx(p_dctx);
	}

	usize Compress(const void* src, const usize srcSize, void* dst, const usize dstSize, const i32 level) const {
		return ZSTD_compressCCtx(p_cctx, dst, dstSize, src, srcSize, level);
	}

	usize Decompress(const void* src, const usize srcSize, void* dst, const usize dstSize) const {
		return ZSTD_decompressDCtx(p_dctx, dst, dstSize, src, srcSize);
	}

	bool ok() const {
		return p_cctx && p_dctx;
	}

private:
	ZSTD_CCtx* const p_cctx;
	ZSTD_DCtx* const p_dctx;
};

iocomp::IOComp::ContextZSTD iocomp::IOComp::_zstdCtx = iocomp::IOComp::ContextZSTD();

bool iocomp::IOComp::ZSTD() {
	return _zstdCtx.ok();
}


usize iocomp::IOComp::Compress(const void* src, const usize& srcSize, niVector<u8>& dst, const CompressionMethod_& compressionMethod) {
	switch (compressionMethod) {
	case CompressionMethod_zlib: {
		uLong srcLen = static_cast<uLong>(srcSize);
		uLongf bound = compressBound(srcLen);
		dst.resize(static_cast<usize>(bound));
		int res = compress(dst.data(), &bound, reinterpret_cast<const Bytef*>(src), srcLen);
		if (res != Z_OK) {
			dst.clear();
			Debug::LogError("Zlib Compression Failed: {}", res);
			return 0;
		}
		dst.resize(static_cast<usize>(bound));
		return dst.size();
	}
	case CompressionMethod_zstd: {
		usize bound = ZSTD_compressBound(srcSize);
		dst.resize(bound);
		usize compressedSize = _zstdCtx.Compress(src, srcSize, dst.data(), bound, 3);
		if (ZSTD_isError(compressedSize)) {
			dst.clear();
			string err(ZSTD_getErrorName(compressedSize));
			Debug::LogError("Zstd Compression Failed: {}", err);
			return 0;
		}
		dst.resize(compressedSize);
		return dst.size();
	}
	case CompressionMethod_lz4:
	case CompressionMethod_none:
	default: {
		dst.resize(srcSize);
		std::memcpy(dst.data(), src, srcSize);
		return srcSize;
	}
	}
}

usize iocomp::IOComp::Compress(const void* src, const usize& srcSize, void* dst, const CompressionMethod_& compressionMethod) {
	switch (compressionMethod) {
	case CompressionMethod_zlib: {
		uLong srcLen = static_cast<uLong>(srcSize);
		uLongf bound = compressBound(srcLen);
		int res = compress(reinterpret_cast<Bytef*>(dst), &bound, reinterpret_cast<const Bytef*>(src), srcLen);
		if (res != Z_OK) {
          	Debug::LogError("Zlib Compression Failed: {}", res);
        	return 0;
        }
		return static_cast<usize>(bound);
	}
	case CompressionMethod_zstd: {
		usize bound = ZSTD_compressBound(srcSize);
		usize compressedSize = _zstdCtx.Compress(src, srcSize, dst, bound, 3);
		if (ZSTD_isError(compressedSize)) {
			string err(ZSTD_getErrorName(compressedSize));
			Debug::LogError("Zstd Compression Failed: {}", err);
			return 0;
		}
		return compressedSize;
	}
	case CompressionMethod_lz4:
	case CompressionMethod_none:
	default: {
		std::memcpy(dst, src, srcSize);
		return srcSize;
	}
	}
}


usize iocomp::IOComp::Decompress(const void* src, const usize& srcSize, void* dst, const usize& expectedSize, const u32& expectedCrc32, const CompressionMethod_& compressionMethod) {
	switch (compressionMethod) {
	case CompressionMethod_zlib: {
		uLongf destLen = static_cast<uLongf>(expectedSize);
		int res = uncompress(reinterpret_cast<Bytef*>(dst), &destLen, reinterpret_cast<const Bytef*>(src), static_cast<uLong>(srcSize));
		if (res != Z_OK || static_cast<usize>(destLen) != expectedSize) {
			return 0;
		}
		u32 crc32 = CalcCRC32(reinterpret_cast<const u8*>(dst), static_cast<usize>(destLen));
		if (crc32 != expectedCrc32) {
			Debug::LogError("Decompression error using zlib! CRC32 mismatch!");
			return 0;
		}
		return static_cast<usize>(destLen);
	}
	case CompressionMethod_zstd: {
		const usize rawSize = ZSTD_getFrameContentSize(src, srcSize);
		if (ZSTD_isError(rawSize)) {
			string err(ZSTD_getErrorName(rawSize));
			Debug::LogError("ZSTD_getFrameContentSize Failed: {}", err);
			return 0;
		}
		const usize accRawSize = _zstdCtx.Decompress(src, srcSize, dst, rawSize);
		if (ZSTD_isError(accRawSize) || accRawSize != expectedSize) {
			string err(ZSTD_getErrorName(accRawSize));
			Debug::LogError("Zstd Decompression Failed: {}", err);
			return false;
		}
		u32 crc32 = CalcCRC32(reinterpret_cast<const u8*>(dst), accRawSize);
		if (crc32 != expectedCrc32) {
			Debug::LogError("Decompression error using zstd! CRC32 mismatch!");
			return 0;
		}
		return accRawSize;
	}
	case CompressionMethod_lz4:
	case CompressionMethod_none:
	default: {
		if (srcSize != expectedSize) return 0;
		u32 crc32 = CalcCRC32(reinterpret_cast<const u8*>(src), srcSize);
		if (crc32 != expectedCrc32) {
			Debug::LogError("Decompression error using lz4 or none! CRC32 mismatch!");
			return 0;
		}
		std::memcpy(dst, src, srcSize);
		return srcSize;
	}
	}
}