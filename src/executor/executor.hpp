#pragma once
#include <functional>

#include "parser/parser.hpp"

class Redirector {
public:
    static bool Apply(const Redirections& redirects);

private:
    static bool SetupStdout(const std::string& path, bool append);
};


class CommandExecutor {
public:
    static void Execute(const std::vector<Redirection>& redirects,
                        const std::function<void(const Tokens&)>& command,
                        const Tokens& args);
};
