#include "parser.hpp"

#include <optional>
#include <unordered_map>

std::string ErrorToString(ParseError err) {
    switch (err) {
        case ParseError::UNCLOSED_DOUBLE_QUOTE:
            return "Syntax error: unclosed double quote (\")";
        case ParseError::UNCLOSED_SINGLE_QUOTE:
            return "Syntax error: unclosed single quote (')";
        case ParseError::EXPECTED_FILENAME_AFTER_REDIRECT:
            return "Syntax error: expected file name after '>'";
        case ParseError::TRAILING_ESCAPE:
            return "Syntax error: backslash at end of line";
        default: return "Unknown parse error";
    }
}


static const std::unordered_map<std::string_view, RedirectType> kRedirectionTypes = {
    {">",  RedirectType::STDOUT_REWRITE},
    {"1>",  RedirectType::STDOUT_REWRITE},
    {">>", RedirectType::STDOUT_APPEND},
    {"1>>", RedirectType::STDOUT_APPEND},
    {"2>", RedirectType::STDERR},
    {"2>>", RedirectType::STDERR_APPEND},
};


std::expected<ParseResult, ParseError> ShellParser::Parse(std::string_view command) {
    ParseResult res;
    size_t i = 0;

    while (i < command.length()) {
        while (i < command.length() && std::isspace(static_cast<unsigned char>(command[i]))) {
            i++;
        }
        if (i >= command.length()) {
            break;
        }

        std::optional<std::pair<std::string_view, RedirectType>> found_redirect;

        auto lens = {3, 2, 1};
        auto it = std::ranges::find_if(lens, [&](size_t len) {
            return i + len <= command.length() && kRedirectionTypes.contains(command.substr(i, len));
        });

        if (it != lens.end()) {
            std::string_view part = command.substr(i, *it);
            found_redirect = {part, kRedirectionTypes.at(part)};
        }

        if (found_redirect) {
            i += found_redirect->first.length();

            auto word_res = ExtractNextWord(command, i);
            if (!word_res.has_value()) {
                return std::unexpected(word_res.error());
            }

            std::string filename = std::move(word_res.value());
            if (filename.empty()) {
                return std::unexpected(ParseError::EXPECTED_FILENAME_AFTER_REDIRECT);
            }

            res.redirections.push_back({std::move(filename), found_redirect->second});
            continue;
        }

        auto word_res = ExtractNextWord(command, i);
        if (!word_res.has_value()) {
            return std::unexpected(word_res.error());
        }

        if (!word_res->empty()) {
            res.tokens.push_back(std::move(*word_res));
        }
    }

    return res;
}


std::expected<std::string, ParseError> ShellParser::ExtractNextWord(std::string_view command, size_t &i) {
    std::string word;
    bool in_double_quotes = false;
    bool in_single_quotes = false;

    while (i < command.length() && std::isspace(static_cast<unsigned char>(command[i]))) {
        i++;
    }

    while (i < command.length()) {
        char c = command[i];

        if (!in_double_quotes && !in_single_quotes) {
            if (std::isspace(static_cast<unsigned char>(c)) || c == '>') {
                break;
            }
        }

        if (c == '\'' && !in_double_quotes) {
            in_single_quotes = !in_single_quotes;
        }
        else if (c == '"' && !in_single_quotes) {
            in_double_quotes = !in_double_quotes;
        }
        else if (c == '\\' && !in_single_quotes) {
            if (i + 1 >= command.length()) {
                return std::unexpected(ParseError::TRAILING_ESCAPE);
            }

            char next = command[i + 1];
            if (in_double_quotes) {
                if (next == '"' || next == '\\' || next == '$' || next == '`') {
                    word += next;
                    i++;
                }
                else {
                    word += c;
                }
            }
            else {
                word += next;
                i++;
            }
        }
        else {
            word += c;
        }
        i++;
    }

    if (in_double_quotes) {
        return std::unexpected(ParseError::UNCLOSED_DOUBLE_QUOTE);
    }
    if (in_single_quotes) {
        return std::unexpected(ParseError::UNCLOSED_SINGLE_QUOTE);
    }

    return word;
}
