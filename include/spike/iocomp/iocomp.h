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

#ifndef __SPIKE_IOCOMP_IOCOMP_H
#define __SPIKE_IOCOMP_IOCOMP_H

#include <ostream>
#include <fstream>
#include <limits>

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"
#include "spike/podio.hpp"
#include "spike/warning.h"
#include "spike/nivector.h"

namespace iocomp {
	enum CompressionMethod_ : u32 {
		CompressionMethod_none = 0,	// raw text - YAML or uncompressed binary
		CompressionMethod_zlib = 1,	// portable
		CompressionMethod_lz4 = 2,		// v. fast + fast runtime
		CompressionMethod_zstd = 3,	// best compromise speed / ratio - recommended for final builds
	};
	
	DISABLE_WARNING_PUSH
	DISABLE_WARNING_UNINITIALIZED_VARIABLE
	
	struct CompressedDataHeader {
		CompressionMethod_ compressionMethod;
		u32 crc32;
		u64 uncompressedSize;
		u64 compressedSize;
		u64 dataOffset;
	};

	inline CompressedDataHeader make_compressed_header(const CompressionMethod_ compMethod, const u32 crc32, const u64 rawSize, const u64 compressedSize, const u64 offset = 0) {
		CompressedDataHeader cdh;
		podio::zero_init(cdh);
		cdh.compressionMethod = compMethod;
		cdh.crc32 = crc32;
		cdh.uncompressedSize = rawSize;
		cdh.compressedSize = compressedSize;
		cdh.dataOffset = offset;
		return cdh;
	}
	
	DISABLE_WARNING_POP
	
	class IOComp {
		struct ContextZSTD;
		static inline ContextZSTD* _zstdCtx = nullptr;

	public:
		static void Initialize();
		static void Shutdown();

		static bool ZSTD() { return _zstdCtx; }

		static usize Compress(const void* src, const usize& srcSize, niVector<u8>& dst, const CompressionMethod_& compressionMethod);

		static usize Compress(const void* src, const usize& srcSize, void* dst, const CompressionMethod_& compressionMethod);
		static usize Decompress(const void* src, const usize& srcSize, void* dst, const usize& expectedSize, const u32& expectedCrc32, const CompressionMethod_& compressionMethod);

		static usize DecompressToBuffer(std::ifstream& ifs, const u64& fileOffset, const usize& srcSize, void* dst, const usize& expectedSize, const u32& expectedCrc32, const CompressionMethod_& compressionMethod);

		template<typename T>
		static usize Decompress(const void* src, const usize& srcSize, niVector<T>& dst, const usize& expectedSize, const u32& expectedCrc32, const CompressionMethod_& compressionMethod) {
			usize realSize = expectedSize / sizeof(T);
			dst.resize(realSize);
			return Decompress(src, srcSize, reinterpret_cast<void*>(dst.data()), expectedSize, expectedCrc32, compressionMethod);
		}

		template<typename T, typename U>
		static usize Decompress(niVector<T>& src, niVector<U>& dst, const usize& expectedSize, const u32& expectedCrc32, const CompressionMethod_& compressionMethod) {
			usize realSourceSize = expectedSize / sizeof(T);
			usize realDestSize = expectedSize / sizeof(U);
			dst.resize(realDestSize);
			return Decompress(reinterpret_cast<void*>(src.data()), realSourceSize, reinterpret_cast<void*>(dst.data()), expectedSize, expectedCrc32, compressionMethod);
		}

		static u32 CalcCRC32(const u8* data, usize len);
		static const string CompMethodToString(const CompressionMethod_& cm);

		static bool CompressToFile(std::ostream& ofs, const void* source, const usize& srcSize, CompressionMethod_& compressionMethod) {
			if (compressionMethod == CompressionMethod_zstd) if (!ZSTD()) compressionMethod = CompressionMethod_zlib;

			niVector<u8> compressed;
			usize compressedSize = Compress(source, srcSize, compressed, compressionMethod);
			if (compressedSize == 0) return false;
			auto p = ofs.tellp();
			if (p == -1) return false;
			CompressedDataHeader cdh = make_compressed_header(compressionMethod, CalcCRC32(reinterpret_cast<const u8*>(source), srcSize), srcSize, compressedSize, static_cast<u64>(p) + sizeof(CompressedDataHeader));
			if (!podio::write_pod(ofs, cdh)) return false;
			ofs.write(reinterpret_cast<const char*>(compressed.data()), compressed.size());
			return ofs.good();
		}

