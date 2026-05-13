#include <iostream>
#include <string>
#include <print>

int main() {
    while (true) {
        std::print("$ ");

        std::string input;
        if (std::getline(std::cin, input)) {
            std::println("{}: command not found", input);
        }
    }

    return 0;
}
