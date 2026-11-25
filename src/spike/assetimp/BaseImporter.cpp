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

#include "spike/assetimp/BaseImporter.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <exception>
#include <filesystem>
#include <memory>
#include <new>

#include "spike/stl_aliases.h"
#include "spike/error.hpp"

#include "spike/assetimp/aicollection.h"

namespace {
	string normalize_extension(string extension) {
		std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char ch) {
			return static_cast<char>(std::tolower(ch));
		});
		return extension;
	}
}

assimp::BaseImporter::BaseImporter() noexcept : extensions(), lastError(), ppFlags(0) {}

bool assimp::BaseImporter::CanParse(const string& file) const {
	const std::filesystem::path path(file);
	return path.has_extension() && SupportsExtension(path.extension().string());
}

bool assimp::BaseImporter::SupportsExtension(const string& extn) const {
	return extensions.contains(normalize_extension(extn));
}

assimp::Collection* assimp::BaseImporter::ReadFile(const string& file) {
	lastError = {};
	if (file.empty()) {
		lastError = errlib::Error::FromCode(errlib::ErrorCode_invalidArgument, "The import path is empty.");
		return nullptr;
	}

	try {
		std::unique_ptr<Collection> collection = std::make_unique<Collection>();
		assert(collection && "BaseImporter::ReadFile must allocate a collection.");
		lastError = InternReadFile(file, collection.get());
		if (!lastError.ok()) return nullptr;
		return collection.release();
	}
	catch (const std::bad_alloc&) {
		lastError = errlib::Error::FromCode(errlib::ErrorCode_outOfMemory, "Could not allocate the imported collection.");
	}
	catch (const std::exception& exception) {
		lastError = errlib::Error::FromCode(errlib::ErrorCode_unknown, exception.what());
	}
	catch (...) {
		lastError = errlib::Error::FromCode(errlib::ErrorCode_unknown, "Importer failed with an unknown exception.");
	}

	return nullptr;
}
