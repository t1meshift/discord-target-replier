#include "Config.h"

#include <fstream>

namespace griha {
	void to_json(nlohmann::json& j, const AdditionalTargetParams& t) {
		j = nlohmann::json{
			{"replies", t.replies},
			{"on_delete_replies", t.on_delete_replies}
		};
	}

	void from_json(const nlohmann::json& j, AdditionalTargetParams& t) {
		j.at("replies").get_to(t.replies);
		j.at("on_delete_replies").get_to(t.on_delete_replies);
	}

	void to_json(nlohmann::json& j, const TargetInfo& t) {
		j = nlohmann::json{
			{"username", t.username},
			{"discriminator", t.discriminator},
			{"additional", t.additional}
		};
	}

	void from_json(const nlohmann::json& j, TargetInfo& t) {
		j.at("username").get_to(t.username);
		j.at("discriminator").get_to(t.discriminator);
		j.at("additional").get_to(t.additional);
	}

	Config::Config(std::string_view filename) {
		std::ifstream f(filename.data());
		config_ = nlohmann::json::parse(f);
	}

	void Config::write_dummy_config(std::string_view path) {
		std::ifstream dummy("config.example.json");
		auto dummy_contents = nlohmann::json::parse(dummy);

		std::ofstream f(path.data());
		f << std::setw(4) << dummy_contents << std::endl;
	}

	std::string Config::app_id() const {
		return get<std::string>("app_id");
	}

	std::string Config::token() const {
		return get<std::string>("token");
	}

	uint64_t Config::guild_id() const {
		return get<uint64_t>("guild_id");
	}

	std::vector<TargetInfo> Config::targets() const {
		return get<std::vector<TargetInfo>>("target");
	}

	std::string Config::reaction() const {
		return get<std::string>("reaction");
	}

	uint8_t Config::response_probability() const {
		auto result = get<uint8_t>("response_probability");
		if (result > 100) {
			result = 100;
		}
		return result;
	}

	std::vector<std::string> Config::replies() const {
		return get<std::vector<std::string>>("replies");
	}

	std::vector<std::string> Config::on_delete_replies() const {
		return get<std::vector<std::string>>("on_delete_replies");
	}
}