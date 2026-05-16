#pragma once

#include "commands/base_command.hpp"


class EchoCommand : public ICommand {
public:
    static constexpr std::string_view Name = "echo";

    void Execute(const std::vector<std::string> &args) override;
    bool IsStateChanging() const override;
};
