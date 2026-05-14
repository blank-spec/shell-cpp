#pragma once

#include <expected>
#include <string>
#include <vector>

struct Redirection;

using Token = std::string;
using Tokens = std::vector<Token>;
using Redirections = std::vector<Redirection>;


enum class RedirectType {
    STDOUT_REWRITE,
    STDOUT_APPEND,
    STDERR,
};


struct Redirection {
    std::string filename;
    RedirectType redirect_type;
};

enum class ParseError {
    UNCLOSED_DOUBLE_QUOTE,
    UNCLOSED_SINGLE_QUOTE,
    EXPECTED_FILENAME_AFTER_REDIRECT,
    TRAILING_ESCAPE
};


std::string ErrorToString(ParseError err);


struct ParseResult {
    Tokens tokens;
    Redirections redirections;
};


class ShellParser {
public:
    static std::expected<ParseResult, ParseError> Parse(std::string_view command);

private:
    static std::expected<std::string, ParseError> ExtractNextWord(std::string_view command, size_t& i);

private:
};
