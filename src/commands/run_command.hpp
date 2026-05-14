#pragma once

#include "commands/base_command.hpp"

class RunCommand : public ICommand {
public:
    void Execute(const std::vector<std::string> &args) override;
    bool IsStateChanging() const override;

#ifdef _WIN32
    void RunWindows(const std::string& path, const std::vector<std::string>& args) const;
#else
    void RunPosix(const std::string& path, const std::vector<std::string>& args) const;
#endif
};
