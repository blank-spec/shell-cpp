#include <filesystem>
#include <memory>
#include <iostream>
#include <string>
#include <print>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

class ICommand {
public:
    virtual void execute(const std::vector<std::string>& args) = 0;
    virtual ~ICommand() = default;
};


class ExitCommand : public ICommand {
public:
    void execute(const std::vector<std::string> &args) override {
        exit(0);
    }
};


class TypeCommand : public ICommand {
public:
    explicit TypeCommand(const std::unordered_map<std::string, std::unique_ptr<ICommand>>& commands) :
        m_commands(commands) {}

    void execute(const std::vector<std::string> &args) override {
        const std::string& command_to_find = args[0];
        if (m_commands.contains(command_to_find)) {
            std::println("{} is a shell builtin", command_to_find);
            return;
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

                const fs::path full_path = fs::path(directory) / command_to_find;

                if (fs::exists(full_path)) {
                    std::println("{} is {}", command_to_find, full_path.string());
                    return;
                }
            }
        }

        std::println("{}: not found", command_to_find);
    }

private:
    const std::unordered_map<std::string, std::unique_ptr<ICommand>>& m_commands;
};


class EchoCommand : public ICommand {
public:
    void execute(const std::vector<std::string> &args) override {
        for (const auto& arg : args) {
            std::print("{} ", arg);
        }

        std::println();
    }
};


class Shell {
public:
    Shell() {
        m_commands["exit"] = std::make_unique<ExitCommand>();
        m_commands["echo"] = std::make_unique<EchoCommand>();
        m_commands["type"] = std::make_unique<TypeCommand>(m_commands);
    }

    void Run() {
        while (true) {
            std::print("$ ");

            std::string line;
            if (!std::getline(std::cin, line) || line.empty()) {
                continue;
            }

            auto tokens = ParseInput(line);
            const std::string command_token = tokens[0];

            std::vector<std::string> args{tokens.begin() + 1, tokens.end()};

            if (m_commands.contains(command_token)) {
                m_commands[command_token]->execute(args);
            }
            else {
                std::println("{}: command not found", command_token);
            }
        }
    }

private:
    std::vector<std::string> ParseInput(std::string_view line) const {
        std::vector<std::string> result;
        size_t start = 0;
        size_t pos = 0;

        while (pos < line.length()) {
            while (pos < line.length() && std::isspace(static_cast<unsigned char>(line[pos]))) {
                pos++;
            }

            start = pos;

            while (pos < line.length() && !std::isspace(static_cast<unsigned char>(line[pos]))) {
                pos++;
            }

            if (start < pos) {
                result.emplace_back(line.substr(start, pos - start));
            }
        }

        return result;
    }

private:
    std::unordered_map<std::string, std::unique_ptr<ICommand>> m_commands;
};

int main() {
    Shell shell;
    shell.Run();

    return 0;
}
