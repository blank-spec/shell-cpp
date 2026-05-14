#include "echo_command.hpp"

#include <print>
#include <vector>
#include <string>
#include <ranges>
#include <fstream>


void EchoCommand::Execute(const std::vector<std::string> &args) {
    const RedirectCommand cmd = ParseEchoCommand(args);
    const auto result = cmd.args | std::views::join_with(' ') | std::ranges::to<std::string>();
    switch (cmd.mode) {
        case RedirectMode::None:
            std::println("{}", result);

        case RedirectMode::Overwrite:
            std::ofstream file(cmd.outputFile);
            file << result;
    }
}


RedirectCommand EchoCommand::ParseEchoCommand(const std::vector<std::string> &args) const {
     RedirectCommand cmd;
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == ">" || args[i] == "1>") {
            if (i + 1 < args.size()) {
                cmd.mode = RedirectMode::Overwrite;
                cmd.outputFile = args[++i];
            }
        }

        else {
            cmd.args.push_back(args[i]);
        }
    }

    return cmd;
}
