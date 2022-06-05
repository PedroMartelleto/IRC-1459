#include "./cmd.h"

class CommandManager
{
public:
    void RegisterCommand(const std::string& name, const CommandArgs& args, const CommandCallback& callback);
    void ExecuteCommand(const std::string& name, const CommandArgs& args);
private:
    std::map<std::string, Ref<CommandSpecs>> m_commands;
    CommandParser m_parser;
};