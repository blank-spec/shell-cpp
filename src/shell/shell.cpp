#include "shell.hpp"
#include "commands/command_repository/command_repository.hpp"
#include "executor/executor.hpp"
#include "parser/parser.hpp"
#include "commands/commands.hpp"
#include "completion/trie.hpp"

#include <iostream>
#include <print>


namespace {
    #if defined(_WIN32) || defined(_WIN64)
        #include <conio.h>
        #define GETCH() _getch()

    #else
        #include <termios.h>
        #include <unistd.h>
        #include <cstdio>

            inline int GETCH() {
                struct termios oldt, newt;
                int ch;
                tcgetattr(STDIN_FILENO, &oldt);
                newt = oldt;
                newt.c_lflag &= ~(ICANON | ECHO);
                tcsetattr(STDIN_FILENO, TCSANOW, &newt);
                ch = getchar();
                tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
                return ch;
            }
    #endif
}


Shell::Shell() {
    RegisterAll<TypeCommand, PwdCommand, ExitCommand, EchoCommand, EchoCommand, ClearCommand, CdCommand>();
}


enum class InputMode {
    Normal,
    Completion
};


void Shell::Run() {
    CommandTrie trie;
    trie.InsertAll(
        CommandRepository::instance().GetCommands()
    );

    std::string line;

    auto redraw_line = [&]() {
        std::print("\r\033[K$ {}", line);
        std::cout.flush();
    };

    auto print_completions_list =
        [&](const std::vector<std::string>& completions) {
            std::print("\n");
            for (const auto& word : completions) {
                std::print("{} ", word);
            }
            std::print("\n");
        };

    auto reset_mode = [&](InputMode& mode) {
        mode = InputMode::Normal;
    };

    InputMode mode = InputMode::Normal;

    std::print("$ ");
    std::cout.flush();

    while (true) {
        int ch = GETCH();

        switch (ch) {
            case '\t': {
                auto generator = trie.GetNextCompletion(line);
                std::vector<std::string> completions;
                for (auto&& word : generator) {
                    completions.push_back(std::move(word));
                }

            switch (mode) {
                case InputMode::Normal: {
                    if (completions.empty()) {
                        break;
                    }

                    if (completions.size() == 1) {
                        if (line != completions.front()) {
                            line = completions.front();
                            redraw_line();
                        }
                        break;
                    }

                    if (line != completions.front()) {
                        line = completions.front();
                        redraw_line();
                    }
                    mode = InputMode::Completion;
                    break;
                }

                case InputMode::Completion: {
                    print_completions_list(completions);
                    line.clear();
                    std::print("$ ");
                    std::cout.flush();
                    mode = InputMode::Normal;
                    break;
                }
            }
            break;
        }

            case '\r':
            case '\n': {
                reset_mode(mode);
                std::print("\n");

                auto parse_result = ShellParser::Parse(line);
                if (!parse_result.has_value()) {
                    std::println(stderr, "{}", ErrorToString(parse_result.error()));
                    line.clear();
                    std::print("$ ");
                    std::cout.flush();
                    break;
                }

                auto [tokens, redirections] = parse_result.value();
                CommandExecutor::Process({ tokens, redirections });

                line.clear();
                std::print("$ ");
                std::cout.flush();
                break;
            }

            case 8:
            case 127: {
                reset_mode(mode);
                if (!line.empty()) {
                    line.pop_back();
                    redraw_line();
                }
                break;
            }

            default: {
                reset_mode(mode);
                line.push_back(static_cast<char>(ch));
                std::cout << static_cast<char>(ch) << std::flush;
                break;
            }
        }
    }
}