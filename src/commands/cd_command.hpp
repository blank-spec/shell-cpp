#pragma once

#include "base_command.hpp"

class CdCommand : public ICommand {
public:
    void Execute(const std::vector<std::string> &args) override;
};
