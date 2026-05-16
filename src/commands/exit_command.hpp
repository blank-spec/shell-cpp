#pragma once

#include "base_command.hpp"


class ExitCommand : public ICommand {
public:
    static constexpr std::string_view Name = "exit";

    void Execute(const std::vector<std::string> &args) override;
    bool IsStateChanging() const override;
};
