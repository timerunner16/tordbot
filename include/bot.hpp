#pragma once
#include <dpp/dpp.h>
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
		static dpp::cluster* bot;
	private:
		static std::string get_question(std::string category, std::string rating);
		static std::vector<std::string> recent_ids;
};
