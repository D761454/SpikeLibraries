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

/*
----------------------------------------------------------------------

File Summary:
	Holds the methoda and auxilary data required to allow users to 
	compress and decompress data to/from buffers and files.

----------------------------------------------------------------------
*/

#include <ostream>
#include <fstream>
#include <limits>
#include <string>

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"
#include "spike/podio.hpp"
#include "spike/nivector.h"

namespace iocomp {
	enum CompressionMethod_ : u32 {
		CompressionMethod_none = 0,	// raw text - YAML or uncompressed binary
		CompressionMethod_zlib = 1,	// portable
		CompressionMethod_lz4 = 2,	// reserved; currently unsupported
		CompressionMethod_zstd = 3	// best compromise speed / ratio - recommended for final builds
	};

	inline std::string to_string(CompressionMethod_ method) {
		switch (method) {
		case CompressionMethod_none: return "NONE";
		case CompressionMethod_zlib: return "ZLIB";
		case CompressionMethod_lz4: return "LZ4";
		case CompressionMethod_zstd: return "ZSTD";
		default: return "UNKNOWN";
		}
	}

	/// <summary>
	/// Calculates a crc32 value from a data buffer
	/// </summary>
	/// <param name="data">pointer to the byte buffer to calculate for</param>
	/// <param name="len">length of the byte buffer</param>
	/// <returns>crc32 value corresponding with the buffer</returns>
	const u32 crc32(const u8* data, usize len);
	
	/// <summary>
	/// Struct used to hold relevant data used to decompress and validate decompression of a compressed data blob
	/// </summary>
	struct CompressedDataHeader {
		CompressionMethod_ compressionMethod;
		u32 crc32;
		u64 uncompressedSize;
		u64 compressedSize;
		u64 dataOffset;

		CompressedDataHeader() noexcept :
			compressionMethod(CompressionMethod_none),
			crc32(0),
			uncompressedSize(0),
			compressedSize(0),
			dataOffset(0) {}

		CompressedDataHeader(const CompressionMethod_ compMethod, const u32 crc32, const u64 rawSize, const u64 compressedSize, const u64 offset = 0) noexcept :
			compressionMethod(compMethod),
			crc32(crc32),
			uncompressedSize(rawSize),
			compressedSize(compressedSize),
			dataOffset(offset) {}
	};
	
	/// <summary>
	/// Holding static class, contains methods for compression and decompressions using various compression algorithms
	/// </summary>
	class IOComp {
		struct ContextZSTD;
		static ContextZSTD _zstdCtx;

	public:
		/// <summary>
		/// Used to check if zstd is available for use
		/// </summary>
		/// <returns>if zstd was able to be successfully initialised</returns>
		static bool ZSTD();

		/// <summary>
		/// Compresses a chunk of memory to a buffer using the corresponding compression method
		/// </summary>
		/// <param name="src">pointer to the data to compress</param>
		/// <param name="srcSize">size of the data to compress (in bytes)</param>
		/// <param name="dst">pointer to the destination to place the compressed data</param>
		/// <param name="compressionMethod">compression method to use on the data</param>
		/// <returns>size of the compressed data</returns>
		static u64 Compress(const void* src, const u64& srcSize, void* dst, const CompressionMethod_& compressionMethod);

		/// <summary>
		/// Compresses a chunk of memory to a byte array using the corresponding compression method
		/// </summary>
		/// <param name="src">pointer to the data to compress</param>
		/// <param name="srcSize">size of the data to compress (in bytes)</param>
		/// <param name="dst">byte array to hold the compressed data</param>
		/// <param name="compressionMethod">compression method to use on the data</param>
		/// <returns>size of the compressed data</returns>
		static u64 Compress(const void* src, const u64& srcSize, niVector<u8>& dst, const CompressionMethod_& compressionMethod);

		/// <summary>
		/// Decompresses a compressed chunk of data to a buffer
		/// </summary>
		/// <param name="src">pointer to the compressed data</param>
		/// <param name="srcSize">size of the compressed data (in bytes)</param>
		/// <param name="dst">pointer to the buffer to store the decompressed data</param>
		/// <param name="expectedSize">expected size of the decompressed data (in bytes)</param>
		/// <param name="expectedCrc32">expected crc32 for the decompressed data, used for corruption detection</param>
		/// <param name="compressionMethod">compression method used to decompress the data, should be the same used to compress the data</param>
		/// <returns>size of the decompressed data</returns>
		static u64 Decompress(const void* src, const u64& srcSize, void* dst, const u64& expectedSize, const u32& expectedCrc32, const CompressionMethod_& compressionMethod);

