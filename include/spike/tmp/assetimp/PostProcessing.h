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