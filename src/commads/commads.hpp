#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


class ICommand {
public:
    virtual void Execute(const std::vector<std::string>& args) = 0;
    virtual ~ICommand() = default;
};


class ExitCommand : public ICommand {
public:
    void Execute(const std::vector<std::string> &args) override;
};


class EchoCommand : public ICommand {
public:
    void Execute(const std::vector<std::string> &args) override;
};


class TypeCommand : public ICommand {
public:
    explicit TypeCommand(const std::unordered_map<std::string, std::unique_ptr<ICommand>>& commands);

    void Execute(const std::vector<std::string> &args) override;

private:
    const std::unordered_map<std::string, std::unique_ptr<ICommand>>& m_commands;
    std::unique_ptr<ICommand> m_run_command;
};


class RunCommand : public ICommand {
public:
    void Execute(const std::vector<std::string> &args) override;
#ifdef _WIN32
    void RunWindows(const std::string& path, const std::vector<std::string>& args) const;
#else
    void RunPosix(const std::string& path, const std::vector<std::string>& args) const;
#endif
};


class PwdCommand : public ICommand {
public:
    void Execute(const std::vector<std::string> &args) override;
};
