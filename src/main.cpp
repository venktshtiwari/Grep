#include <cstddef>
#include <iostream>
#include <ostream>
#include <string>
#include <cctype>
#include <stdexcept>
#include <string_view>

bool match_here(std::string_view input, std::string_view pattern);

bool match_pattern(std::string_view input, std::string_view pattern) {
    if (!pattern.empty() && pattern[0] == '^') {
        return match_here(input, pattern.substr(1));
    }
    for (size_t i=0; i<=input.length(); i++) {
	if (match_here(input.substr(i), pattern)) {
	    return true;
	}
    }
    return false;
}

bool match_here(std::string_view input, std::string_view pattern) {
    if (pattern.empty()) return true;
    if (pattern == "$") return input.empty();

    // ---------------ALTERNATION--------------------------
    if (pattern[0]=='(') {
	size_t end_paren = pattern.find(')');
	if (end_paren == std::string::npos) throw std::runtime_error("Malformed parenthesis group.");
	std::string_view group = pattern.substr(1,end_paren-1);
	std::string_view rest_pattern = pattern.substr(end_paren+1);

	size_t start = 0;
	while (1) {
	    size_t pipe = group.find('|', start);
	    std::string_view alt = (pipe == std::string::npos ? group.substr(start) : group.substr(start, pipe-start));
	    std::string combined = std::string(alt) + std::string(rest_pattern);
	    if (match_here(input, combined)) return true;
	    if (pipe == std::string::npos) break;
	    start = pipe + 1;
	}
	return false;
    }

    // ---------------PARSING TOKENS-----------------------
    size_t token_len = 0;
    bool is_group = false;
    bool is_negative = false;
    std::string_view group_contents;
    
    if (pattern[0] == '\\') {
	if (pattern.length() < 2) throw std::runtime_error("Malformed escape sequence");
	token_len = 2;
    }
    else if (pattern[0] == '[') {
	size_t end_bracket = pattern.find(']');
	if (end_bracket == std::string::npos) throw std::runtime_error("Malformed character class");
	token_len = end_bracket+1;
	is_group = true;
	group_contents = pattern.substr(1,end_bracket-1);

	if (!group_contents.empty() && group_contents[0]=='^') {
	    is_negative = true;
	    group_contents = group_contents.substr(1);
	}
    }
    else token_len = 1;
    bool has_plus = (pattern.length() > token_len && pattern[token_len]=='+');
    bool has_ques = (pattern.length() > token_len && pattern[token_len]=='?');

    // ---------------------------------------------------------

    auto match_token = [&] (char ch) -> bool {
	if (pattern[0]=='\\') {
	    if (pattern[1]=='d') return std::isdigit(ch);
	    if (pattern[1]=='w') return std::isalnum(ch) || ch=='_';
	    return false;
	}
	if (is_group) {
	    bool found = (group_contents.find(ch) != std::string::npos);
	    return is_negative ? !found : found;
	}
	return (ch == pattern[0] || pattern[0]=='.');
    };

    if (has_plus) {
	size_t match_count = 0;

	while (match_count < input.length() && match_token(input[match_count])) {
	    match_count++;
	}
	if (!match_count) return false;
	std::string_view rest_pattern = pattern.substr(token_len+1);

	while (match_count) {
	    if (match_here(input.substr(match_count),rest_pattern)) return true;
	    match_count--;
	}
	return false;
    }
    else if (has_ques) {
	std::string_view rest_pattern = pattern.substr(token_len + 1);
	if (!input.empty() && match_token(input[0])) {
	    if (match_here(input.substr(1), rest_pattern)) return true;
	}
	return match_here(input, rest_pattern);
    }
    else {
	if (input.empty() || !match_token(input[0])) return false;
	return match_here(input.substr(1), pattern.substr(token_len));
    }
}

int main(int argc, char* argv[]) {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // You can use print statements as follows for debugging, they'll be visible when running tests.
    std::cerr << "Logs from your program will appear here" << std::endl;

    if (argc != 3) {
        std::cerr << "Expected two arguments" << std::endl;
        return 1;
    }

    std::string flag = argv[1];
    std::string pattern = argv[2];

    if (flag != "-E") {
        std::cerr << "Expected first argument to be '-E'" << std::endl;
        return 1;
    }

    std::string input_line;
    bool any_match = false;

    while (std::getline(std::cin, input_line)) {
	try {
	    if (match_pattern(input_line, pattern)) {
		std::cout << input_line << std::endl;
		any_match = true;
	    }
	} catch (const std::runtime_error& e) {
	    std::cerr << e.what() << std::endl;
	    return 1;
	}
    }
    return any_match ? 0 : 1;
}

