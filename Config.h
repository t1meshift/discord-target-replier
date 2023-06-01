#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace griha {
	class Config {
	public:
		explicit Config(std::string_view filename);

		static void write_dummy_config(std::string_view path);

		[[nodiscard]] std::string app_id() const;
		[[nodiscard]] std::string token() const;
		[[nodiscard]] uint64_t guild_id() const;
		[[nodiscard]] std::string target_username() const;
		[[nodiscard]] uint16_t target_discriminator() const;
		[[nodiscard]] std::string reaction() const;
		[[nodiscard]] uint8_t response_probability() const;
		[[nodiscard]] std::vector<std::string> replies() const;
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