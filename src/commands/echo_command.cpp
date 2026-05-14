#include "echo_command.hpp"

#include <print>

#include <vector>
#include <string>
#include <ranges>


void EchoCommand::Execute(const std::vector<std::string>& args)
{
    if (args.empty()) {
        std::println();
        return;
    }

    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];

        for (char c : arg) {
            if (c != '\'') {
                std::print("{}", c);
            }
        }

        if (i < args.size() - 1) {
            std::print(" ");
        }
    }

    std::println();
}