#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "commands/base_command.hpp"


class Shell {
public:
    Shell();
    void Run();

private:
    std::unordered_map<std::string, std::unique_ptr<ICommand>> m_commands;
    std::unique_ptr<ICommand> m_external_handler;
};
