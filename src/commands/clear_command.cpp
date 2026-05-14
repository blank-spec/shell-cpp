#include "clear_command.hpp"

#include <iostream>
#include <print>


void ClearCommand::Execute(const std::vector<std::string>&) {
    std::print("\033[H\033[2J\033[3J");
    std::cout.flush();
}


bool ClearCommand::IsStateChanging() const {
    return true;
}
