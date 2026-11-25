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

#include "spike/assetimp/Importer.hpp"

#include <cassert>
#include <exception>
#include <new>

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

#include "spike/assetimp/BaseImporter.h"
#include "spike/assetimp/ImporterRegistry.h"
#include "spike/assetimp/aicollection.h"

assimp::Importer::Importer() : importers(), scene(nullptr), lastError(), pp(0) {
	get_importer_instances(importers);
}

assimp::Importer::~Importer() {
	delete_importer_instances(importers);
	delete scene;
	scene = nullptr;
}

const assimp::Collection* assimp::Importer::ReadFile(const string& path) {
	return ReadFile(path, pp);
}

const assimp::Collection* assimp::Importer::ReadFile(const string& path, u32 ppSteps) {
	FreeCollection();
	lastError = {};
	if (path.empty()) {
		lastError = errlib::Error::FromCode(errlib::ErrorCode_invalidArgument, "The import path is empty.");
		return nullptr;
	}

	bool foundImporter = false;
	try {
		for (BaseImporter* importer : importers) {
			assert(importer && "Importer registry contains a null importer.");
			if (!importer || !importer->CanParse(path)) continue;
			foundImporter = true;
			importer->SetFlags(ppSteps);
			scene = importer->ReadFile(path);
			if (scene) {
				lastError = {};
				return scene;
			}
			lastError = importer->GetLastError();
		}
	}
	catch (const std::bad_alloc&) {
		lastError = errlib::Error::FromCode(errlib::ErrorCode_outOfMemory, "Could not allocate importer data.");
	}
	catch (const std::exception& exception) {
		lastError = errlib::Error::FromCode(errlib::ErrorCode_unknown, exception.what());
	}
	catch (...) {
		lastError = errlib::Error::FromCode(errlib::ErrorCode_unknown, "Import failed with an unknown exception.");
	}

	if (!foundImporter && lastError.ok()) {
		lastError = errlib::Error::FromCode(errlib::ErrorCode_notFound, "No importer supports the file extension.");
	}
	else if (lastError.ok()) {
		lastError = errlib::Error::FromCode(errlib::ErrorCode_unknown, "The importer returned no collection and no error.");
	}
	return nullptr;
}

const assimp::Collection* assimp::Importer::GetCollection() {
	return scene;
}

const assimp::Collection* assimp::Importer::GetCollection() const noexcept {
	return scene;
}

void assimp::Importer::FreeCollection() {
	if (scene) delete scene;
	scene = nullptr;
}

bool assimp::Importer::IsExtensionSupported(const string& extn) const {
	for (auto* bi : importers) {
		if (bi && bi->SupportsExtension(extn)) return true;
	}

	return false;
}