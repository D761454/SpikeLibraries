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

#ifndef __ERROR_HPP
#define __ERROR_HPP

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
	enum class ErrorCode : uint16_t {
		errorCode_success = 0,
		errorCode_invalidArgument = 1,
		errorCode_notFound = 2,
		errorCode_IOerror = 3,
		errorCode_outOfMemory = 4,
		errorCode_alreadyExists = 5,
		errorCode_outOfBounds = 6,
		errorCode_magicMismatch = 7,
		errorCode_versionMismatch = 8,
		errorCode_failedCompression = 9,
		errorCode_failedDecompression = 10,
		errorCode_invalidCRC32 = 11,
		errorCode_invalidAsset = 12,

		errorCode_unknown = 0xFFFF
	};

	/// <summary>
	/// returns the associated string representation for an error code
	/// </summary>
	/// <param name="ec">- error code to convert</param>
	/// <returns>string representation / error descriptor</returns>
	constexpr std::string_view to_string_view(ErrorCode ec) noexcept {
		switch (ec) {

		case ErrorCode::errorCode_success: return "success";
		case ErrorCode::errorCode_invalidArgument: return "invalid argument";
		case ErrorCode::errorCode_notFound: return "not found";
		case ErrorCode::errorCode_IOerror: return "I/O error";
		case ErrorCode::errorCode_outOfMemory: return "out of memory";
		case ErrorCode::errorCode_alreadyExists: return "already exists";
		case ErrorCode::errorCode_outOfBounds: return "out of bounds";
		case ErrorCode::errorCode_magicMismatch: return "magic value mismatch";
		case ErrorCode::errorCode_versionMismatch: return "version value mismatch";
		case ErrorCode::errorCode_failedCompression: return "failed compression";
		case ErrorCode::errorCode_failedDecompression: return "failed decompression";
		case ErrorCode::errorCode_invalidCRC32: return "invalid CRC32";
		case ErrorCode::errorCode_invalidAsset: return "invalid asset";
		case ErrorCode::errorCode_unknown: return "unknown error";
		default: return "Invalid Error Code!";
		}
	}

	class ErrorCategoryImpl : public std::error_category {
	public:
		const char* name() const noexcept override { return "errlib"; }

		std::string message(int ev) const override {
			return std::string(to_string_view(static_cast<ErrorCode>(static_cast<uint16_t>(ev))));
		}
	};

	inline const std::error_category& error_category() noexcept {
		static ErrorCategoryImpl s_instance;
		return s_instance;
	}

	inline std::error_code make_error_code(ErrorCode e) noexcept {
		return { static_cast<int>(static_cast<uint16_t>(e)), error_category() };
	}

	/// <summary>
	/// error struct used to house an error code alongside an associated message, used to provide more context during testing / debug
	/// </summary>
	struct Error {
		ErrorCode code = ErrorCode::errorCode_success;
		std::string message;

		Error() = default;
		Error(ErrorCode c, std::string m = {}) : code(c), message(std::move(m)) {}

		bool ok() const noexcept { return code == ErrorCode::errorCode_success; }

		/// <summary>
		/// generates an error object from its constituent elements
		/// </summary>
		/// <param name="c">- error code to store</param>
		/// <param name="extra">- message to associate with this error object</param>
		/// <returns>error code object housing the provided data</returns>
		static Error FromCode(ErrorCode c, std::string_view extra = {}) {
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
	template<> struct is_error_code_enum<errlib::ErrorCode> : true_type {};
}

// example usage #1
/*
static errlib::Error ParseAsset(const std::filesystem::path& path) {
	std::ifstream in(path, std::ios::binary);
	if (!in) {
		return errlib::Error::FromCode(errlib::ErrorCode::errorCode_notFound, path.string());
	}

	in.seekg(0, std::ios::end);
	auto size = in.tellg();
	if (size <= 0) {
		return errlib::Error::FromCode(errlib::ErrorCode::errorCode_invalidArgument, "empty file");
	}

	return errlib::Error{};
}

errlib::Error err = ParseAsset(p);
if (!err.ok()) {
	std::cerr << "ParseAsset failed: " << err.message << '\n';
}
else {
	std::cout << "ParseAsset succeeded\n";
}
*/

// example usage #2
/*
static std::error_code LoadAssetStd(const std::filesystem::path& path) {
	std::ifstream in(path, std::ios::binary);
	if (!in) {
		return errlib::make_error_code(errlib::ErrorCode::errorCode_notFound);
	}

	return {};
}

std::error_code ec = LoadAssetStd(p);
	if (ec) {
		std::cerr << "LoadAssetStd failed: " << ec.message() << '\n';
		try {
			throw std::system_error(ec, "LoadAssetStd");
		}
		catch (const std::system_error& se) {
			std::cerr << "Caught system_error: " << se.what() << '\n';
		}
	}
	else {
		std::cout << "LoadAssetStd succeeded\n";
	}
*/

#endif