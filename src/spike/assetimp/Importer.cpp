#include "spike/assetimp/Importer.hpp"

#include <algorithm>
#include <set>
#include <vector>

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"
#include "spike/debugl/debug.hpp"
#include "spike/error.hpp"

#include "spike/assetimp/BaseImporter.h"
#include "spike/assetimp/ImporterRegistry.h"
#include "spike/assetimp/aicollection.h"

assimp::Importer::Importer() : importers(), scene(nullptr) {
	get_importer_instances(importers);
}

assimp::Importer::~Importer() {
	delete_importer_instances(importers);
	delete scene;
	scene = nullptr;
}

errlib::Error assimp::Importer::RegisterLoader(BaseImporter* l) {
	if (!l) return errlib::Error(errlib::ErrorCode_invalidArgument, "Attempting to register importer with nullptr");

	importers.push_back(l);
	return errlib::Error{};
}

errlib::Error assimp::Importer::UnregisterLoader(BaseImporter* l) {
	if (!l) return errlib::Error{};

	std::vector<BaseImporter*>::iterator it = std::find(importers.begin(), importers.end(), l);
	if (it != importers.end()) {
		importers.erase(it);
		return errlib::Error{};
	}

	return errlib::Error(errlib::ErrorCode_invalidArgument, "Unable to remove importer!");
}

const assimp::Collection* assimp::Importer::ReadFile(const string& path, u32 ppSteps) {
	if (path.empty()) return nullptr;

	FreeScene();

	for (auto& bi : importers) {
		if (!bi->CanParse(path)) continue;
		bi->SetFlags(ppSteps);
		if (Collection* s = bi->ReadFile(path); s) return s;
	}

	Debug::LogError("Asset file format not supported! Try exporting the asset as a gltf/glb or fbx file instead if importing a mesh or animations, and a png or jpg if importing a texture!");

	return nullptr;
}

const assimp::Collection* assimp::Importer::GetScene() {
	return scene;
}

void assimp::Importer::FreeScene() {
	if (scene) delete scene;
	scene = nullptr;
}

bool assimp::Importer::IsExtensionSupported(const string& extn) const {
	for (auto& bi : importers) {
		if (bi->SupportsExtension(extn)) return true;
	}

	return false;
}

assimp::BaseImporter* assimp::Importer::GetImporter(const string& extn) const {
	for (auto& bi : importers) {
		if (bi->SupportsExtension(extn)) return bi;
	}

	return nullptr;
}

const usize assimp::Importer::GetImporterIndex(const string& extn) const {
	for (usize i = 0; i < importers.size(); ++i) {
		if (importers[i]->SupportsExtension(extn)) return i;
	}
	return -1;
}