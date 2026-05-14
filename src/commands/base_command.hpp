#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


class ICommand {
public:
    virtual void Execute(const std::vector<std::string>& args) = 0;
    virtual ~ICommand() = default;
};
