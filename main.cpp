#include <iostream>
#include <filesystem>
#include <random>
#include <dpp/dpp.h>
#include "Config.h"

int main() {
	const std::string config_path = "config.json";

	if (!std::filesystem::exists(config_path)) {
		std::cout << "Configuration file is not found!" << std::endl;
		std::cout << "Generating dummy..." << std::endl;
		griha::Config::write_dummy_config(config_path);
	}

	griha::Config config(config_path);
	const auto reply_list = config.replies();

	// Validate config file
	if (config.app_id().empty()) {
		std::cout << "App ID is not set, generate your own app here:" << std::endl;
		std::cout << "https://discord.com/developers/applications" << std::endl;
		return 1;
	}
	if (config.token().empty()) {
		std::cout << "Bot token is not set, get it here:" << std::endl;
		std::cout << "https://discord.com/developers/applications/"
			<< config.app_id() << "/bot" << std::endl;
		return 1;
	}
	if (config.guild_id() == 0) {
		std::cout << "Guild ID is not set, you can google how to get it:" << std::endl;
		std::cout << "https://www.google.com/search?q=discord+get+guild+id" << std::endl;
		return 1;
	}

	dpp::cluster bot(config.token(), dpp::i_all_intents);
	bot.on_log(dpp::utility::cout_logger());

	std::random_device rd;
	std::mt19937 gen(rd());

	bot.on_message_create([&bot, &reply_list, config, &gen](const dpp::message_create_t& event) {
		auto username = config.target_username();
		auto discriminator = config.target_discriminator();

		const auto& author_data = event.msg.author;
		if (author_data.username != username
			|| author_data.discriminator != discriminator) {
			return;
		}

		if (!config.reaction().empty()) {
			bot.message_add_reaction(event.msg, config.reaction());
		}

		std::uniform_int_distribution<> response_dist(0, 100);
		if (response_dist(gen) < 100 - config.response_probability()) {
			return;
		}

		if (!reply_list.empty()) {
			std::uniform_int_distribution<size_t> reply_dist(0, reply_list.size() - 1);

			const auto& reply_contents = reply_list.at(reply_dist(gen));

			dpp::message msg(event.msg.channel_id, reply_contents);
			event.reply(msg);
		}
	});

	bot.on_ready([&bot, &config](const dpp::ready_t& event) {
		if (dpp::run_once<struct get_auth_link>()) {
			std::cout << "Add this bot with the link below:" << std::endl;
			std::cout << "https://discord.com/api/oauth2/authorize?client_id="
			 + config.app_id() + "&permissions=137439292480"
			 "&scope=bot%20applications.commands" << std::endl << std::endl;
		}
		if (dpp::run_once<struct print_available_emojis>()) {
			auto available_emojis = bot.guild_emojis_get_sync(config.guild_id());
			std::cout << "Available emojis for guild ID " << config.guild_id() << ":" << std::endl;
			for (const auto& [id, emoji] : available_emojis) {
				std::cout << "* " << emoji.format() << std::endl;
			}
		}
	});

	bot.start(dpp::st_wait);
	return 0;
}
