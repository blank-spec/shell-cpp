#include "shell.hpp"
#include "commands/commands.hpp"

#include <iostream>
#include <print>


Shell::Shell() {
    m_commands["exit"] = std::make_unique<ExitCommand>();
    m_commands["echo"] = std::make_unique<EchoCommand>();
    m_commands["type"] = std::make_unique<TypeCommand>(m_commands);
    m_commands["pwd"] = std::make_unique<PwdCommand>();
    m_commands["cd"] = std::make_unique<CdCommand>();
    m_commands["clear"] = std::make_unique<ClearCommand>();

    m_external_handler = std::make_unique<RunCommand>();
}


std::vector<std::string> Shell::ParseInput(std::string_view line) const {
    std::vector<std::string> result;
    std::string current_arg;
    bool in_quotes = false;

    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];

        if (c == '\'') {
            in_quotes = !in_quotes;
            continue;
        }

        if (std::isspace(static_cast<unsigned char>(c)) && !in_quotes) {
            if (!current_arg.empty()) {
                result.push_back(current_arg);
                current_arg.clear();
            }
        }
        else {
            current_arg += c;
        }
    }

    if (!current_arg.empty()) {
        result.push_back(current_arg);
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
