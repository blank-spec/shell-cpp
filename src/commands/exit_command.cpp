#include "exit_command.hpp"
#include "command_repository/command_repository.hpp"


void ExitCommand::Execute(const std::vector<std::string> &args) {
    exit(0);
}


bool ExitCommand::IsStateChanging() const {
    return true;
}
