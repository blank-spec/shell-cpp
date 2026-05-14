#include <print>

#include "file_utills.hpp"
#include <vector>

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
            fs::path cmd_path(command);

            if (cmd_path.is_absolute()) {
                if (fs::exists(cmd_path)) return cmd_path.string();
                return std::nullopt;
            }

            const char* path_env = std::getenv("PATH");
            if (path_env) {
                std::stringstream ss{path_env};
                std::string directory;

#ifdef _WIN32
                char delimiter = ';';
                std::vector<std::string> extensions = {"", ".exe", ".bat", ".cmd", ".com"};
#else
                char delimiter = ':';
                std::vector<std::string> extensions = {""};
#endif

                while (std::getline(ss, directory, delimiter)) {
                    if (directory.empty()) continue;

                    for (const auto& ext : extensions) {
                        const fs::path full_path = fs::path(directory) / (std::string(command) + ext);

                        if (fs::exists(full_path) && fs::is_regular_file(full_path)) {
                            if (IsExecutable(full_path)) {
                                return full_path.string();
                            }
                        }
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
