#include <algorithm>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include "bot.hpp"
using json=nlohmann::json;

const std::string categories[5] = {TRUTH, DARE, WYR, NHIE, PARANOIA};
const std::string ratings[2] = {PG, PG13};

dpp::cluster* bot_commands::bot = nullptr;

dpp::message create_message(std::string question, std::string category, std::string rating, std::string category_use, std::string rating_use, dpp::user user, dpp::snowflake channel_id) {
	std::string author = "Requested by " + user.username;
	std::string author_avatar = user.get_avatar_url();
	std::string category_formatted = category_use;
	std::string rating_formatted = rating_use;
	std::transform(category_formatted.begin(), category_formatted.end(), category_formatted.begin(), ::toupper);
	std::transform(rating_formatted.begin(), rating_formatted.end(), rating_formatted.begin(), ::toupper);
	std::string footer = "Category: " + category_formatted + " | Rating: " + rating_formatted;

	dpp::embed embed = dpp::embed()
		.set_color(0x5534EFFF)
		.set_title(question)
		.set_author(author, "", author_avatar)
		.set_footer(footer, "");
	dpp::message message = dpp::message(channel_id, embed);
	message.add_component(
		dpp::component().add_component(
			dpp::component()
				.set_label("New Question")
				.set_type(dpp::cot_button)
				.set_style(dpp::cos_primary)
				.set_id(std::string(QUESTION)+";"+rating+";"+category)
	));
	return message;
}

void bot_commands::ping(const dpp::slashcommand_t& event) {
	event.reply("Howdy!");
}

void bot_commands::question(const dpp::slashcommand_t& event) {
	auto category_value = event.get_parameter(CATEGORY_PARAM);
	auto rating_value = event.get_parameter(RATING_PARAM);
	
	std::string category;
	std::string rating;
	
	try {
		category = std::get<std::string>(category_value);
	} catch (const std::bad_variant_access& e) {
		category = RANDOM;
	}
	try {
		rating = std::get<std::string>(rating_value);
	} catch (const std::bad_variant_access& e) {
		rating = RANDOM;
	}
	
	std::string category_use = category;
	std::string rating_use = rating;

	if (category == RANDOM) category_use = categories[rand()%(sizeof(categories)/sizeof(std::string))];
	if (rating == RANDOM) rating_use = ratings[rand()%(sizeof(ratings)/sizeof(std::string))];

	cpr::Response r = cpr::Get(cpr::Url{"https://api.truthordarebot.xyz/v1/" + category_use},
							   cpr::Parameters{{RATING_PARAM,rating_use}});
	if (r.status_code != 200) {
		event.reply(
				"Couldn't get data from truthordarebot API.\n**Status code:** " +
				std::to_string(r.status_code) +
				"\n**Error:** " + 
				r.error.message);
		return;
	}

	std::string raw_json = r.text;
	auto j = json::parse(raw_json);
	event.reply(create_message(j["question"], category, rating, category_use, rating_use, event.command.usr, event.command.get_channel().id));
}

void bot_commands::question_frombtn(const dpp::button_click_t& event, std::string rating, std::string category) {
	std::string category_use = category;
	std::string rating_use = rating;

	if (category == RANDOM) category_use = categories[rand()%(sizeof(categories)/sizeof(std::string))];
	if (rating == RANDOM) rating_use = ratings[rand()%(sizeof(ratings)/sizeof(std::string))];

	cpr::Response r = cpr::Get(cpr::Url{"https://api.truthordarebot.xyz/v1/" + category_use},
							   cpr::Parameters{{RATING_PARAM,rating_use}});
	if (r.status_code != 200) {
		event.reply(
				"Couldn't get data from truthordarebot API.\n**Status code:** " +
				std::to_string(r.status_code) +
				"\n**Error:** " + 
				r.error.message);
		return;
	}

	std::string raw_json = r.text;
	auto j = json::parse(raw_json);
	if (bot != nullptr) {
		bot->message_get(event.command.message_id, event.command.channel_id, [](const dpp::confirmation_callback_t& callback) {
			if (callback.is_error()) {std::cout << callback.get_error().message << std::endl; return;}
			dpp::message message = callback.get<dpp::message>();

			message.components.clear();
			bot->message_edit(message);
		});
		
		event.reply(create_message(j["question"], category, rating, category_use, rating_use, event.command.usr, event.command.get_channel().id));
	} else {
		event.reply(create_message(j["question"], category, rating, category_use, rating_use, event.command.usr, event.command.get_channel().id));
	}
}
