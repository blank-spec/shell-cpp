#include "shell.hpp"

#include <iostream>
#include <print>


Shell::Shell() {
    m_commands["exit"] = std::make_unique<ExitCommand>();
    m_commands["echo"] = std::make_unique<EchoCommand>();
    m_commands["type"] = std::make_unique<TypeCommand>(m_commands);

    m_external_handler = std::make_unique<RunCommand>();
}


std::vector<std::string> Shell::ParseInput(std::string_view line) const {
    std::vector<std::string> result;
    size_t start = 0;
    size_t pos = 0;

    while (pos < line.length()) {
        while (pos < line.length() && std::isspace(static_cast<unsigned char>(line[pos]))) {
            pos++;
        }

        start = pos;

        while (pos < line.length() && !std::isspace(static_cast<unsigned char>(line[pos]))) {
            pos++;
        }

        if (start < pos) {
            result.emplace_back(line.substr(start, pos - start));
        }
    }

    return result;
}


void Shell::Run() {
    while (true) {
        std::print("$ ");

        std::string line;
        if (!std::getline(std::cin, line) || line.empty()) {
            continue;
        }

        auto tokens = ParseInput(line);
        const std::string& command_token = tokens[0];

        std::vector<std::string> args{tokens.begin() + 1, tokens.end()};

        if (m_commands.contains(command_token)) {
            m_commands[command_token]->Execute(args);
        }
        else {
            m_external_handler->Execute(tokens);
        }
    }
}
