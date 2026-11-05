#include "commons.hpp"

#include <fstream>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include <spike/def_aliases.h>
#include <spike/engine_aliases.h>
#include <spike/podio.hpp>
#include <spike/stl_aliases.h>
#include <spike/error.hpp>

bool fileacc::check_file(const string& path, const u32 expectedMagic, const u32 expectedVer) noexcept {
	if (path.empty()) return false;
	std::ifstream ifs(path, std::ios::binary);
	if (!ifs) return false;
	FileCheck hdr;
	podio::read_pod(ifs, hdr);
	return valid_file(hdr, expectedMagic, expectedVer);
}

bool fileacc::check_file(const filepath& path, const u32 expectedMagic, const u32 expectedVer) noexcept {
	if (path.empty()) return false;
	std::ifstream ifs(path, std::ios::binary);
	if (!ifs) return false;
	FileCheck hdr;
	podio::read_pod(ifs, hdr);
	return valid_file(hdr, expectedMagic, expectedVer);
}

bool fileacc::check_file(const string& path, const FileCheck& expectedHeader) noexcept {
	if (path.empty()) return false;
	std::ifstream ifs(path, std::ios::binary);
	if (!ifs) return false;
	FileCheck hdr;
	podio::read_pod(ifs, hdr);
	return valid_file(hdr, expectedHeader);
}

bool fileacc::check_file(const filepath& path, const FileCheck& expectedHeader) noexcept {
	if (path.empty()) return false;
	std::ifstream ifs(path, std::ios::binary);
	if (!ifs) return false;
	FileCheck hdr;
	podio::read_pod(ifs, hdr);
	return valid_file(hdr, expectedHeader);
}