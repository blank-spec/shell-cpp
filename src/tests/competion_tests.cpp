#include "completion/trie.hpp"

#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <algorithm>

namespace {
    std::vector<std::string> ToVector(CommandTrie::Generator gen) {
        std::vector<std::string> result;
        for (auto&& value : gen) {
            result.push_back(value);
        }

        std::ranges::sort(result);
        return result;
    }
}


class CommandTrieTest : public ::testing::Test {
protected:
    CommandTrie trie;
};


TEST_F(CommandTrieTest, InsertAndCompleteExact) {
    trie.Insert("help");
    trie.Insert("history");

    const auto results = ToVector(trie.GetNextCompletion("he"));

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0], "help");
}


TEST_F(CommandTrieTest, PrefixMatchingMultiple) {
    trie.Insert("command_a");
    trie.Insert("command_b");
    trie.Insert("common");
    trie.Insert("other");

    const auto results = ToVector(trie.GetNextCompletion("comm"));

    ASSERT_EQ(results.size(), 3);
    EXPECT_EQ(results[0], "command_a");
    EXPECT_EQ(results[1], "command_b");
    EXPECT_EQ(results[2], "common");
}


TEST_F(CommandTrieTest, NoMatches) {
    trie.Insert("apple");
    trie.Insert("apply");

    const auto results = ToVector(trie.GetNextCompletion("b"));

    EXPECT_TRUE(results.empty());
}


TEST_F(CommandTrieTest, NestedCommands) {
    trie.Insert("git");
    trie.Insert("git-commit");
    trie.Insert("git-push");

    const auto results = ToVector(trie.GetNextCompletion("git"));

    ASSERT_EQ(results.size(), 3);
    EXPECT_EQ(results[0], "git");
    EXPECT_EQ(results[1], "git-commit");
    EXPECT_EQ(results[2], "git-push");
}


TEST_F(CommandTrieTest, EmptyPrefixReturnsAll) {
    trie.Insert("a");
    trie.Insert("b");

    const auto results = ToVector(trie.GetNextCompletion(""));

    EXPECT_EQ(results.size(), 2);
}


TEST_F(CommandTrieTest, CaseSensitivity) {
    trie.Insert("Help");

    const auto results_lower = ToVector(trie.GetNextCompletion("help"));
    const auto results_upper = ToVector(trie.GetNextCompletion("Help"));

    EXPECT_TRUE(results_lower.empty());
    ASSERT_EQ(results_upper.size(), 1);
}