		/// <summary>
		/// Decompresses a compressed chunk of data to an array of objects of type T, such as compressed vectors of structs
		/// </summary>
		/// <typeparam name="T">type of object stored in the output vector</typeparam>
		/// <param name="src">pointer to the compressed data</param>
		/// <param name="srcSize">size of the compressed data (in bytes)</param>
		/// <param name="dst">output vector holding objects of type T</param>
		/// <param name="expectedSize">expected size of the decompressed data (in bytes)</param>
		/// <param name="expectedCrc32">expected crc32 for the decompressed data, used for corruption detection</param>
		/// <param name="compressionMethod">compression method used to decompress the data, should be the same used to compress the data</param>
		/// <returns>size of the decompressed data</returns>
		template<typename T>
		static u64 Decompress(const void* src, const u64& srcSize, niVector<T>& dst, const u64& expectedSize, const u32& expectedCrc32, const CompressionMethod_& compressionMethod) {
			usize realSize = expectedSize / sizeof(T);
			dst.resize(realSize);
			return Decompress(src, srcSize, reinterpret_cast<void*>(dst.data()), expectedSize, expectedCrc32, compressionMethod);
		}
	};

	namespace internal {
		inline bool is_supported_method(CompressionMethod_ method) noexcept {
			return method == CompressionMethod_none || method == CompressionMethod_zlib || method == CompressionMethod_zstd;
		}

		class InputPositionGuard {
		public:
			explicit InputPositionGuard(std::ifstream& stream) : _stream(stream), _position(stream.tellg()) {}
			~InputPositionGuard() { if (_active && _position != std::streampos(-1)) restore(_stream, _position); }

			bool valid() const noexcept { return _position != std::streampos(-1); }
			std::streampos position() const noexcept { return _position; }
			void set_restore_position(std::streampos position) noexcept { _position = position; }
			bool restore_position() noexcept {
				if (_position == std::streampos(-1) || !restore(_stream, _position)) return false;
				_active = false;
				return true;
			}
			void commit() noexcept { _active = false; }

		private:
			static bool restore(std::ifstream& stream, std::streampos position) noexcept {
				try {
					stream.clear();
					stream.seekg(position);
					return stream.good();
				}
				catch (const std::ios_base::failure&) {
					return false;
				}
			}

			std::ifstream& _stream;
			std::streampos _position;
			bool _active = true;
		};

		inline bool file_size(std::ifstream& stream, std::streampos restorePosition, u64& size) {
			stream.clear();
			stream.seekg(0, std::ios::end);
			if (!stream.good()) return false;
			const std::streampos end = stream.tellg();
			if (end == std::streampos(-1)) return false;
			const std::streamoff endOffset = static_cast<std::streamoff>(end);
			if (endOffset < 0) return false;
			size = static_cast<u64>(endOffset);
			stream.clear();
			stream.seekg(restorePosition);
			return stream.good();
		}

		inline bool read_header(std::ifstream& stream, CompressedDataHeader& header, std::streampos& afterHeader, u64& fileSize) {
			if (!stream.good() || !podio::read_pod(stream, header)) return false;
			afterHeader = stream.tellg();
			if (afterHeader == std::streampos(-1)) return false;
			return file_size(stream, afterHeader, fileSize);
		}

		inline bool validate_payload(const CompressedDataHeader& header, u64 fileSize, u64 minimumOffset) noexcept {
			return is_supported_method(header.compressionMethod) && header.compressedSize > 0 && header.uncompressedSize > 0
				&& header.dataOffset >= minimumOffset && header.dataOffset <= fileSize
				&& header.compressedSize <= fileSize - header.dataOffset
				&& header.dataOffset <= static_cast<u64>(std::numeric_limits<std::streamoff>::max())
				&& header.compressedSize <= static_cast<u64>(std::numeric_limits<std::streamsize>::max());
		}

