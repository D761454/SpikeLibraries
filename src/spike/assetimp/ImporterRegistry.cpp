#include "spike/assetimp/ImporterRegistry.h"

#include <vector>

#include "spike/assetimp/BaseImporter.h"

// Include Importers [...]
#include "spike/assetimp/FbxImporter.h"
#include "spike/assetimp/GltfImporter.h"
#include "spike/assetimp/ObjImporter.h"
#include "spike/assetimp/BTexImporter.h"
#include "spike/assetimp/ShaderImporter.h"

void assimp::get_importer_instances(std::vector<BaseImporter*>& out) {
	out.reserve(5);
	out.push_back(new ObjImporter());
	out.push_back(new GltfImporter());
	out.push_back(new FbxImporter());
	out.push_back(new BTexImporter());
	out.push_back(new ShaderImporter());
}

void assimp::delete_importer_instances(std::vector<BaseImporter*>& del) {
	for (size_t i = 0; i < del.size(); ++i) {
		delete del[i];
		del[i] = nullptr;
	}
	del.clear();
}