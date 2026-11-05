#include "YAMLFileIO.h"

#include <exception>
#include <filesystem>
#include <fstream>
#include <limits>
#include <optional>
#include <string>

#include <spike/warning.h>
#include <UUID/uuid.h>
#include <YAML/emitter.h>
#include <YAML/node/node.h>
#include <YAML/node/parse.h>
#include <YAML/node/type.h>
#include <spike/def_aliases.h>
#include <spike/engine_aliases.h>
#include <spike/stl_aliases.h>

DISABLE_WARNING_PUSH
DISABLE_WARNING_YAML_H
#include <YAML/yaml.h>
DISABLE_WARNING_POP
#include <sstream>

#include <spike/error.hpp>
#include <debug.hpp>

DISABLE_WARNING_PUSH
DISABLE_WARNING_UNINITIALIZED_VARIABLE
DISABLE_WARNING_UNUSED_VARIABLE

namespace YAML {
	template<>
	struct convert<uuid> {
		static Node encode(const uuid& rhs) {
			return Node(uuids::to_string(rhs));
		}

		static bool decode(const Node& node, uuid& rhs) {
			if (!node.IsScalar()) return false;

			try {
				const auto s = node.as<string>();
				if (auto maybe = uuid::from_string(s)) {
					rhs = *maybe;
					return true;
				}
			}
			catch (...) {}

			return false;
		}
	};
}

namespace {
	YAML::Node YAMLDataToNode(const fileacc::YAMLData& d) {
		YAML::Node node;

		if (d.IsMap()) {
			node = YAML::Node(YAML::NodeType::Map);
			const auto& m = d.AsMap();
			for (const auto& kv : m) {
				node[kv.first] = YAMLDataToNode(kv.second);
			}
			return node;
		}

		if (d.Isuuid()) {
			return YAML::Node(*d.Get<uuid>());
		}

		if (d.IsSequence()) {
			node = YAML::Node(YAML::NodeType::Sequence);
			const auto& seq = d.AsSequence();
			for (const auto& item : seq) {
				node.push_back(YAMLDataToNode(item));
			}
			return node;
		}

		if (d.IsString()) {
			return YAML::Node(*d.Get<string>());
		}
		if (d.IsI32()) {
			return YAML::Node(*d.Get<i32>());
		}
		if (d.IsI64()) {
			return YAML::Node(*d.Get<i64>());
		}
		if (d.IsU32()) {
			return YAML::Node(*d.Get<u32>());
		}
		if (d.IsU64()) {
			return YAML::Node(*d.Get<u64>());
		}
		if (d.IsF64()) {
			return YAML::Node(*d.Get<f64>());
		}

		return YAML::Node(string{});
	}

	fileacc::YAMLData ParseScalarStringToYAMLData(const string& s) {
		if (auto maybe = uuid::from_string(s)) {
			return fileacc::YAMLData(*maybe);
		}

		try {
			usize pos = 0;
			ll lv = std::stoll(s, &pos, 0);
			if (pos == s.size()) {
				if (lv >= std::numeric_limits<i32>::min() && lv <= std::numeric_limits<i32>::max())
					return fileacc::YAMLData(static_cast<i32>(lv));
				return fileacc::YAMLData(static_cast<i64>(lv));
			}
		}
		catch (...) {}

		try {
			usize pos = 0;
			ull uv = std::stoull(s, &pos, 0);
			if (pos == s.size()) {
				if (uv <= std::numeric_limits<u32>::max())
					return fileacc::YAMLData(static_cast<u32>(uv));
				return fileacc::YAMLData(static_cast<u64>(uv));
			}
		}
		catch (...) {}

		try {
			usize pos = 0;
			f64 dv = std::stod(s, &pos);
			if (pos == s.size()) {
				return fileacc::YAMLData(dv);
			}
		}
		catch (...) {}

		return fileacc::YAMLData(s);
	}

	fileacc::YAMLData NodeToYAMLData(const YAML::Node& node) {
		if (!node) return fileacc::YAMLData(string{});

		if (node.IsMap()) {
			fileacc::YAMLData out;
			out.AsMap();
			auto& m = out.AsMap();
			for (const auto& kv : node) {
				const string key = kv.first.as<string>();
				m.emplace(key, NodeToYAMLData(kv.second));
			}
			return out;
		}

		if (node.IsSequence()) {
			fileacc::YAMLData out;
			auto& seq = out.AsSequence();
			for (const auto& item : node) {
				seq.push_back(NodeToYAMLData(item));
			}
			return out;
		}

		if (node.IsScalar()) {
			string s = node.as<string>();
			return ParseScalarStringToYAMLData(s);
		}

		return fileacc::YAMLData(string{});
	}
}

std::optional<fileacc::YAMLData> fileacc::YAMLFileIO::LoadFromFile(const string& filePath) {
	try {
		if (!std::filesystem::exists(std::filesystem::path(filePath))) {
			Debug::LogError("YAMLFileIO::LoadFromFile - file not found: {}", filePath);
			return std::nullopt;
		}

		YAML::Node root = YAML::LoadFile(filePath);
		return NodeToYAMLData(root);
	}
	catch (const std::exception& ex) {
		Debug::LogError("YAMLFileIO::LoadFromFile - exception loading {}: {}", filePath, ex.what());
		return std::nullopt;
	}
	catch (...) {
		Debug::LogError("YAMLFileIO::LoadFromFile - unknown exception loading {}", filePath);
		return std::nullopt;
	}
}

errlib::Error fileacc::YAMLFileIO::SaveToFile(const string& filePath, const YAMLData& data) {
	try {
		YAML::Node root = YAMLDataToNode(data);

		YAML::Emitter emitter;
		emitter << root;

		if (!emitter.good()) {
			Debug::LogError("YAMLFileIO::SaveToFile - emitter error: {}", emitter.GetLastError());
			return errlib::Error::FromCode(errlib::ErrorCode::errorCode_IOerror, emitter.GetLastError());
		}

		std::ofstream ofs(filePath, std::ios::binary);
		if (!ofs) {
			Debug::LogError("YAMLFileIO::SaveToFile - failed to open: {}", filePath);
			return errlib::Error::FromCode(errlib::ErrorCode::errorCode_IOerror, filePath);
		}

		ofs << emitter.c_str();
		ofs.close();
		return errlib::ErrorCode{};
	}
	catch (const std::exception& ex) {
		Debug::LogError("YAMLFileIO::SaveToFile - exception writing {}: {}", filePath, ex.what());
		return errlib::Error::FromCode(errlib::ErrorCode::errorCode_IOerror, string(ex.what()));
	}
	catch (...) {
		Debug::LogError("YAMLFileIO::SaveToFile - unknown exception writing {}", filePath);
		return errlib::Error::FromCode(errlib::ErrorCode::errorCode_unknown, filePath);
	}
}

DISABLE_WARNING_POP