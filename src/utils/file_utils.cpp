#include <print>

#include "file_utills.hpp"

namespace utils {
    bool IsExecutable(const std::filesystem::path& path) {
        namespace fs = std::filesystem;
        try {
            const auto status = fs::status(path);
            const auto perms = status.permissions();

            return (perms & fs::perms::owner_exec) != fs::perms::none ||
                   (perms & fs::perms::group_exec) != fs::perms::none ||
                   (perms & fs::perms::others_exec) != fs::perms::none;
        }
        catch (...) {
            return false;
        }
    }


    std::optional<std::string> GetCommandPath(std::string_view command) {
        namespace fs = std::filesystem;

        try {
            if (fs::path(command).is_absolute()) {
                return std::string{command};
            }

            const char* path_env = std::getenv("PATH");
            if (path_env) {
                const std::string path_str{path_env};
                std::stringstream ss{path_str};
                std::string directory;
                char delimiter = ':';
#ifdef _WIN32
                delimiter = ';';
#endif
                while (std::getline(ss, directory, delimiter)) {
                    if (directory.empty()) {
                        continue;
                    }

                    const fs::path full_path = fs::path(directory) / command;

                    if (fs::exists(full_path) && fs::is_regular_file(full_path) && IsExecutable(full_path)) {
                        return full_path.string();
                    }
                }
            }
        }
        catch (...) {
            return std::nullopt;
        }

        return std::nullopt;
    }
}
