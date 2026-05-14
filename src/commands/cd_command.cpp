#include "cd_command.hpp"

#include <print>

void CdCommand::Execute(const std::vector<std::string> &args) {
    if (args.size() < 1) {
        return;
    }

    std::string path = args[0];
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
