#pragma once
#include <dpp/dpp.h>
#include <sqlite3.h>
#include <vector>

#define PING "ping"


#define QUESTION "question"

#define CATEGORY_PARAM "category"
#define RATING_PARAM "rating"

#define TRUTH "truth"
#define DARE "dare"
#define WYR "wyr"
#define NHIE "nhie"
#define PARANOIA "paranoia"

#define PG "pg"
#define PG13 "pg13"
#define R "r"

#define RANDOM "random"

#define RECENT_IDS_SIZE 100


#define CONFESS "confess"

#define CONFESSION_PARAM "confession"
#define PIRATIZE_PARAM "piratize"


#define SET_CONFESSION_CHANNEL "set_confession_channel"

struct question_t {
	std::string question;
	std::string category;
	std::string rating;
};

class bot_commands {
	public:
		static void ping(const dpp::slashcommand_t& event);
		static void question(const dpp::slashcommand_t& event);
		static void question_frombtn(const dpp::button_click_t& event, std::string rating, std::string category);
		static void confess(const dpp::slashcommand_t& event);
		static void set_confession_channel(const dpp::slashcommand_t& event);
		static void prepare_settings_db();
		static void close_settings_db();

		static dpp::cluster* bot;
	private:
		static std::string get_question(std::string category, std::string rating);
		static void set_confession_channel_internal(dpp::snowflake guild_id, dpp::snowflake channel_id);
		static bool guild_exists(dpp::snowflake guild_id);
		static dpp::snowflake get_confession_channel(dpp::snowflake guild_id);

		static std::vector<std::string> recent_ids;
		static sqlite3* sqlite3_con;
};
