#pragma once

#include <functional>

#include "commands/base_command.hpp"
#include "parser/parser.hpp"


class Redirector {
public:
    static bool Apply(const Redirections& redirects);

private:
    static bool SetupStdout(const std::string& path, bool append);
    static bool SetupStderr(const std::string& path, bool append);
};


struct Args {
    Tokens       tokens;
    Redirections redirections;
};


class CommandExecutor {
public:
    static void Process(const Args& args);

private:
    static void Execute(const Args& args, const std::function<void(const Tokens&)>& command);
    static void HandleBuiltin(std::unique_ptr<ICommand> cmd, const Args& args);
    static void HandleExternal(const Args& args);
};

