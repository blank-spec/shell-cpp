#pragma once

#include "commands/base_command.hpp"


class EchoCommand : public ICommand {
public:
    void Execute(const std::vector<std::string> &args) override;
    bool IsStateChanging() const override;
};
