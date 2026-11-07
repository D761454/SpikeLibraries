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

#ifndef __ASSET_IMP_POSTPROCESSING_H
#define __ASSET_IMP_POSTPROCESSING_H

#include "spike/def_aliases.h"

namespace assimp {
	enum PostProcessingFlags_ : u32 {
		PostProcessingFlags_none = 0,
		PostProcessingFlags_triangulateFaces = 1 << 0,
		PostProcessingFlags_pbrImport = 1 << 1, // data is loaded in as (position, normal, uv, tangent)
		PostProcessingFlags_importTangents = 1 << 2,
		PostProcessingFlags_importColours = 1 << 3,
		PostProcessingFlags_vec3UVs = 1 << 4,
		PostProcessingFlags_generateMissingNormals = 1 << 5,
		PostProcessingFlags_generateMissingUVs = 1 << 6,
		PostProcessingFlags_importMeshes = 1 << 7,
		PostProcessingFlags_importTextures = 1 << 8,
		PostProcessingFlags_importAnimations = 1 << 9,
		PostProcessingFlags_splitLargeMeshes = 1 << 10, // splits a large mesh into many sub meshes
		PostProcessingFlags_improveCacheLocality = 1 << 11, // reorders vertices and indexes to improve fetching
		PostProcessingFlags_populateArmatureData = 1 << 12,
		PostProcessingFlags_flipUVs = 1 << 13, // flips UVs along the Y axis
		PostProcessingFlags_flipWindingOrder = 1 << 14, // swaps the last 2 indexes every 3 to swap winding order
		PostProcessingFlags_flipTextureYOnImport = 1 << 15,
	};
}

#endif