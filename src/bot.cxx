#include <algorithm>
#include <map>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include "bot.hpp"
#include "dpp/unicode_emoji.h"
#include "util.hpp"
using json=nlohmann::json;

const std::string categories[5] = {TRUTH, DARE, WYR, NHIE, PARANOIA};
const std::string ratings[2] = {PG, PG13};

dpp::cluster* bot_commands::bot = nullptr;
sqlite3* bot_commands::sqlite3_con = nullptr;
std::vector<std::string> bot_commands::recent_ids = std::vector<std::string>();

std::map<std::string, std::string> filters({
	std::make_pair("kiss", "hug"),
	std::make_pair("kissing", "hugging"),
	std::make_pair("kissed", "hugged"),
});

std::vector<std::string> confession_footers({
	"Ye'v been naughty.",
	"Bloody hell, mate...",
	"What the hell, man?",
	"What is wrong with you?!",
	"Oooooooh!",
	dpp::unicode_emoji::fearful_face,
	dpp::unicode_emoji::exploding_head,
	"Jesus Christ...",
	"3:",
	":c",
	"I feel unsafe here.",
	"I'm calling the cops!",
	"AAAAAAAAHHHH!",
	"You freaky git...",
	"Booooooo!",
	"Get off the stage!"
});

std::map<std::string, std::string> piracy({
	std::make_pair("hello", "ahoy"),
	std::make_pair("hey", "ahoy"),
	std::make_pair("greetings", "ahoy"),
	std::make_pair("yes", "aye"),
	std::make_pair("no", "nay"),
	std::make_pair("omg", "blimey"),
	std::make_pair("friend", "bucko"),
	std::make_pair("friends", "shipmates"),
	std::make_pair("money", "booty"),
	std::make_pair("cash", "booty"),
	std::make_pair("awful", "bilge-sucking"),
	std::make_pair("song", "shanty"),
	std::make_pair("songs", "shanties"),
	std::make_pair("sword", "cutlass"),
	std::make_pair("swords", "cutlasses"),
	std::make_pair("gun", "cannon"),
	std::make_pair("guns", "cannons"),
	std::make_pair("pistol", "flintlock"),
	std::make_pair("died", "visited davy jones' locker"),
	std::make_pair("dies", "visits davy jones' locker"),
	std::make_pair("die", "visit davy jones' locker"),
	std::make_pair("dying", "visiting davy jones' locker"),
	std::make_pair("coin", "doubloon"),
	std::make_pair("coins", "doubloons"),
	std::make_pair("alcohol", "grog"),
	std::make_pair("drink", "grog"),
	std::make_pair("drinks", "grog"),
	std::make_pair("kitchen", "galley"),
	std::make_pair("kitchens", "gallies"),
	std::make_pair("bathroom", "head"),
	std::make_pair("bathrooms", "heads"),
	std::make_pair("restroom", "head"),
	std::make_pair("restrooms", "heads"),
	std::make_pair("stop","heave to"),
	std::make_pair("stops","heaves to"),
	std::make_pair("stopping","heaving to"),
	std::make_pair("stopped","heaved to"),
	std::make_pair("kid", "youngin"),
	std::make_pair("kids", "youngins"),
	std::make_pair("boy", "lad"),
	std::make_pair("boys", "lads"),
	std::make_pair("guy", "lad"),
	std::make_pair("guys", "lads"),
	std::make_pair("girl", "lass"),
	std::make_pair("girl", "lasses"),
	std::make_pair("my", "me"),
	std::make_pair("am", "be"),
	std::make_pair("im", "i be"),
	std::make_pair("i'm", "i be"),
	std::make_pair("is", "be"),
	std::make_pair("everybody", "crew"),
	std::make_pair("everyone", "crew"),
	std::make_pair("telescope", "spyglass"),
	std::make_pair("drunk", "squiffy"),
	std::make_pair("intoxicated", "squiffy"),
	std::make_pair("nap", "caulk"),
	std::make_pair("woman", "wench"),
	std::make_pair("man", "dandy"),
	std::make_pair("you", "ye"),
	std::make_pair("yippee", "yo-ho-ho"),
	std::make_pair("yay", "arrrgh"),
	std::make_pair("wow", "blow me down"),
	std::make_pair("maybe", "mayhaps"),
	std::make_pair("classmate", "hand"),
	std::make_pair("classmates", "hands"),
	std::make_pair("ok", "savvy"),
	std::make_pair("steal", "pirate"),
	std::make_pair("steals", "pirates"),
	std::make_pair("stealing", "pirating"),
	std::make_pair("stole", "pirated"),
	std::make_pair("sailor", "tar"),
	std::make_pair("sailors", "tars"),
	std::make_pair("citizen", "landlubber"),
	std::make_pair("citizens", "landlubbers"),
	std::make_pair("pedestrian", "landlubber"),
	std::make_pair("pedestrians", "landlubbers"),
});

