#include "echo_command.hpp"
#include "command_repository/command_repository.hpp"

#include <ostream>
#include <print>
#include <vector>
#include <string>


void EchoCommand::Execute(const std::vector<std::string> &args) {
    for (size_t i = 1; i < args.size(); ++i) {
        std::print("{}", args[i]);

        if (i < args.size() - 1) {
            std::print(" ");
        }
    }
    std::println();
}


bool EchoCommand::IsStateChanging() const {
    return false;
}