		template<typename T>
		static bool CompressToFile(std::ostream& ofs, const T* source, CompressionMethod_& compressionMethod) {
			static_assert(podio::is_pod_writable_v<T>, "CompressToFile->write_pod requires trivially copyable, standard-layout type");
			if (compressionMethod == CompressionMethod_zstd) if (!ZSTD()) compressionMethod = CompressionMethod_zlib;

			niVector<u8> compressed;
			usize compressedSize = Compress(source, sizeof(T), compressed, compressionMethod);
			if (compressedSize == 0) return false;
			auto p = ofs.tellp();
			if (p == -1) return false;
			CompressedDataHeader cdh = make_compressed_header(compressionMethod, CalcCRC32(reinterpret_cast<const u8*>(source), sizeof(T)), sizeof(T), compressedSize, static_cast<u64>(p) + sizeof(CompressedDataHeader));
			if (!podio::write_pod(ofs, cdh)) return false;
			ofs.write(reinterpret_cast<const char*>(compressed.data()), compressed.size());
			return ofs.good();
		}

		template<typename T>
		static T* DecompressFromFile(std::ifstream& ifs) {
			static_assert(podio::is_pod_writable_v<T>, "DecompressFromFile->read_pod requires trivially copyable, standard-layout type");
			u64 remsize = 0;
			auto curr = ifs.tellg();
			if (curr == -1) return nullptr;
			ifs.seekg(0, std::ios::beg);
			auto start = ifs.tellg();
			if (start == -1) return nullptr;
			ifs.seekg(0, std::ios::end);
			auto end = ifs.tellg();
			if (end == -1) return nullptr;
			remsize = end - start;
			ifs.seekg(curr, std::ios::beg);

			CompressedDataHeader cdh;
			if (!podio::read_pod(ifs, cdh) || ifs.eof()) return nullptr;
			if (cdh.dataOffset > remsize || cdh.compressedSize > remsize - cdh.dataOffset) return nullptr;
			ifs.seekg(cdh.dataOffset);
			if (!ifs.good() || ifs.eof() || cdh.uncompressedSize != sizeof(T)) return nullptr;
			

			niVector<u8> compressed;
			if (cdh.compressedSize > std::numeric_limits<std::streamsize>::max() || cdh.compressedSize > compressed.max_size()) return nullptr;
			compressed.resize(cdh.compressedSize);
			ifs.read(reinterpret_cast<char*>(compressed.data()), cdh.compressedSize);
			if (ifs.gcount() != cdh.compressedSize) return nullptr;
			T* newObject = new T();
			if (Decompress(compressed.data(), cdh.compressedSize, newObject, cdh.uncompressedSize, cdh.crc32, cdh.compressionMethod) != cdh.uncompressedSize) {
				delete newObject;
				return nullptr;
			}
			return newObject;
		}

		static bool DecompressFromFile(std::ifstream& ifs, void* dst, const u64 capacity) {
			u64 remsize = 0;
			auto curr = ifs.tellg();
			if (curr == -1) return false;
			ifs.seekg(0, std::ios::beg);
			auto start = ifs.tellg();
			if (start == -1) return false;
			ifs.seekg(0, std::ios::end);
			auto end = ifs.tellg();
			if (end == -1) return false;
			remsize = end - start;
			ifs.seekg(curr, std::ios::beg);

			CompressedDataHeader cdh;
			if (!podio::read_pod(ifs, cdh) || ifs.eof()) return false;
			if (cdh.dataOffset > remsize || cdh.compressedSize > remsize - cdh.dataOffset) return false;
			ifs.seekg(cdh.dataOffset);
			if (!ifs.good() || ifs.eof() || cdh.uncompressedSize > capacity) return false;
			
			niVector<u8> compressed;
			if (cdh.compressedSize > std::numeric_limits<std::streamsize>::max() || cdh.compressedSize > compressed.max_size()) return false;
			compressed.resize(cdh.compressedSize);
			ifs.read(reinterpret_cast<char*>(compressed.data()), cdh.compressedSize);
			if (ifs.gcount() != cdh.compressedSize) return false;
			if (Decompress(compressed.data(), cdh.compressedSize, dst, cdh.uncompressedSize, cdh.crc32, cdh.compressionMethod) != cdh.uncompressedSize) {
				return false;
			}
			return true;
		}
	};
}

#endif