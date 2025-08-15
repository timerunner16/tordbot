#include <vector>
#include <string>

class string_utils {
public:
	static std::vector<std::string> split_string(std::string input, std::string delimiter) {
		std::vector<std::string> output = std::vector<std::string>();
		std::string remaining_string = input;
		size_t pos = remaining_string.find(delimiter);
		while (pos != std::string::npos) {
			std::string current = remaining_string.substr(0, pos);
			output.push_back(current);
			remaining_string.erase(0,pos+delimiter.size());
			pos = remaining_string.find(delimiter);
		}
		output.push_back(remaining_string);
		return output;
	}
	
	static std::vector<std::string> tokenize_string(std::string input, std::string delimiter) {
		std::vector<std::string> tokens(0);
		size_t startpos = 0, currentpos = input.find_first_of(delimiter);
		do {
			tokens.push_back(input.substr(startpos, currentpos-startpos));
			tokens.push_back(input.substr(currentpos, 1));
			startpos = currentpos + 1;
			currentpos = input.find_first_of(delimiter, startpos);
		} while(currentpos != std::string::npos);

		tokens.push_back(input.substr(startpos, currentpos-startpos+1));

		return tokens;
	}
};
