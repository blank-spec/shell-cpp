#include "file_utills.hpp"

#include <unordered_map>
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

        if (command.empty()) {
            return std::nullopt;
        }

        static std::unordered_map<std::string, std::string> cache;
        static std::vector<fs::path> path_dirs;
        static bool path_initialized = false;

        if (command.front() == '/' ||
            (command.size() >= 2 && command[1] == ':')) {

            std::error_code ec;
            const fs::path p(command);

            if (p.is_absolute() &&
                fs::exists(p, ec) && !ec &&
                fs::is_regular_file(p, ec) && !ec &&
                IsExecutable(p)) {
                return p.string();
            }
            return std::nullopt;
        }

        const std::string cmd_str(command);

        if (cache.contains(cmd_str)) {
            return cache[cmd_str];
        }

        if (!path_initialized) {
            const char* path_env = std::getenv("PATH");
            if (path_env && *path_env) {
                std::stringstream ss(path_env);
                std::string dir;
    #ifdef _WIN32
                char delim = ';';
    #else
                char delim = ':';
    #endif
                while (std::getline(ss, dir, delim)) {
                    if (!dir.empty()) {
                        path_dirs.emplace_back(std::move(dir));
                    }
                }
            }
            path_initialized = true;
        }

        auto check_and_cache = [&](const fs::path& path) -> bool {
            std::error_code ec;
            if (fs::exists(path, ec) && !ec &&
                fs::is_regular_file(path, ec) && !ec &&
                IsExecutable(path)) {

                std::string result = path.string();
                cache[cmd_str] = result;
                return true;
            }
            return false;
        };

        for (const auto& dir : path_dirs) {
            fs::path base = dir / cmd_str;

            if (check_and_cache(base)) {
                return cache[cmd_str];
            }

    #ifdef _WIN32
            for (std::string_view ext : {".exe", ".bat", ".cmd", ".com"}) {
                fs::path p = base;
                p += ext;

                if (check_and_cache(p)) {
                    return cache[cmd_str];
                }
            }
    #endif
        }

        return std::nullopt;
    }
}
