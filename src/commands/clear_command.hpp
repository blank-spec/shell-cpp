#pragma once

#include "base_command.hpp"

class ClearCommand : public ICommand {
public:
    void Execute(const std::vector<std::string> &args) override;
};