#pragma once
#include <dpp/dpp.h>

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

class bot_commands {
	public:
		static void ping(const dpp::slashcommand_t& event);
		static void question(const dpp::slashcommand_t& event);
		static void question_frombtn(const dpp::button_click_t& event, std::string rating, std::string category);
		static dpp::cluster* bot;
};
