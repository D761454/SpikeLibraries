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

#ifndef __SPIKE_ERROR_HPP
#define __SPIKE_ERROR_HPP

#include <cstdint>
#include <string>
#include <string_view>
#include <system_error>

/*
----------------------------------------------------------------------

File Summary:
	This file holds Error structs and enums for use in safe error handling
	and debugging of projects.

----------------------------------------------------------------------
*/

namespace errlib {
	/// <summary>
	/// error code enum used to house various error types
	/// </summary>
	enum ErrorCode_ : uint16_t {
		ErrorCode_success = 0,
		ErrorCode_invalidArgument = 1,
		ErrorCode_notFound = 2,
		ErrorCode_IOerror = 3,
		ErrorCode_outOfMemory = 4,
		ErrorCode_alreadyExists = 5,
		ErrorCode_outOfBounds = 6,
		ErrorCode_magicMismatch = 7,
		ErrorCode_versionMismatch = 8,
		ErrorCode_failedCompression = 9,
		ErrorCode_failedDecompression = 10,
		ErrorCode_invalidCRC32 = 11,
		ErrorCode_invalidAsset = 12,
		ErrorCode_missingFile = 13,
		ErrorCode_IDmismatch = 14,
		ErrorCode_missingAsset = 15,
		
		ErrorCode_unknown = 0xFFFF
	};

	/// <summary>
	/// returns the associated string representation for an error code
	/// </summary>
	/// <param name="ec">error code to convert</param>
	/// <returns>string representation / error descriptor</returns>
	constexpr std::string_view to_string_view(ErrorCode_ ec) noexcept {
		switch (ec) {

		case ErrorCode_success: return "success";
		case ErrorCode_invalidArgument: return "invalid argument";
		case ErrorCode_notFound: return "not found";
		case ErrorCode_IOerror: return "I/O error";
		case ErrorCode_outOfMemory: return "out of memory";
		case ErrorCode_alreadyExists: return "already exists";
		case ErrorCode_outOfBounds: return "out of bounds";
		case ErrorCode_magicMismatch: return "magic value mismatch";
		case ErrorCode_versionMismatch: return "version value mismatch";
		case ErrorCode_failedCompression: return "failed compression";
		case ErrorCode_failedDecompression: return "failed decompression";
		case ErrorCode_invalidCRC32: return "invalid CRC32";
		case ErrorCode_invalidAsset: return "invalid asset";
		case ErrorCode_missingFile: return "missing file";
		case ErrorCode_IDmismatch: return "id mismatch";
		case ErrorCode_missingAsset: return "missing asset";
		case ErrorCode_unknown: return "unknown error";
		default: return "Invalid Error Code!";
		}
	}

	class ErrorCategoryImpl : public std::error_category {
	public:
		const char* name() const noexcept override { return "errlib"; }

		std::string message(int ev) const override {
			return std::string(to_string_view(static_cast<ErrorCode_>(static_cast<uint16_t>(ev))));
		}
	};

	inline const std::error_category& error_category() noexcept {
		static ErrorCategoryImpl s_instance;
		return s_instance;
	}

	inline std::error_code make_error_code(ErrorCode_ e) noexcept {
		return { static_cast<int>(static_cast<uint16_t>(e)), error_category() };
	}

	/// <summary>
	/// error struct used to house an error code alongside an associated message, used to provide more context during testing / debug
	/// </summary>
	struct Error {
		ErrorCode_ code = ErrorCode_success;
		std::string message;

		Error() = default;
		Error(ErrorCode_ c, std::string m = {}) : code(c), message(std::move(m)) {}

		bool ok() const noexcept { return code == ErrorCode_success; }

		/// <summary>
		/// generates an error object from its constituent elements
		/// </summary>
		/// <param name="c">error code to store</param>
		/// <param name="extra">message to associate with this error object</param>
		/// <returns>error code object housing the provided data</returns>
		static Error FromCode(ErrorCode_ c, std::string_view extra = {}) {
			std::string msg = std::string(to_string_view(c));
			if (!extra.empty()) {
				msg += ": ";
				msg += std::string(extra);
			}
			return Error(c, std::move(msg));
		}

		/// <summary>
		/// converts the erro object to a std::error_code for use with default error handling
		/// </summary>
		/// <returns>std::error_code generated from this objects errlib::ErrorCode</returns>
		std::error_code as_std_error() const noexcept {
			return make_error_code(code);
		}
	};
}

namespace std {
	template<> struct is_error_code_enum<errlib::ErrorCode_> : true_type {};
}

#endif