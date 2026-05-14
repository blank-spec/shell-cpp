#include "cat_command.hpp"

#include <fstream>
#include <print>
#include <iostream>


void CatCommand::Execute(const std::vector<std::string> &args) {
    for (const auto& path : args) {
        ReadFile(path);
    }
}


void CatCommand::ReadFile(const std::string& path) const {
    if (std::filesystem::is_directory(path)) {
        std::println("cat: {}: Is a directory", path);
    }

    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::println("cat: {}: No such file or directory", path);
    }

    std::cout << file.rdbuf();
}