		inline bool read_payload(std::ifstream& stream, const CompressedDataHeader& header, u64 fileSize, u64 minimumOffset, niVector<u8>& compressed) {
			if (!validate_payload(header, fileSize, minimumOffset) || header.compressedSize > compressed.max_size()) return false;

			stream.clear();
			stream.seekg(static_cast<std::streamoff>(header.dataOffset), std::ios::beg);
			if (!stream.good()) return false;
			compressed.resize(static_cast<usize>(header.compressedSize));
			stream.read(reinterpret_cast<char*>(compressed.data()), static_cast<std::streamsize>(header.compressedSize));
			return stream.good() && static_cast<u64>(stream.gcount()) == header.compressedSize;
		}

		inline bool seek_payload_end(std::ifstream& stream, const CompressedDataHeader& header, u64 fileSize, u64 minimumOffset) {
			if (!validate_payload(header, fileSize, minimumOffset)) return false;
			const u64 endOffset = header.dataOffset + header.compressedSize;
			if (endOffset > static_cast<u64>(std::numeric_limits<std::streamoff>::max())) return false;
			stream.clear();
			stream.seekg(static_cast<std::streamoff>(endOffset), std::ios::beg);
			return stream.good();
		}
	}

	/// <summary>
	/// Used to compress a chunk of data and serialize it to file, where the appended data is [header][compressed blob]
	/// </summary>
	/// <param name="ofs">file stream to the file</param>
	/// <param name="source">pointer to the data to compress</param>
	/// <param name="srcSize">size of the data to compress (in bytes)</param>
	/// <param name="compressionMethod">compression method to use on the data</param>
	/// <returns>if the write was successful</returns>
	[[nodiscard]] static bool compress_to_file(std::ostream& ofs, const void* source, const usize& srcSize, CompressionMethod_& compressionMethod) {
		if (compressionMethod == CompressionMethod_zstd) if (!IOComp::ZSTD()) compressionMethod = CompressionMethod_zlib;

		niVector<u8> compressed;
		usize compressedSize = IOComp::Compress(source, srcSize, compressed, compressionMethod);
		if (compressedSize == 0) return false;
		auto p = ofs.tellp();
		if (p == -1) return false;
		CompressedDataHeader cdh(compressionMethod, crc32(reinterpret_cast<const u8*>(source), srcSize), srcSize, compressedSize, static_cast<u64>(p) + sizeof(CompressedDataHeader));
		if (!podio::write_pod(ofs, cdh)) return false;
		ofs.write(reinterpret_cast<const char*>(compressed.data()), compressed.size());
		return ofs.good();
	}

	/// <summary>
	/// Used to compress a chunk of data and serialize it to file, where the appended data is [header][compressed blob]
	/// </summary>
	/// <typeparam name="T">object type to compress and serialize</typeparam>
	/// <param name="ofs">file stream to the file</param>
	/// <param name="source">pointer to the data to compress</param>
	/// <param name="compressionMethod">compression method to use on the data</param>
	/// <returns>if the write was successful</returns>
	template<typename T>
	[[nodiscard]] static bool compress_to_file(std::ostream& ofs, const T* source, CompressionMethod_& compressionMethod) {
		static_assert(podio::is_pod_writable_v<T>, "CompressToFile->write_pod requires trivially copyable, standard-layout type");
		if (compressionMethod == CompressionMethod_zstd) if (!IOComp::ZSTD()) compressionMethod = CompressionMethod_zlib;

		niVector<u8> compressed;
		usize compressedSize = IOComp::Compress(source, sizeof(T), compressed, compressionMethod);
		if (compressedSize == 0) return false;
		auto p = ofs.tellp();
		if (p == -1) return false;
		CompressedDataHeader cdh(compressionMethod, crc32(reinterpret_cast<const u8*>(source), sizeof(T)), sizeof(T), compressedSize, static_cast<u64>(p) + sizeof(CompressedDataHeader));
		if (!podio::write_pod(ofs, cdh)) return false;
		ofs.write(reinterpret_cast<const char*>(compressed.data()), compressed.size());
		return ofs.good();
	}

