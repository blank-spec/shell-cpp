#include "echo_command.hpp"

#include <print>

#include <vector>
#include <string>
#include <ranges>


void EchoCommand::Execute(const std::vector<std::string> &args) {
    for (size_t i = 0; i < args.size(); ++i) {
        std::print("{}", args[i]);

        if (i < args.size() - 1) {
            std::print(" ");
        }
    }
    std::println();
}