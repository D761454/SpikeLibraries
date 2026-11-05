#ifndef __ASSET_FILE_PARSER_COMMONS
#define __ASSET_FILE_PARSER_COMMONS

#include <list>

#include <spike/def_aliases.h>
#include <spike/engine_aliases.h>
#include <spike/podio.hpp>
#include <spike/stl_aliases.h>
#include <spike/warning.h>

namespace errlib { struct Error; }

namespace fileacc {
	enum CompressionMethod : u32 {
		afpCompressionMode_none = 0,	// raw text - YAML or uncompressed binary
		afpCompressionMode_zlib = 1,	// portable
		afpCompressionMode_lz4 = 2,		// v. fast + fast runtime
		afpCompressionMode_zstd = 3,	// best compromise speed / ratio - recommended for final builds
	};

	DISABLE_WARNING_PUSH
	DISABLE_WARNING_UNINITIALIZED_VARIABLE

	/* file header data used to verify file validity and type */
	struct FileCheck {
		/* denotes content structure for the file */
		u32 magic;
		/* determines fileacc version used for file creation */
		u32 version;
	};

	/* preferred method for header creation, zeroes all data in range, preventing leaking uninitialized data */
	inline FileCheck make_file_check(const u32 magic, const u32 version) noexcept {
		FileCheck hdr;
		podio::zero_init(hdr);
		hdr.magic = magic;
		hdr.version = version;
		return hdr;
	}

	/*
	returns:
	0 = good
	-1 = invalid magic
	-2 = invalid version
	*/
	inline i32 valid_file(FileCheck& header, const u32 expectedMagic, const u32 expectedVer) noexcept {
		if (header.magic != expectedMagic) return -1;
		if (header.version != expectedVer) return -2;
		return 0;
	}

	/*
	returns:
	0 = good
	-1 = invalid magic
	-2 = invalid version
	*/
	inline i32 valid_file(FileCheck& header, const FileCheck& expectedHeader) noexcept {
		if (header.magic != expectedHeader.magic) return -1;
		if (header.version != expectedHeader.version) return -2;
		return 0;
	}

	bool check_file(const string& path, const u32 expectedMagic, const u32 expectedVer) noexcept;
	bool check_file(const filepath& path, const u32 expectedMagic, const u32 expectedVer) noexcept;

	bool check_file(const string& path, const FileCheck& expectedHeader) noexcept;
	bool check_file(const filepath& path, const FileCheck& expectedHeader) noexcept;

	struct CompressedDataHeader {
		CompressionMethod compressionMethod;
		u32 crc32;
		u64 uncompressedSize;
		u64 compressedSize;
		u64 dataOffset;
	};

	inline CompressedDataHeader make_compressed_header(const CompressionMethod compMethod, const u32 crc32, const u64 rawSize, const u64 compressedSize, const u64 offset = 0) {
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

	/* return a hash value from a string of size len */
	inline u64 fnv1a64(const char* data, usize len) {
		const u64 fnvOffset = 14695981039346656037ULL;
		const u64 fnvPrime = 1099511628211ULL;
		u64 hash = fnvOffset;
		const u8* ptr = reinterpret_cast<const u8*>(data);
		for (usize i = 0; i < len; i++) {
			hash ^= static_cast<u64>(ptr[i]);
			hash *= fnvPrime;
		}
		return hash;
	}
}

#endif