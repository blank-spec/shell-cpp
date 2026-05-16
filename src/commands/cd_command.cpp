#include "cd_command.hpp"
#include "command_repository/command_repository.hpp"

#include <print>


void CdCommand::Execute(const std::vector<std::string> &args) {
    if (args.size() < 2) {
        return;
    }

    std::string path = args[1];
    if (path == "~") {
        const char* home = nullptr;
#ifdef _WIN32
        home = std::getenv("USERPROFILE");
#else
        home = std::getenv("HOME");
#endif

        if (home) {
            path = home;
        }
        else {
            return;
        }
    }

    std::error_code ec;
    std::filesystem::current_path(path, ec);

    if (ec) {
        std::println(stderr, "cd: {}: No such file or directory", path);
    }
}


bool CdCommand::IsStateChanging() const {
    return true;
}
