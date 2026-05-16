#pragma once

#include "base_command.hpp"


class CdCommand : public ICommand {
public:
    static constexpr std::string_view Name = "cd";

    void Execute(const std::vector<std::string> &args) override;
    bool IsStateChanging() const override;
};
