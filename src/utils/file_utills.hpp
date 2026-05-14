#pragma once

#include <filesystem>


namespace utils {
    bool IsExecutable(const std::filesystem::path& path);

    std::optional<std::string> GetCommandPath(std::string_view command);
}

