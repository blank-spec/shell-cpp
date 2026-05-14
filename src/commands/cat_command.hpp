#pragma once

#include "base_command.hpp"


class CatCommand : public ICommand {
public:
    void Execute(const std::vector<std::string> &args) override;

private:
    void ReadFile(const std::string& path) const;
};