	/// <summary>
	/// Used to decompress a chunk of data from file
	/// </summary>
	/// <typeparam name="T">type of object that should be created</typeparam>
	/// <param name="ifs">file stream to the file</param>
	/// <param name="seekBackToheader">whether to seek back to the file position just after the header or to remain after the compressed blob</param>
	/// <returns>object of type T if successful, else nullptr</returns>
	template<typename T>
	[[nodiscard]] static T* decompress_from_file(std::ifstream& ifs, bool seekBackToheader = false) {
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
		auto afterheader = ifs.tellg();
		if (afterheader == -1) return nullptr;
		if (cdh.dataOffset > remsize || cdh.compressedSize > remsize - cdh.dataOffset) return nullptr;
		ifs.seekg(cdh.dataOffset);
		if (!ifs.good() || ifs.eof() || cdh.uncompressedSize != sizeof(T)) return nullptr;


		niVector<u8> compressed;
		if (cdh.compressedSize > std::numeric_limits<std::streamsize>::max() || cdh.compressedSize > compressed.max_size()) return nullptr;
		compressed.resize(cdh.compressedSize);
		ifs.read(reinterpret_cast<char*>(compressed.data()), cdh.compressedSize);
		if (ifs.gcount() != cdh.compressedSize) return nullptr;
		T* newObject = new T();
		if (IOComp::Decompress(compressed.data(), cdh.compressedSize, newObject, cdh.uncompressedSize, cdh.crc32, cdh.compressionMethod) != cdh.uncompressedSize) {
			delete newObject;
			newObject = nullptr;
		}

		if (seekBackToheader) {
			ifs.seekg(afterheader, std::ios::beg);
		}

		return newObject;
	}

	/// <summary>
	/// Used to decompress a chunk of data from file
	/// </summary>
	/// <param name="ifs">file stream to the file</param>
	/// <param name="dst">pointer to the buffer to store the decompressed data</param>
	/// <param name="capacity">size of the destination buffer</param>
	/// <param name="seekBackToheader">whether to seek back to the file position just after the header or to remain after the compressed blob</param>
	/// <returns>if the read and decompression were successful</returns>
	[[nodiscard]] static bool decompress_from_file(std::ifstream& ifs, void* dst, const u64 capacity, bool seekBackToheader = false) {
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
		auto afterheader = ifs.tellg();
		if (afterheader == -1) return false;
		if (cdh.dataOffset > remsize || cdh.compressedSize > remsize - cdh.dataOffset) return false;
		ifs.seekg(cdh.dataOffset);
		if (!ifs.good() || ifs.eof() || cdh.uncompressedSize > capacity) return false;

		niVector<u8> compressed;
		if (cdh.compressedSize > std::numeric_limits<std::streamsize>::max() || cdh.compressedSize > compressed.max_size()) return false;
		compressed.resize(cdh.compressedSize);
		ifs.read(reinterpret_cast<char*>(compressed.data()), cdh.compressedSize);
		if (ifs.gcount() != cdh.compressedSize) return false;
		if (IOComp::Decompress(compressed.data(), cdh.compressedSize, dst, cdh.uncompressedSize, cdh.crc32, cdh.compressionMethod) != cdh.uncompressedSize) {
			if (seekBackToheader) {
				ifs.seekg(afterheader, std::ios::beg);
			}
			return false;
		}
		if (seekBackToheader) {
			ifs.seekg(afterheader, std::ios::beg);
		}
		return true;
	}

	/// <summary>
	/// Used to decompress a chunk of data from file
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="ifs">file stream to the file</param>
	/// <param name="dst">object vector used to store the decompressed output</param>
	/// <param name="seekBackToheader">whether to seek back to the file position just after the header or to remain after the compressed blob</param>
	/// <returns>if the read and decompression were successful</returns>
	template<typename T>
	[[nodiscard]] static bool decompress_from_file(std::ifstream& ifs, niVector<T>& dst, bool seekBackToheader = false) {
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
		auto afterheader = ifs.tellg();
		if (afterheader == -1) return false;
		if (cdh.dataOffset > remsize || cdh.compressedSize > remsize - cdh.dataOffset) return false;
		ifs.seekg(cdh.dataOffset);
		if (!ifs.good() || ifs.eof()) return false;

		niVector<u8> compressed;
		if (cdh.compressedSize > std::numeric_limits<std::streamsize>::max() || cdh.compressedSize > compressed.max_size()) return false;
		compressed.resize(cdh.compressedSize);
		ifs.read(reinterpret_cast<char*>(compressed.data()), cdh.compressedSize);
		if (ifs.gcount() != cdh.compressedSize) return false;
		if (IOComp::Decompress(compressed.data(), cdh.compressedSize, dst, cdh.uncompressedSize, cdh.crc32, cdh.compressionMethod) != cdh.uncompressedSize) {
			if (seekBackToheader) {
				ifs.seekg(afterheader, std::ios::beg);
			}
			return false;
		}
		if (seekBackToheader) {
			ifs.seekg(afterheader, std::ios::beg);
		}
		return true;
	}

