#include "base_command.hpp"

class TypeCommand : public ICommand {
public:
    explicit TypeCommand(const std::unordered_map<std::string, std::unique_ptr<ICommand>>& commands);

    void Execute(const std::vector<std::string> &args) override;
    bool IsStateChanging() const override;

private:
    const std::unordered_map<std::string, std::unique_ptr<ICommand>>& m_commands;
    std::unique_ptr<ICommand> m_run_command;
};