std::string lower(std::string input) {
	std::string result = input;
	std::transform(result.begin(), result.end(), result.begin(), ::tolower);
	return result;
}

std::string filter_text(std::string input) {
	for (auto& [original, replacement] : filters) {
		size_t pos = lower(input).find(lower(original));
		while (pos != input.npos) {
			char following = input[pos+original.length()];
			if (following == ' ' || following == '?' || following == '.' || following == '\'' || following == '"') {
				bool capitalized = std::isupper(input[pos]);
				input.replace(pos, original.length(), lower(replacement));
				if (capitalized) input[pos] = std::toupper(input[pos]);
			}
			pos = input.find(original, pos+1);
		}
	}
	return input;
}

std::string pirate_media(std::string media) {
	std::vector<std::string> tokens = string_utils::tokenize_string(media, "\r\n\t ,,.<>?!()[]{}-_=+\\|;:\"@#$%^&*`~");
	for (size_t i = 0; i < tokens.size(); i++) {
		std::string lower_media = tokens[i];
		std::transform(lower_media.begin(), lower_media.end(), lower_media.begin(),
			[](unsigned char c){ return std::tolower(c); });
		if (piracy.find(lower_media) != piracy.end()) {
			tokens[i] = piracy[lower_media];
		} else tokens[i] = lower_media;
	}

	std::string result;
	for (std::string i : tokens) result += i;

	return result;
}

std::string bot_commands::get_question(std::string category, std::string rating) {
	bool success = false;
	json j;
	int count = 0;
	while (!success) {
		cpr::Response r = cpr::Get(cpr::Url{"https://api.truthordarebot.xyz/v1/" + category},
							   cpr::Parameters{{RATING_PARAM,rating}});
		if (r.status_code != 200) {}

		std::string raw_json = r.text;
		j = json::parse(raw_json);
		std::string id = j["id"];
		bool found = false;
		for (auto i : recent_ids) {
			found = found || i == id;
		}
		if (!found || ++count == 5) success = true;
	}

	recent_ids.insert(recent_ids.begin(), j["id"]);
	if (recent_ids.size() > RECENT_IDS_SIZE) recent_ids.pop_back();
	return j["question"];
}

dpp::message create_message(std::string question, std::string category, std::string rating, std::string category_use, std::string rating_use, dpp::user user, dpp::snowflake channel_id) {
	std::string author = "Requested by " + user.username;
	std::string author_avatar = user.get_avatar_url();
	std::string category_formatted = category_use;
	std::string rating_formatted = rating_use;
	std::transform(category_formatted.begin(), category_formatted.end(), category_formatted.begin(), ::toupper);
	std::transform(rating_formatted.begin(), rating_formatted.end(), rating_formatted.begin(), ::toupper);
	std::string footer = "Category: " + category_formatted + " | Rating: " + rating_formatted;

	std::string question_filtered = question;
	question_filtered = filter_text(question_filtered);

	dpp::embed embed = dpp::embed()
		.set_color(0x5534EFFF)
		.set_title(question_filtered)
		.set_author(author, "", author_avatar)
		.set_footer(footer, "");
	dpp::message message = dpp::message(channel_id, embed);
	if (channel_id != dpp::snowflake()) {
		message.add_component(
			dpp::component().add_component(
				dpp::component()
					.set_label("New Question")
					.set_type(dpp::cot_button)
					.set_style(dpp::cos_primary)
					.set_id(std::string(QUESTION)+";"+rating+";"+category)
		));
	}
	return message;
}

void bot_commands::ping(const dpp::slashcommand_t& event) {
	dpp::embed embed = dpp::embed()
		.set_color(0xE0B92BFF)
		.set_title("Howdy!")
		.set_author(event.command.usr.username, "", event.command.usr.get_avatar_url())
		.set_footer(":]", "");
	dpp::message message = dpp::message(embed);
	event.reply(message);
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

	std::string question = get_question(category_use, rating_use);
	dpp::snowflake channel_id = dpp::snowflake();
	if (event.command.is_guild_interaction()) channel_id = event.command.get_channel().id;
	event.reply(create_message(question, category, rating, category_use, rating_use, event.command.usr, channel_id));
}

void bot_commands::question_frombtn(const dpp::button_click_t& event, std::string rating, std::string category) {
	std::string category_use = category;
	std::string rating_use = rating;

	if (category == RANDOM) category_use = categories[rand()%(sizeof(categories)/sizeof(std::string))];
	if (rating == RANDOM) rating_use = ratings[rand()%(sizeof(ratings)/sizeof(std::string))];

	std::string question = get_question(category_use, rating_use);

	if (bot != nullptr) {
		bot->message_get(event.command.message_id, event.command.channel_id, [](const dpp::confirmation_callback_t& callback) {
			if (callback.is_error()) {std::cout << callback.get_error().message << std::endl; return;}
			dpp::message message = callback.get<dpp::message>();

			message.components.clear();
			bot->message_edit(message);
		});
		
		event.reply(create_message(question, category, rating, category_use, rating_use, event.command.usr, event.command.get_channel().id));
	} else {
		event.reply(create_message(question, category, rating, category_use, rating_use, event.command.usr, event.command.get_channel().id));
	}
}

