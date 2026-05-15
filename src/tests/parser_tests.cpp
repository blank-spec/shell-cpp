#include <gtest/gtest.h>

#include "parser/parser.hpp"


bool operator==(const Redirection& lhs, const Redirection& rhs) {
    return lhs.filename == rhs.filename && lhs.redirect_type == rhs.redirect_type;
}


TEST(ShellParserTest, SimpleTokens) {
    auto res = ShellParser::Parse("ls -l /tmp");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->tokens, Tokens({"ls", "-l", "/tmp"}));
    EXPECT_TRUE(res->redirections.empty());
}


TEST(ShellParserTest, QuotesAndSpaces) {
    auto res = ShellParser::Parse("git commit -m \"optimized pathfinding for commands\"");
    ASSERT_TRUE(res.has_value());
    Tokens expected = {"git", "commit", "-m", "optimized pathfinding for commands"};
    EXPECT_EQ(res->tokens, expected);
}

TEST(ShellParserTest, SingleQuotes) {
    auto res = ShellParser::Parse("echo 'hello world'");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->tokens[1], "hello world");
}


TEST(ShellParserTest, EscapeSequences) {
    auto res = ShellParser::Parse("echo \\\"quoted\\\" \\ space");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->tokens[1], "\"quoted\"");
    EXPECT_EQ(res->tokens[2], " space");
}


TEST(ShellParserTest, Redirections) {
    {
        auto res = ShellParser::Parse("ls > out.txt");
        ASSERT_TRUE(res.has_value());
        ASSERT_EQ(res->redirections.size(), 1);
        EXPECT_EQ(res->redirections[0].filename, "out.txt");
        EXPECT_EQ(res->redirections[0].redirect_type, RedirectType::STDOUT_REWRITE);
    }

    {
        auto res = ShellParser::Parse("command 2>> error.log");
        ASSERT_TRUE(res.has_value());
        ASSERT_EQ(res->redirections.size(), 1);
        EXPECT_EQ(res->redirections[0].filename, "error.log");
        EXPECT_EQ(res->redirections[0].redirect_type, RedirectType::STDERR_APPEND);
    }
}


TEST(ShellParserTest, JoinedRedirection) {
    auto res = ShellParser::Parse("ls>output");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->tokens, Tokens({"ls"}));
    ASSERT_EQ(res->redirections.size(), 1);
    EXPECT_EQ(res->redirections[0].filename, "output");
}


TEST(ShellParserErrorTest, UnclosedQuotes) {
    auto res = ShellParser::Parse("echo \"unclosed quote");
    ASSERT_FALSE(res.has_value());
    EXPECT_EQ(res.error(), ParseError::UNCLOSED_DOUBLE_QUOTE);
}


TEST(ShellParserErrorTest, MissingFilename) {
    auto res = ShellParser::Parse("ls > ");
    ASSERT_FALSE(res.has_value());
    EXPECT_EQ(res.error(), ParseError::EXPECTED_FILENAME_AFTER_REDIRECT);
}


TEST(ShellParserErrorTest, TrailingEscape) {
    auto res = ShellParser::Parse("echo hello \\");
    ASSERT_FALSE(res.has_value());
    EXPECT_EQ(res.error(), ParseError::TRAILING_ESCAPE);
}


TEST(ShellParserTest, ComplexCommand) {
    auto res = ShellParser::Parse("grep \"pattern\" < input.txt 2> /dev/null >> all.log");
    ASSERT_TRUE(res.has_value());

    EXPECT_EQ(res->tokens, Tokens({"grep", "pattern"}));
    ASSERT_EQ(res->redirections.size(), 3);

    EXPECT_EQ(res->redirections[0].redirect_type, RedirectType::STDIN);
    EXPECT_EQ(res->redirections[0].filename, "input.txt");

    EXPECT_EQ(res->redirections[1].redirect_type, RedirectType::STDERR);
    EXPECT_EQ(res->redirections[1].filename, "/dev/null");

    EXPECT_EQ(res->redirections[2].redirect_type, RedirectType::STDOUT_APPEND);
    EXPECT_EQ(res->redirections[2].filename, "all.log");
}