#include "type_command.hpp"
#include "utils/file_utills.hpp"

#include <print>

#include "run_command.hpp"


TypeCommand::TypeCommand(const std::unordered_map<std::string, std::unique_ptr<ICommand> > &commands)
: m_commands(commands),
  m_run_command(std::make_unique<RunCommand>())
{}


void TypeCommand::Execute(const std::vector<std::string> &args) {
    namespace fs = std::filesystem;

    const std::string& command_to_find = args[0];
    if (m_commands.contains(command_to_find)) {
        std::println("{} is a shell builtin", command_to_find);
        return;
    }

    if (const auto path = utils::GetCommandPath(command_to_find); path.has_value()) {
        std::println("{} is {}", command_to_find, path.value());
    }
    else {
        std::println("{}: not found", command_to_find);
    }
}