void bot_commands::confess(const dpp::slashcommand_t &event) {
	dpp::snowflake channel_id = get_confession_channel(event.command.guild_id);
	if (!channel_id.empty()) event.reply(dpp::message("Got your confession, I'll be exposing you momentarily.").set_flags(dpp::m_ephemeral));
	else {
		event.reply(dpp::message("A confession channel hasn't been set in this server.").set_flags(dpp::m_ephemeral));
		return;
	}

	std::string confession;
	try {
		confession = std::get<std::string>(event.get_parameter(CONFESSION_PARAM));
	} catch (const std::bad_variant_access& e) {
		confession = "balls";
	}

	bool piratize;
	try {
		piratize = std::get<bool>(event.get_parameter(PIRATIZE_PARAM));
	} catch (const std::bad_variant_access& e) {
		piratize = true;
	}

	if (piratize) confession = pirate_media(confession);
	dpp::embed embed = dpp::embed()
		.set_color(0xB86A60FF)
		.set_title("Confession")
		.set_description(confession)
		.set_footer("\"" + confession_footers[rand()%confession_footers.size()] + "\"", "");

	std::this_thread::sleep_for(std::chrono::seconds(30+rand()%60));

	dpp::message message = dpp::message(channel_id, embed);
	bot->message_create(message);
}

void bot_commands::prepare_settings_db() {
	sqlite3_open("./tord_settings.sqlite3", &sqlite3_con);
	sqlite3_exec(sqlite3_con, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
	sqlite3_exec(sqlite3_con, "CREATE TABLE IF NOT EXISTS settings(GuildID VARCHAR(20), ConfessionChannelID VARCHAR(20));", nullptr, nullptr, nullptr);
	sqlite3_exec(sqlite3_con, "END TRANSACTION;", nullptr, nullptr, nullptr);
}

void bot_commands::close_settings_db() {
	sqlite3_close(sqlite3_con);

	sqlite3_con = nullptr;
}

bool bot_commands::guild_exists(dpp::snowflake guild_id) {
	if (sqlite3_con == nullptr) return false;

	bool found = false;

	std::string sql = "SELECT GuildID FROM settings WHERE GuildID = " + guild_id.str() + ";";
	auto callback = [](void* found, int colc, char**, char**) -> int {
		*(bool*)found = colc > 0;
		return 0;
	};
	sqlite3_exec(sqlite3_con, sql.c_str(), callback, &found, nullptr);
	return found;
}

void bot_commands::set_confession_channel_internal(dpp::snowflake guild_id, dpp::snowflake channel_id) {
	if (sqlite3_con == nullptr) return;

	bool exists = guild_exists(guild_id);

	sqlite3_exec(sqlite3_con, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
	
	std::string sql = "";

	if (exists) sql = "UPDATE OR ABORT SET ConfessionChannelID = " + channel_id.str() + " FROM settings WHERE GuildID = " + guild_id.str() + ";";
	else sql = "INSERT INTO settings VALUES (" + guild_id.str() + ", " + channel_id.str() + " );";

	sqlite3_exec(sqlite3_con, sql.c_str(), nullptr, nullptr, nullptr);
	sqlite3_exec(sqlite3_con, "END TRANSACTION;", nullptr, nullptr, nullptr);
}

dpp::snowflake bot_commands::get_confession_channel(dpp::snowflake guild_id) {
	if (sqlite3_con == nullptr) return dpp::snowflake();
	
	dpp::snowflake channel_id = dpp::snowflake();
	std::string sql = "SELECT ConfessionChannelID FROM settings WHERE GuildID = " + guild_id.str() + ";";
	std::string channel_id_str = "";
	auto callback = [](void* str, int colc, char** values, char**) -> int {
		if (colc > 0) *(std::string*)str = std::string(values[0]);
		return 0;
	};
	sqlite3_exec(sqlite3_con, sql.c_str(), callback, &channel_id_str, nullptr);
	return dpp::snowflake(channel_id_str);
}

void bot_commands::set_confession_channel(const dpp::slashcommand_t &event) {
	dpp::channel c = event.command.get_channel();
	if (c.get_user_permissions(&event.command.usr).can(dpp::p_manage_channels)) {
		set_confession_channel_internal(event.command.guild_id, event.command.channel_id);
		dpp::embed embed = dpp::embed()
			.set_color(0xDDE339FF)
			.set_title("Setting Change")
			.set_description("Set this channel (" + c.name + ") to be the active confession channel.")
			.set_footer("Change made by " + event.command.usr.username, event.command.usr.get_avatar_url());
		event.reply(embed);
	} else {
		event.reply(dpp::message("You don't have permission to run this command; you need the Manage Channels permission.").set_flags(dpp::m_ephemeral));
	}
}
