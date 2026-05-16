#include "pwd_command.hpp"
#include "command_repository/command_repository.hpp"

#include <print>


void PwdCommand::Execute(const std::vector<std::string> &args) {
    std::println("{}", std::filesystem::current_path().string());
}


bool PwdCommand::IsStateChanging() const {
    return false;
}
