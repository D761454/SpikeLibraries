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

#include "assetFileIO.h"

#include <string.h>
#include <fstream>

#include <zlib/zlib.h>
#define ZSTD_STATIC_LINKING_ONLY
#include <zstd/zstd.h>
#include <zlib/zconf.h>

#include <spike/def_aliases.h>
#include <spike/nivector.h>
#include <spike/podio.hpp>
#include <spike/stl_aliases.h>
#include <spike/error.hpp>
#include <debug.hpp>

#include "commons.hpp"

u32 fileacc::AssetFileIO::CalcCRC32(const u8* data, size_t len) {
	return static_cast<u32>(crc32(0UL, reinterpret_cast<const Bytef*>(data), static_cast<uInt>(len)));
}

const string fileacc::AssetFileIO::CompMethodToString(const fileacc::CompressionMethod& cm) {
	switch (cm) {
	case fileacc::afpCompressionMode_none:  return "0: NONE";
	case fileacc::afpCompressionMode_zlib:  return "1: ZLIB";
	case fileacc::afpCompressionMode_lz4:   return "2: LZ4";
	case fileacc::afpCompressionMode_zstd:  return "3: ZSTD";
	default: return "UNKNOWN";
	}
}

struct fileacc::ContextZSTD {
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

void fileacc::AssetFileIO::Initialize() {
	if (_zstdCtx) return;

	_zstdCtx = new ContextZSTD();
	if (!_zstdCtx->ok()) {
		delete _zstdCtx;
		_zstdCtx = new ContextZSTD();
		if (!_zstdCtx->ok()) {
			delete _zstdCtx;
			_zstdCtx = nullptr;
			Debug::LogWarning("Failed to setup zstd context! Compression algorithm falling back to zlib!");
			// use zlib instead of zstd cant be initialized
		}
	}
}

void fileacc::AssetFileIO::Shutdown() {
	if (!_zstdCtx) return;

	delete _zstdCtx;
	_zstdCtx = nullptr;
}

usize fileacc::AssetFileIO::Compress(const void* src, const usize& srcSize, niVector<u8>& dst, const CompressionMethod& compressionMethod) {
	switch (compressionMethod) {
	case afpCompressionMode_zlib: {
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
	case afpCompressionMode_zstd: {
		usize bound = ZSTD_compressBound(srcSize);
		dst.resize(bound);
		usize compressedSize = _zstdCtx->Compress(src, srcSize, dst.data(), bound, 3);
		if (ZSTD_isError(compressedSize)) {
			dst.clear();
			string err(ZSTD_getErrorName(compressedSize));
			Debug::LogError("Zstd Compression Failed: {}", err);
			return 0;
		}
		dst.resize(compressedSize);
		return dst.size();
	}
	case afpCompressionMode_lz4:
	case afpCompressionMode_none:
	default: {
		dst.resize(srcSize);
		std::memcpy(dst.data(), src, srcSize);
		return srcSize;
	}
	}
}

usize fileacc::AssetFileIO::Compress(const void* src, const usize& srcSize, void* dst, const CompressionMethod& compressionMethod) {
	switch (compressionMethod) {
	case afpCompressionMode_zlib: {
		uLong srcLen = static_cast<uLong>(srcSize);
		uLongf bound = compressBound(srcLen);
		int res = compress(reinterpret_cast<Bytef*>(dst), &bound, reinterpret_cast<const Bytef*>(src), srcLen);
		if (res != Z_OK) {
          	Debug::LogError("Zlib Compression Failed: {}", res);
        	return 0;
        }
		return static_cast<usize>(bound);
	}
	case afpCompressionMode_zstd: {
		usize bound = ZSTD_compressBound(srcSize);
		usize compressedSize = _zstdCtx->Compress(src, srcSize, dst, bound, 3);
		if (ZSTD_isError(compressedSize)) {
			string err(ZSTD_getErrorName(compressedSize));
			Debug::LogError("Zstd Compression Failed: {}", err);
			return 0;
		}
		return compressedSize;
	}
	case afpCompressionMode_lz4:
	case afpCompressionMode_none:
	default: {
		std::memcpy(dst, src, srcSize);
		return srcSize;
	}
	}
}

usize fileacc::AssetFileIO::Decompress(const void* src, const usize& srcSize, void* dst, const usize& expectedSize, const u32& expectedCrc32, const CompressionMethod& compressionMethod) {
	switch (compressionMethod) {
	case afpCompressionMode_zlib: {
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
	case afpCompressionMode_zstd: {
		const usize rawSize = ZSTD_getFrameContentSize(src, srcSize);
		if (ZSTD_isError(rawSize)) {
			string err(ZSTD_getErrorName(rawSize));
			Debug::LogError("ZSTD_getFrameContentSize Failed: {}", err);
			return 0;
		}
		const usize accRawSize = _zstdCtx->Decompress(src, srcSize, dst, rawSize);
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
	case afpCompressionMode_lz4:
	case afpCompressionMode_none:
	default: {
		std::memcpy(dst, src, srcSize);
		return srcSize;
	}
	}
}

usize fileacc::AssetFileIO::DecompressToBuffer(std::ifstream& ifs, const u64& fileOffset, const usize& srcSize, void* dst, const usize& expectedSize, const u32& expectedCrc32, const CompressionMethod& compressionMethod) {
	switch (compressionMethod) {
	case afpCompressionMode_zlib: {
		niVector<u8> compressedData;
		compressedData.resize(srcSize);

		ifs.seekg(fileOffset, std::ios::beg);
		ifs.read(reinterpret_cast<char*>(compressedData.data()), srcSize);

		uLongf destLen = static_cast<uLongf>(expectedSize);
		int res = uncompress(reinterpret_cast<Bytef*>(dst), &destLen, reinterpret_cast<const Bytef*>(compressedData.data()), static_cast<uLong>(srcSize));
		if (res != Z_OK || static_cast<usize>(destLen) != expectedSize) {
			return 0;
		}
		u32 crc32 = CalcCRC32(reinterpret_cast<const u8*>(dst), expectedSize);
		if (crc32 != expectedCrc32) {
			Debug::LogError("Decompression error using zlib! CRC32 mismatch!");
			return 0;
		}
		return static_cast<usize>(destLen);
	}
	case afpCompressionMode_zstd: {
		niVector<u8> compressedData;
		compressedData.resize(srcSize);

		ifs.seekg(fileOffset, std::ios::beg);
		ifs.read(reinterpret_cast<char*>(compressedData.data()), srcSize);

		const usize res = _zstdCtx->Decompress(compressedData.data(), srcSize, dst, expectedSize);
		if (ZSTD_isError(res) || res != expectedSize) {
			string err(ZSTD_getErrorName(res));
			Debug::LogError("Zstd Decompression Failed: {}", err);
			return false;
		}
		u32 crc32 = CalcCRC32(reinterpret_cast<const u8*>(dst), expectedSize);
		if (crc32 != expectedCrc32) {
			Debug::LogError("Decompression error using zstd! CRC32 mismatch!");
			return 0;
		}
		return res;
	}
	case afpCompressionMode_lz4:
	case afpCompressionMode_none:
	default: {
		ifs.seekg(fileOffset, std::ios::beg);
		ifs.read(reinterpret_cast<char*>(dst), srcSize);
		return srcSize;
	}
	}
}