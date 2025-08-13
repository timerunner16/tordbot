#include <dpp/dpp.h>
#include <string>
#include <vector>
#include "bot.hpp"
#include "dotenv.h"
using namespace dotenv;
std::string TOKEN;

constexpr unsigned int hashstr(const char* str, int h = 0) {return !str[h] ? 5381 : (hashstr(str, h+1) * 33) ^ str[h];}

std::vector<std::string> split_string(std::string input, std::string delimiter) {
	std::vector<std::string> output = std::vector<std::string>();
	std::string remaining_string = input;
	int pos = remaining_string.find(delimiter);
	while (pos != std::string::npos) {
		std::string current = remaining_string.substr(0, pos);
		output.push_back(current);
		remaining_string.erase(0,pos+delimiter.size());
		pos = remaining_string.find(delimiter);
	}
	output.push_back(remaining_string);
	return output;
}

int main() {
	env.load_dotenv();

	dpp::cluster* bot = new dpp::cluster(std::getenv("BOT_TOKEN"));

	bot->on_log(dpp::utility::cout_logger());

	bot_commands::bot = bot;

	bot->on_slashcommand([](const dpp::slashcommand_t& event) {
		switch (hashstr(event.command.get_command_name().c_str())) {
			case hashstr(PING): {bot_commands::ping(event); break;}
			case hashstr(QUESTION): {bot_commands::question(event); break;}
			case hashstr(CONFESS): {bot_commands::confess(event); break;}
		}
	});

	bot->on_button_click([](const dpp::button_click_t& event) {
		std::string data = event.custom_id;
		auto data_split = split_string(data, ";");
		std::string cmd = data_split[0];
		if (cmd == QUESTION) {
			std::string rating = data_split[1];
			std::string category = data_split[2];
			bot_commands::question_frombtn(event, rating, category);
		}
	});

	bot->on_ready([&bot](const dpp::ready_t& event) {
		if (dpp::run_once<struct register_bot_commands>()) {
			dpp::slashcommand ping_command(PING, "Pong!", bot->me.id);
			ping_command.set_interaction_contexts({
				dpp::itc_guild, dpp::itc_bot_dm, dpp::itc_private_channel
			});

			dpp::slashcommand question_command(QUESTION, "Ask a new question.", bot->me.id);
			question_command.add_option(
				dpp::command_option(dpp::co_string, CATEGORY_PARAM, "Category for the new question.", false)
					.add_choice(dpp::command_option_choice("Truth", std::string(TRUTH)))
					.add_choice(dpp::command_option_choice("Dare", std::string(DARE)))
					.add_choice(dpp::command_option_choice("WYR", std::string(WYR)))
					.add_choice(dpp::command_option_choice("NHIE", std::string(NHIE)))
					.add_choice(dpp::command_option_choice("Paranoia", std::string(PARANOIA)))
					.add_choice(dpp::command_option_choice("Random", std::string(RANDOM)))
			);
			question_command.add_option(
				dpp::command_option(dpp::co_string, RATING_PARAM, "Age rating for the new question.", false)
					.add_choice(dpp::command_option_choice("PG", std::string(PG)))
					.add_choice(dpp::command_option_choice("PG13", std::string(PG13)))
					.add_choice(dpp::command_option_choice("R", std::string(R)))
					.add_choice(dpp::command_option_choice("Random", std::string(RANDOM)))
			);
			question_command.set_interaction_contexts({
				dpp::itc_guild, dpp::itc_bot_dm, dpp::itc_private_channel
			});

			dpp::slashcommand confess_command(CONFESS, "Make an anonymous confession.", bot->me.id);
			confess_command.add_option(dpp::command_option(dpp::co_string, CONFESSION_PARAM, "The confession to send.", true));

			bot->global_command_create(ping_command);
			bot->global_command_create(question_command);
			bot->global_command_create(confess_command);
		}
	});

	bot->start(dpp::st_wait);

	return 0;
}