	/// <summary>
	/// Used to decompress a chunk of data from file, given the compressed data header for the relevant blob
	/// </summary>
	/// <param name="ifs">file stream to the file</param>
	/// <param name="dataHeader">header containing all required information to decompress the associated blob</param>
	/// <param name="dst">pointer to the buffer to store the decompressed data</param>
	/// <returns>if the read and decompression were successful</returns>
	[[nodiscard]] static bool decompress_from_file(std::ifstream& ifs, const CompressedDataHeader& dataHeader, void* dst) {
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

		if (dataHeader.dataOffset > remsize || dataHeader.compressedSize > remsize - dataHeader.dataOffset) return false;
		ifs.seekg(dataHeader.dataOffset);
		if (!ifs.good() || ifs.eof()) return false;

		niVector<u8> compressed;
		if (dataHeader.compressedSize > std::numeric_limits<std::streamsize>::max() || dataHeader.compressedSize > compressed.max_size()) return false;
		compressed.resize(dataHeader.compressedSize);
		ifs.read(reinterpret_cast<char*>(compressed.data()), dataHeader.compressedSize);
		if (ifs.gcount() != dataHeader.compressedSize) return false;
		if (IOComp::Decompress(compressed.data(), dataHeader.compressedSize, dst, dataHeader.uncompressedSize, dataHeader.crc32, dataHeader.compressionMethod) != dataHeader.uncompressedSize) {
			return false;
		}
		return true;
	}

	/// <summary>
	/// Used to decompress a chunk of data from file, given the compressed data header for the relevant blob
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="ifs">file stream to the file</param>
	/// <param name="dataHeader">header containing all required information to decompress the associated blob</param>
	/// <param name="dst">object vector used to store the decompressed output</param>
	/// <returns>if the read and decompression were successful</returns>
	template<typename T>
	[[nodiscard]] static bool decompress_from_file(std::ifstream& ifs, const CompressedDataHeader& dataHeader, niVector<T>& dst) {
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

		if (dataHeader.dataOffset > remsize || dataHeader.compressedSize > remsize - dataHeader.dataOffset) return false;
		ifs.seekg(dataHeader.dataOffset);
		if (!ifs.good() || ifs.eof()) return false;

		niVector<u8> compressed;
		if (dataHeader.compressedSize > std::numeric_limits<std::streamsize>::max() || dataHeader.compressedSize > compressed.max_size()) return false;
		compressed.resize(dataHeader.compressedSize);
		ifs.read(reinterpret_cast<char*>(compressed.data()), dataHeader.compressedSize);
		if (ifs.gcount() != dataHeader.compressedSize) return false;
		if (IOComp::Decompress(compressed.data(), dataHeader.compressedSize, dst, dataHeader.uncompressedSize, dataHeader.crc32, dataHeader.compressionMethod) != dataHeader.uncompressedSize) {
			return false;
		}
		return true;
	}

	/// <summary>
	/// The method allows the user to seek ahead within a file when compressed data headers and compressed data blobs are located next to one another in sequence
	/// </summary>
	/// <param name="ifs">file stream to the file</param>
	/// <returns>if the seek ahead was successful</returns>
	static bool seek_ahead_in_file(std::ifstream& ifs) {
		internal::InputPositionGuard position(ifs);
		if (!position.valid()) return false;

		CompressedDataHeader header;
		std::streampos afterHeader;
		u64 fileSize;
		if (!internal::read_header(ifs, header, afterHeader, fileSize)
			|| !internal::seek_payload_end(ifs, header, fileSize, static_cast<u64>(static_cast<std::streamoff>(afterHeader)))) return false;
		position.commit();
		return true;
	}
}

#endif