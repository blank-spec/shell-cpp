#include "exit_command.hpp"


void ExitCommand::Execute(const std::vector<std::string> &args) {
    exit(0);
}


bool ExitCommand::IsStateChanging() const {
    return true;
}
