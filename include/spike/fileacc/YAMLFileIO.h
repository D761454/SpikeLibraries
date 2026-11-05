#ifndef __YAML_FILE_IO
#define __YAML_FILE_IO

#include <optional>
#include <variant>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

#include <spike/def_aliases.h>
#include <spike/engine_aliases.h>
#include <spike/error.hpp>
#include <spike/stl_aliases.h>

#include "commons.hpp"

namespace YAML { class Node; } // may not need?

namespace fileacc {
	class YAMLData {
		using Sequence = std::vector<YAMLData>;
		using Map = std::unordered_map<string, YAMLData>;
		using Variable = std::variant<string, i32, i64, u32, u64, f64, uuid, Sequence, Map>;

		Variable value;

	public:
		YAMLData() : value(string{}) {}

		YAMLData(const string& s) : value(s) {}
		YAMLData(string&& s) : value(std::move(s)) {}
		YAMLData(i32 v) : value(v) {}
		YAMLData(i64 v) : value(v) {}
		YAMLData(u8 v) : value(v) {}
		YAMLData(u32 v) : value(v) {}
		YAMLData(u64 v) : value(v) {}
		YAMLData(f32 v) : value(v) {}
		YAMLData(f64 v) : value(v) {}
		YAMLData(uuid v) : value(v) {}

		bool IsString()     const noexcept { return std::holds_alternative<string>(value); }
		bool IsI32()        const noexcept { return std::holds_alternative<i32>(value); }
		bool IsI64()        const noexcept { return std::holds_alternative<i64>(value); }
		bool IsU32()        const noexcept { return std::holds_alternative<u32>(value); }
		bool IsU64()        const noexcept { return std::holds_alternative<u64>(value); }
		bool IsF64()        const noexcept { return std::holds_alternative<f64>(value); }
		bool Isuuid()       const noexcept { return std::holds_alternative<uuid>(value); }
		bool IsSequence()   const noexcept { return std::holds_alternative<Sequence>(value); }
		bool IsMap()        const noexcept { return std::holds_alternative<Map>(value); }

		template<typename T>
		std::optional<T> Get() const noexcept {
			if (auto p = std::get_if<T>(&value)) return *p;
			return std::nullopt;
		}

		template<typename T>
		std::optional<T> Get(T& def) const noexcept {
			if (auto p = std::get_if<T>(&value)) return *p;
			return def;
		}

		const Sequence& AsSequence() const {
			if (!IsSequence()) throw std::runtime_error("YAMLData: not a sequence");
			return std::get<Sequence>(value);
		}
		Sequence& AsSequence() {
			if (!IsSequence()) value = Sequence{};
			return std::get<Sequence>(value);
		}

		const Map& AsMap() const {
			if (!IsMap()) throw std::runtime_error("YAMLData: not a map");
			return std::get<Map>(value);
		}
		Map& AsMap() {
			if (!IsMap()) value = Map{};
			return std::get<Map>(value);
		}

#pragma region SEQUENCE

		void SequencePushBack(const YAMLData& item) {
			AsSequence().push_back(item);
		}
		void SequencePushBack(YAMLData&& item) {
			AsSequence().push_back(std::move(item));
		}
		usize SequenceSize() const {
			if (!IsSequence()) return 0;
			return std::get<Sequence>(value).size();
		}
		YAMLData& SequenceAt(usize idx) {
			return AsSequence().at(idx);
		}
		const YAMLData& SequenceAt(usize idx) const {
			return AsSequence().at(idx);
		}

#pragma endregion

#pragma region MAP

		YAMLData& operator[](const string& key) {
			return AsMap()[key];
		}
		YAMLData& operator[](string&& key) {
			return AsMap()[std::move(key)];
		}

		const YAMLData* FindInMap(const string& key) const noexcept {
			if (auto p = std::get_if<Map>(&value)) {
				auto it = p->find(key);
				if (it != p->end()) return &it->second;
			}
			return nullptr;
		}
		bool MapContains(const string& key) const noexcept {
			return FindInMap(key) != nullptr;
		}
		usize MapSize() const noexcept {
			if (!IsMap()) return 0;
			return std::get<Map>(value).size();
		}

#pragma endregion

		enum class Kind { String, I32, I64, U32, U64, F64, uuid, Sequence, Map };
		Kind GetKind() const noexcept {
			switch (value.index()) {
			case 0: return Kind::String;
			case 1: return Kind::I32;
			case 2: return Kind::I64;
			case 3: return Kind::U32;
			case 4: return Kind::U64;
			case 5: return Kind::F64;
			case 6: return Kind::uuid;
			case 7: return Kind::Sequence;
			case 8: return Kind::Map;
			default: return Kind::String;
			}
		}
	};

	class YAMLFileIO {
	public:
		/*
		Load a YAML file and convert it into YAMLData.
		Returns YAMLData on success, std::nullopt on failure.
		*/
		static std::optional<YAMLData> LoadFromFile(const string& filePath);

		/* Save YAMLData to file */
		static errlib::Error SaveToFile(const string& filePath, const YAMLData& data);
	};
}

#endif