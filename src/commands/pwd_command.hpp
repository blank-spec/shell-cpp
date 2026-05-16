#pragma once

#include "base_command.hpp"


class PwdCommand : public ICommand {
public:
    static constexpr std::string_view Name = "pwd";

    void Execute(const std::vector<std::string> &args) override;
    bool IsStateChanging() const override;
};
