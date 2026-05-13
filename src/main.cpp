#include <iostream>
#include <string>
#include <print>

int main() {
    std::print("$ ");

    std::string input;
    if (std::getline(std::cin, input)) {
        std::println("{}: command not found", input);
    }

    return 0;
}
