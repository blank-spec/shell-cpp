#pragma once

#include <filesystem>
#include <string>
#include <vector>


class ICommand {
public:
    virtual void Execute(const std::vector<std::string>& args) = 0;
    virtual bool IsStateChanging() const = 0;

    virtual ~ICommand() = default;
};
