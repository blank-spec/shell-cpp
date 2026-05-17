#pragma once

#include "commands/base_command.hpp"

#include <functional>
#include <memory>
#include <string_view>
#include <unordered_map>


class CommandRepository {
public:
    using Creator = std::function<std::unique_ptr<ICommand>()>;

    static CommandRepository& instance();

    void RegisterCommand(std::string_view name, Creator creator);

    bool IsBuiltin(std::string_view name) const;

    std::optional<std::unique_ptr<ICommand>>
    Create(std::string_view name) const;

    std::vector<std::string> GetCommands() const;

private:
    struct StringHash {
        using is_transparent = void;

        size_t operator()(std::string_view sv) const noexcept;
        size_t operator()(const std::string& s) const noexcept;
    };

private:
    std::unordered_map<std::string, Creator, StringHash, std::equal_to<>> m_commands;
};


template <typename... Commands>
void RegisterAll() {
    auto& repo = CommandRepository::instance();
    (repo.RegisterCommand(Commands::Name, [] { return std::make_unique<Commands>(); }), ...);
}
