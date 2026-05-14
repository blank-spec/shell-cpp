#pragma once

#include "base_command.hpp"

class PwdCommand : public ICommand {
public:
    void Execute(const std::vector<std::string> &args) override;
    bool IsStateChanging() const override;
};
