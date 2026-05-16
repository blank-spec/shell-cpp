#include "shell.hpp"
#include "commands/command_repository/command_repository.hpp"
#include "executor/executor.hpp"
#include "parser/parser.hpp"
#include "commands/commands.hpp"

#include <iostream>
#include <print>


Shell::Shell() {
    RegisterAll<TypeCommand, PwdCommand, ExitCommand, EchoCommand, EchoCommand, ClearCommand, CdCommand>();
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
            std::println(stderr, "{}", ErrorToString(parse_result.error()));
            continue;
        }

        auto [tokens, redirections] = parse_result.value();

        CommandExecutor::Process({tokens, redirections});
    }
}
