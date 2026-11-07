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

#include "spike/assetimp/BTexImporter.h"

#include <filesystem>
#include <utility>

#define STBI_NO_PIC
#define STBI_NO_PSD
#define STBI_NO_PNM
#define STBI_NO_GIF
#define STBI_FAILURE_USERMSG

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb/stb_image.h"

#include "spike/error.hpp"
#include "spike/warning.h"
#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

#include "spike/assetimp/PostProcessing.h"
#include "spike/assetimp/aicollection.h"
#include "spike/assetimp/aitexture.h"
#include "spike/assetimp/BaseImporter.h"

DISABLE_WARNING_UNUSED_VARIABLE

assimp::BTexImporter::BTexImporter() : BaseImporter(), sourcePath() {
	extensions = { ".png", ".jpg", ".jpeg", ".tga", ".bmp", ".hdr" };
}

assimp::BTexImporter::~BTexImporter() {}

//void assimp::Texture::STBImageDeleter::operator()(u8* ptr) const {
//	stbi_image_free(ptr);
//}
//
//void assimp::Texture::STBImageDeleter::operator()(f32* ptr) const {
//	stbi_image_free(ptr);
//}

template<>
void assimp::STBImageDeleter<u8>::release(void* ptr) const {
	stbi_image_free(ptr);
}

template<>
void assimp::STBImageDeleter<f32>::release(void* ptr) const {
	stbi_image_free(ptr);
}

errlib::Error assimp::BTexImporter::InternReadFile(const string& file, Collection* scene) {
	if (ppFlags & PostProcessingFlags_flipTextureYOnImport) stbi_set_flip_vertically_on_load(false);
	else stbi_set_flip_vertically_on_load(true);

	Texture tex; tex.fileName = std::filesystem::path(file).filename().string();

	if (stbi_is_hdr(file.c_str())) {
		f32* data = stbi_loadf(file.c_str(), reinterpret_cast<i32*>(&tex.width), reinterpret_cast<i32*>(&tex.height), &tex.channels, 4);
		if (!data) {
			stbi_image_free(data); data = nullptr;
			errlib::Error(errlib::ErrorCode_unknown, stbi_failure_reason());
		}
		tex.pixelDataHDR.reset(data);

		tex.sRGB = true;
	}
	else {
		uchar* data = stbi_load(file.c_str(), reinterpret_cast<i32*>(&tex.width), reinterpret_cast<i32*>(&tex.height), &tex.channels, 4);
		if (!data) {
			stbi_image_free(data); data = nullptr;
			errlib::Error(errlib::ErrorCode_unknown, stbi_failure_reason());
		}
		tex.pixelDataSDR.reset(data);

		switch (tex.channels) {
		case 1:
			break;
		case 2:
			break;
		default:
			tex.sRGB = true;
		}
	}

	scene->textures.push_back(std::move(tex));

	scene->name = std::filesystem::path(file).filename().stem().string();
	sourcePath = std::filesystem::absolute(file).string();

	return errlib::Error{};
}