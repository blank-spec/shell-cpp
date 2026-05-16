#include "type_command.hpp"
#include "utils/file_utills.hpp"
#include "run_command.hpp"
#include "command_repository/command_repository.hpp"

#include <print>


void TypeCommand::Execute(const std::vector<std::string> &args) {
    namespace fs = std::filesystem;

    const std::string& command_to_find = args[1];
    if (CommandRepository::instance().IsBuiltin(command_to_find)) {
        std::println("{} is a shell builtin", command_to_find);
        return;
    }

    if (const auto path = utils::GetCommandPath(command_to_find); path.has_value()) {
        std::println("{} is {}", command_to_find, path.value());
    }
    else {
        std::println(stderr, "{}: not found", command_to_find);
    }
}


bool TypeCommand::IsStateChanging() const {
    return false;
}
