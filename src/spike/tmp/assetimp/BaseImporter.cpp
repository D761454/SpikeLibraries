#include "spike/assetimp/BaseImporter.h"

#include <filesystem>
#include <memory>
#include <set>

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