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
    std::string originalPrefix;

    auto redraw_line = [&]() {
        std::print("\r\033[K$ {}", line);
        std::cout.flush();
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
                if (mode == InputMode::Normal) {
                    originalPrefix = line;
                    auto gen = trie.GetNextCompletion(originalPrefix);

                    auto it = gen.begin();
                    if (it == gen.end()) {
                        std::cout << '\x07' << std::flush;
                        break;
                    }

                    std::string first = *it;
                    ++it;

                    if (it == gen.end()) {
                        if (line != first) {
                            line = std::move(first);
                            redraw_line();
                        }
                    }
                    else {
                        if (line != first) {
                            line = std::move(first);
                            redraw_line();
                        }
                        mode = InputMode::Completion;
                    }
                }
                else {
                    auto gen = trie.GetNextCompletion(originalPrefix);

                    std::print("\n");
                    for (auto&& word : gen) {
                        std::print("{} ", word);
                    }
                    std::print("\n");

                    redraw_line();
                    mode = InputMode::Normal;
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