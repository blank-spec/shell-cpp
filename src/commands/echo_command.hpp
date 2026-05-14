#pragma once

#include "commands/base_command.hpp"


namespace {
    enum class RedirectMode {
        None,
        Overwrite,
    };

    struct RedirectCommand {
        std::vector<std::string> args;
        std::string outputFile;
        RedirectMode mode = RedirectMode::None;
    };
}


class EchoCommand : public ICommand {
public:
    void Execute(const std::vector<std::string> &args) override;

private:
    RedirectCommand ParseEchoCommand(const std::vector<std::string> &args) const;
};
