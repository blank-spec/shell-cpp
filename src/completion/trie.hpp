#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <experimental/generator>


class CommandTrie {
private:
    struct TrieNode {
        std::unordered_map<char, std::unique_ptr<TrieNode>> children;
        bool is_end_of_command = false;
    };

public:
    using Generator = std::experimental::generator<std::string>;

    CommandTrie();

    Generator GetNextCompletion(const std::string& prefix);
    void Insert(std::string_view command) const ;
    void InsertAll(const std::vector<std::string>& commands) const;

private:
    Generator CollectAll(TrieNode* node, std::string curr_prefix);

private:
    std::unique_ptr<TrieNode> m_root;
};
