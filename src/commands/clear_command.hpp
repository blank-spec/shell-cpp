#pragma once

#include "base_command.hpp"


class ClearCommand : public ICommand {
public:
    static constexpr std::string_view Name = "clear";

    void Execute(const std::vector<std::string> &args) override;
    bool IsStateChanging() const override;
};