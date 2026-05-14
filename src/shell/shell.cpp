#include "shell.hpp"
#include "commands/commands.hpp"

#include <iostream>
#include <print>

#include "executor/executor.hpp"
#include "parser/parser.hpp"


Shell::Shell() {
    m_commands["exit"] = std::make_unique<ExitCommand>();
    m_commands["echo"] = std::make_unique<EchoCommand>();
    m_commands["type"] = std::make_unique<TypeCommand>(m_commands);
    m_commands["pwd"] = std::make_unique<PwdCommand>();
    m_commands["cd"] = std::make_unique<CdCommand>();
    m_commands["clear"] = std::make_unique<ClearCommand>();

    m_external_handler = std::make_unique<RunCommand>();
}


void Shell::Run() {
    while (true) {
        std::print("$ ");

        std::string line;
        if (!std::getline(std::cin, line) || line.empty()) {
            continue;
        }

        auto parse_result = ShellParser::Parse(line);
        if (!parse_result.has_value()) {
            std::println("{}", ErrorToString(parse_result.error()));
            continue;
        }

        auto [tokens, redirections] = parse_result.value();
        const std::string& command_token = tokens[0];

        std::vector<std::string> args{tokens.begin() + 1, tokens.end()};

        if (m_commands.contains(command_token)) {
            auto cmd = m_commands[command_token].get();
            if (cmd->IsStateChanging()) {
                cmd->Execute(args);
            }
            else {
                CommandExecutor::Execute(redirections, [cmd](const Tokens& args) {
                   cmd->Execute(args);
                }, args);
            }
        }
        else {
            CommandExecutor::Execute(redirections, [this](const Tokens& inner_args) {
                m_external_handler->Execute(inner_args);
            }, tokens);
        }
    }
}
