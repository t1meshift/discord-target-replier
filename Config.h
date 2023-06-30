#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace griha {
	struct AdditionalTargetParams {
		std::vector<std::string> replies;
		std::vector<std::string> on_delete_replies;
	};
	void to_json(nlohmann::json& j, const AdditionalTargetParams& t);
	void from_json(const nlohmann::json& j, AdditionalTargetParams& t);

	struct TargetInfo {
		std::string username;
		uint16_t discriminator;
		AdditionalTargetParams additional;
	};
	void to_json(nlohmann::json& j, const TargetInfo& t);
	void from_json(const nlohmann::json& j, TargetInfo& t);

	class Config {
	public:
		explicit Config(std::string_view filename);

		static void write_dummy_config(std::string_view path);

		[[nodiscard]] std::string app_id() const;
		[[nodiscard]] std::string token() const;
		[[nodiscard]] uint64_t guild_id() const;
		[[nodiscard]] std::vector<TargetInfo> targets() const;
		[[nodiscard]] std::string reaction() const;
		[[nodiscard]] uint8_t response_probability() const;
		[[nodiscard]] std::vector<std::string> replies() const;
		[[nodiscard]] std::vector<std::string> on_delete_replies() const;
	private:
		nlohmann::json config_;

	private:
		template<typename T>
		[[nodiscard]] T get(const std::string& field_name) const {
			return config_.at(field_name).get<T>();
		}

		template<typename T>
		[[nodiscard]] T get(std::initializer_list<std::string> field_path) const {
			nlohmann::json obj = config_;
			for (const auto& path : field_path) {
				obj = obj.at(path);
			}
			return obj.get<T>();
		}
	};
}