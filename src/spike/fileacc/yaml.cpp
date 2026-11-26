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

#include "spike/fileacc/yaml.h"

#include <exception>
#include <filesystem>
#include <fstream>
#include <limits>
#include <optional>
#include <string>
#include <sstream>

#include "external/stduuid/uuid.h"
#include <yaml-cpp/emitter.h>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/node/type.h>

#include "spike/warning.h"

DISABLE_WARNING_PUSH
DISABLE_WARNING_YAML_H
#include <yaml-cpp/yaml.h>
DISABLE_WARNING_POP

#include "spike/def_aliases.h"
#include "spike/stl_aliases.h"
#include "spike/error.hpp"

namespace YAML {
	template<>
	struct convert<uuids::uuid> {
		static Node encode(const uuids::uuid& rhs) {
			return Node(uuids::to_string(rhs));
		}

		static bool decode(const Node& node, uuids::uuid& rhs) {
			if (!node.IsScalar()) return false;

			try {
				const auto s = node.as<string>();
				if (auto maybe = uuids::uuid::from_string(s)) {
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
	YAML::Node yaml_data_to_node(const fileacc::YAMLData& d) {
		YAML::Node node;

		if (d.IsMap()) {
			node = YAML::Node(YAML::NodeType::Map);
			const auto& m = d.AsMap();
			for (const auto& kv : m) {
				node[kv.first] = yaml_data_to_node(kv.second);
			}
			return node;
		}

		if (d.Isuuid()) {
			return YAML::Node(*d.Get<uuids::uuid>());
		}

		if (d.IsSequence()) {
			node = YAML::Node(YAML::NodeType::Sequence);
			const auto& seq = d.AsSequence();
			for (const auto& item : seq) {
				node.push_back(yaml_data_to_node(item));
			}
			return node;
		}

		if (d.IsString())
			return YAML::Node(*d.Get<string>());
		if (d.IsI32())
			return YAML::Node(*d.Get<i32>());
		if (d.IsI64())
			return YAML::Node(*d.Get<i64>());
		if (d.IsU32())
			return YAML::Node(*d.Get<u32>());
		if (d.IsU64())
			return YAML::Node(*d.Get<u64>());
		if (d.IsF64())
			return YAML::Node(*d.Get<f64>());

		return YAML::Node(string{});
	}

	fileacc::YAMLData parse_scalar_string_to_yaml_data(const string& s) {
		if (auto maybe = uuids::uuid::from_string(s)) {
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

	fileacc::YAMLData node_to_yaml_data(const YAML::Node& node) {
		if (!node) return fileacc::YAMLData(string{});

		if (node.IsMap()) {
			fileacc::YAMLData out;
			out.AsMap();
			auto& m = out.AsMap();
			for (const auto& kv : node) {
				const string key = kv.first.as<string>();
				m.emplace(key, node_to_yaml_data(kv.second));
			}
			return out;
		}

		if (node.IsSequence()) {
			fileacc::YAMLData out;
			auto& seq = out.AsSequence();
			for (const auto& item : node) {
				seq.push_back(node_to_yaml_data(item));
			}
			return out;
		}

		if (node.IsScalar()) {
			string s = node.as<string>();
			return parse_scalar_string_to_yaml_data(s);
		}

		return fileacc::YAMLData(string{});
	}
}

errlib::Error fileacc::save_yaml(const string& filePath, const YAMLData& data) {
	try {
		YAML::Node root = yaml_data_to_node(data);

		YAML::Emitter emitter;
		emitter << root;

		if (!emitter.good())
			return errlib::Error::FromCode(errlib::ErrorCode_IOerror, emitter.GetLastError());

		std::ofstream ofs(filePath, std::ios::binary);
		if (!ofs)
			return errlib::Error::FromCode(errlib::ErrorCode_IOerror, "save_yaml: failed to open: " + filePath);

		ofs << emitter.c_str();
		ofs.close();
		return errlib::Error();
	}
	catch (const std::exception& ex) {
		return errlib::Error::FromCode(errlib::ErrorCode_IOerror, string(ex.what()));
	}
	catch (...) {
		return errlib::Error::FromCode(errlib::ErrorCode_unknown, "unknown exception writing to: " + filePath);
	}
}

errlib::Error fileacc::load_yaml(const string& filePath, YAMLData& outData) {
	try {
		if (!std::filesystem::exists(std::filesystem::path(filePath)))
			return errlib::Error::FromCode(errlib::ErrorCode_missingFile, "load_yaml: file does not exist: " + filePath);

		YAML::Node root = YAML::LoadFile(filePath);
		outData = node_to_yaml_data(root);
		return errlib::Error();
	}
	catch (const std::exception& ex) {
		return errlib::Error::FromCode(errlib::ErrorCode_IOerror, string(ex.what()));
	}
	catch (...) {
		return errlib::Error::FromCode(errlib::ErrorCode_unknown, "unknown exception reading from: " + filePath);
	}
}