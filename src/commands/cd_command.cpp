#include "cd_command.hpp"

#include <print>
#include <iostream>


void CdCommand::Execute(const std::vector<std::string> &args) {
    if (args.size() < 1) {
        return;
    }

    std::string path = args[0];
    std::cout << "DEBUG: changing dir to " << path << std::endl;
    if (path == "~") {
        const char* home;
#ifdef _WIN32
        home = getenv("USERPROFILE");
#else
        home = getenv("HOME");
#endif
        if (home) {
            path = home;
        }
    }

    std::error_code ec;
    std::filesystem::current_path(path, ec);

    if (ec) {
        std::println("cd: {}: No such file or directory", path);
    }
}
