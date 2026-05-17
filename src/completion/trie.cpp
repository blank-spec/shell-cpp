#include "trie.hpp"


CommandTrie::CommandTrie()
    : m_root(std::make_unique<TrieNode>())
{}


void CommandTrie::Insert(std::string_view command) const {
    TrieNode* curr = m_root.get();
    for (char ch : command) {
        if (!curr->children.contains(ch)) {
            curr->children[ch] = std::make_unique<TrieNode>();
        }

        curr = curr->children[ch].get();
    }

    curr->is_end_of_command = true;
}


void CommandTrie::InsertAll(const std::vector<std::string>& commands) const {
    for (const auto& command : commands) {
        Insert(command);
    }
}


CommandTrie::Generator CommandTrie::GetNextCompletion(const std::string& prefix) {
    TrieNode* curr = m_root.get();

    for (char ch : prefix) {
        if (!curr->children.contains(ch)) {
            co_return;
        }

        curr = curr->children[ch].get();
    }

    for (const auto& res : CollectAll(curr, prefix)) {
        co_yield res;
    }
}



CommandTrie::Generator CommandTrie::CollectAll(TrieNode* node, std::string curr_prefix) {
    if (node->is_end_of_command) {
        co_yield curr_prefix;
    }

    for (const auto& [ch, child_node] : node->children) {
        for (const auto& sub : CollectAll(child_node.get(), curr_prefix + ch)) {
            co_yield sub;
        }
    }
}

