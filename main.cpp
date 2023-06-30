#include <iostream>
#include <filesystem>
#include <random>
#include <dpp/dpp.h>
#include <fmt/format.h>
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
	const auto on_delete_reply_list = config.on_delete_replies();
	const auto targets = config.targets();

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

	bot.on_message_create([&bot, &reply_list, &targets, config, &gen](const dpp::message_create_t& event) {
		const auto& author_data = event.msg.author;

		auto found_target = std::find_if(targets.cbegin(), targets.cend(), [&author_data](const auto& v) {
			return author_data.username == v.username &&
				author_data.discriminator == v.discriminator;
		});

		if (found_target == targets.cend()) {
			return;
		}

		if (!config.reaction().empty()) {
			bot.message_add_reaction(event.msg, config.reaction());
		}

		std::uniform_int_distribution<> response_dist(0, 100);
		if (response_dist(gen) < 100 - config.response_probability()) {
			return;
		}

		const auto& custom_replies = found_target->additional.replies;
		std::vector<std::string> total_replies;
		total_replies.reserve(reply_list.size() + custom_replies.size());
		total_replies.insert(total_replies.end(),
			custom_replies.begin(), custom_replies.end());
		total_replies.insert(total_replies.end(),
			reply_list.begin(), reply_list.end());

		if (!total_replies.empty()) {
			std::uniform_int_distribution<size_t> reply_dist(0, total_replies.size() - 1);
			const auto& reply_format = total_replies.at(reply_dist(gen));

			const auto reply_contents = fmt::format(reply_format,
				fmt::arg("user",
					fmt::format("<@{}>",
						static_cast<uint64_t>(author_data.id))));

			dpp::message msg(event.msg.channel_id, reply_contents);
			event.reply(msg);
		}
	});

	bot.on_message_delete([&bot, &on_delete_reply_list, &targets, config, &gen](const dpp::message_delete_t& event) {
		const auto& author_data = event.deleted->author;

		auto found_target = std::find_if(targets.cbegin(), targets.cend(), [&author_data](const auto& v) {
			return author_data.username == v.username &&
				author_data.discriminator == v.discriminator;
		});

		if (found_target == targets.cend()) {
			return;
		}

		std::uniform_int_distribution<> response_dist(0, 100);
		if (response_dist(gen) < 100 - config.response_probability()) {
			return;
		}

		const auto& custom_replies = found_target->additional.on_delete_replies;
		std::vector<std::string> total_replies;
		total_replies.reserve(on_delete_reply_list.size() + custom_replies.size());
		total_replies.insert(total_replies.end(),
			custom_replies.begin(), custom_replies.end());
		total_replies.insert(total_replies.end(),
			on_delete_reply_list.begin(), on_delete_reply_list.end());

		if (!total_replies.empty()) {
			std::uniform_int_distribution<size_t> reply_dist(0, total_replies.size() - 1);
			const auto& reply_format = total_replies.at(reply_dist(gen));

			const auto reply_contents = fmt::format(reply_format,
				fmt::arg("user",
					fmt::format("<@{}>",
						static_cast<uint64_t>(author_data.id))));

			dpp::message msg(event.deleted->channel_id, reply_contents);
			bot.message_create(msg);
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
