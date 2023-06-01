#include "Config.h"

#include <fstream>

namespace griha {
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

	std::string Config::target_username() const {
		return get<std::string>({ "target", "username" });
	}

	uint16_t Config::target_discriminator() const {
		return get<uint16_t>({ "target", "discriminator" });
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
}