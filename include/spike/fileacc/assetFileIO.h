#ifndef __ASSET_FILE_IO
#define __ASSET_FILE_IO

#include <fstream>

#include <spike/def_aliases.h>
#include <spike/error.hpp>
#include <spike/stl_aliases.h>
#include <spike/nivector.h>

#include "commons.hpp"

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