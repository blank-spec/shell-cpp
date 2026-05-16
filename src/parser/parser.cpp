#include "parser.hpp"

#include <optional>
#include <unordered_map>
#include <cctype>

namespace {
    std::optional<std::pair<std::string_view, RedirectType>> GetRedirectAt(std::string_view cmd, size_t pos) {
        static const std::vector<std::pair<std::string_view, RedirectType>> redirs = {
            {"1>>", RedirectType::STDOUT_APPEND},
            {"2>>", RedirectType::STDERR_APPEND},
            {">>",  RedirectType::STDOUT_APPEND},
            {"1>",  RedirectType::STDOUT_REWRITE},
            {"2>",  RedirectType::STDERR},
            {">",   RedirectType::STDOUT_REWRITE},
            {"<",   RedirectType::STDIN}
        };

        for (const auto& [prefix, type] : redirs) {
            if (cmd.substr(pos).starts_with(prefix)) {
                return {{prefix, type}};
            }
        }
        return std::nullopt;
    }
}


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


std::expected<ParseResult, ParseError> ShellParser::Parse(std::string_view command) {
    ParseResult res;
    size_t i = 0;

    while (i < command.length()) {
        while (i < command.length() && std::isspace(static_cast<unsigned char>(command[i]))) {
            i++;
        }
        if (i >= command.length()) break;

        if (auto found_redirect = GetRedirectAt(command, i)) {
            i += found_redirect->first.length();

            auto word_res = ExtractNextWord(command, i);
            if (!word_res.has_value()) {
                return std::unexpected(word_res.error());
            }

            if (word_res->empty()) {
                return std::unexpected(ParseError::EXPECTED_FILENAME_AFTER_REDIRECT);
            }

            res.redirections.push_back({std::move(word_res.value()), found_redirect->second});
            continue;
        }

        auto word_res = ExtractNextWord(command, i);
        if (!word_res.has_value()) {
            return std::unexpected(word_res.error());
        }

        res.tokens.push_back(std::move(*word_res));
    }

    return res;
}


std::expected<std::string, ParseError> ShellParser::ExtractNextWord(std::string_view command, size_t &i) {
    std::string word;
    bool in_double = false;
    bool in_single = false;

    while (i < command.length() && std::isspace(static_cast<unsigned char>(command[i]))) {
        ++i;
    }

    if (i >= command.length()) {
        return "";
    }

    while (i < command.length()) {
        char c = command[i];

        if (!in_double && !in_single) {
            if (std::isspace(static_cast<unsigned char>(c))) {
                break;
            }
            if (GetRedirectAt(command, i)) {
                break;
            }
        }

        if (c == '\'' && !in_double) {
            in_single = !in_single;
            ++i;
            continue;
        }

        if (c == '"' && !in_single) {
            in_double = !in_double;
            ++i;
            continue;
        }

        if (c == '\\' && !in_single) {
            if (i + 1 >= command.length()) {
                return std::unexpected(ParseError::TRAILING_ESCAPE);
            }
            char next = command[i + 1];
            if (in_double) {
                if (next == '"' || next == '\\' || next == '$' || next == '`' || next == '\n') {
                    word += next;
                }
                else {
                    word += '\\';
                    word += next;
                }
            }
            else {
                word += next;
            }
            i += 2;
            continue;
        }

        word += c;
        ++i;
    }

    if (in_double) {
        return std::unexpected(ParseError::UNCLOSED_DOUBLE_QUOTE);
    }
    if (in_single) {
        return std::unexpected(ParseError::UNCLOSED_SINGLE_QUOTE);
    }

    return word;
}