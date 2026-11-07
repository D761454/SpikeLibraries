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

#ifndef __ASSET_IMP_TEXTURE_H
#define __ASSET_IMP_TEXTURE_H

#include <memory>
#include <utility>

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"

namespace assimp {
	enum CompressionFormats_ : u8 {
		/*4 bits/pixel - no alpha or 1bit alpha*/
		CompressionFormats_BC1 = 0,
		/*4 bits/pixel - no alpha or 1bit alpha*/
		CompressionFormats_DXT1 = CompressionFormats_BC1,
		/*8 bits/pixel - smooth alpha*/
		CompressionFormats_BC3 = 1,
		/*8 bits/pixel - smooth alpha*/
		CompressionFormats_DXT5 = CompressionFormats_BC3,
		/*single channel compression*/
		CompressionFormats_BC4 = 2,
		/*dual channel, store x and y, recalculate z in pixel shader*/
		CompressionFormats_BC5 = 3,
		/*HDR block compression*/
		CompressionFormats_BC6H = 4,
		/*high quality rgb/rgba, less artifacts than BC1/3*/
		CompressionFormats_BC7 = 5,
		/*Legacy baseline standard for OpenGL ES 3.0+ / Android*/
		CompressionFormats_ETC2 = 6,
		/*Legacy baseline standard for OpenGL ES 3.0+ / Android*/
		CompressionFormats_EAC = CompressionFormats_ETC2,

		CompressionFormats_auto = 255
	};

	template<typename T>
	struct STBImageDeleter {
		void operator()(T* ptr) const {
			release(ptr);
		}
		void release(void* ptr) const;
	};

	template<typename T>
	using StbImagePtr = std::unique_ptr<T, STBImageDeleter<T>>;

	struct Texture {
		u32 width;
		u32 height;
		i32 depth;
		i32 channels;
		i32 mipLevels;
		/*PixelFormat format;*/
		bool sRGB;
		bool flippedY;
		StbImagePtr<u8> pixelDataSDR;
		StbImagePtr<f32> pixelDataHDR;
		string fileName;

		Texture() noexcept :
			width(0), height(0),
			depth(1), mipLevels(1), channels(0),
			fileName(), flippedY(false), sRGB(false) {
		}

		Texture(Texture&& other) noexcept :
			width(other.width), height(other.height),
			depth(other.depth), mipLevels(other.mipLevels), channels(other.channels),
			fileName(other.fileName), flippedY(other.flippedY), sRGB(other.sRGB) {
			pixelDataSDR = std::move(other.pixelDataSDR); pixelDataHDR = std::move(other.pixelDataHDR);
		}

		~Texture() {}
	};

	// basis_universal: convert to BC1-BC7 at runtime.
	// stb_image.h: lightweight library to handle png, jpg, tga, bmp, hdr
	// tinyexr for ext files
}

#endif