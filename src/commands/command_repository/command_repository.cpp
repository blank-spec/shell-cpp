#include "command_repository.hpp"

#include <print>
#include <ranges>


CommandRepository &CommandRepository::instance() {
    static CommandRepository instance;
    return instance;
}


void CommandRepository::RegisterCommand(std::string_view name, Creator creator) {
    m_commands.emplace(name, std::move(creator));
}


std::optional<std::unique_ptr<ICommand>>
CommandRepository::Create(std::string_view name) const {
    if (auto it = m_commands.find(name); it != m_commands.end()) {
        return it->second();
    }

    return std::nullopt;
}


std::vector<std::string> CommandRepository::GetCommands() const {
    return m_commands | std::ranges::views::keys | std::ranges::to<std::vector<std::string>>();
}


bool CommandRepository::IsBuiltin(std::string_view name) const {
    return m_commands.contains(name);
}


size_t CommandRepository::StringHash::operator()(std::string_view sv) const noexcept {
    return std::hash<std::string_view>{}(sv);
}


size_t CommandRepository::StringHash::operator()(const std::string &s) const noexcept {
    return std::hash<std::string>{}(s);
}
