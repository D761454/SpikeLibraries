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

#include <filesystem>
#include <memory>
#include <new>

#include "spike/stl_aliases.h"
#include "spike/debugl/debug.hpp"
#include "spike/error.hpp"

#include "spike/assetimp/aicollection.h"

assimp::BaseImporter::BaseImporter() noexcept : extensions(), ppFlags(0) {}

bool assimp::BaseImporter::CanParse(const string& file) const {
	std::filesystem::path p(file);

	return p.has_extension() ? extensions.contains(p.extension().string()) : false;
}

assimp::Collection* assimp::BaseImporter::ReadFile(const string& file) {
	std::unique_ptr<Collection> sc(new Collection());

	errlib::Error err = InternReadFile(file, sc.get());

	if (!err.ok()) {
		Debug::LogError("Issue Importing Asset: {}: {}", errlib::to_string_view(err.code), err.message);
		return nullptr;
	}

	return sc.release();
}