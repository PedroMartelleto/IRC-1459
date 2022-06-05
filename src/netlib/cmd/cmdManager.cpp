#include "cmdManager.h"
#include "cmdParser.h"

Ref<CommandSpecs> CommandManager::GetCmdSpecsByName(const std::string& name) const
{
    auto it = m_commands.find(name);
    if (it == m_commands.end())
    {
        return nullptr;
    }
    return it->second;
}

void CommandManager::RegisterCommand(const std::string& name, const CommandArgs& args, int minArgs, const std::string& description,  const CommandCallback& callback)
{
    auto specs = CreateRef<CommandSpecs>();
    specs->name = name;
    specs->argNames = args;
    specs->callback = callback;
    specs->description = description;
    specs->minArgs = minArgs >= 0 ? minArgs : args.size();
    m_commands[name] = specs;
}

void CommandManager::RunCommand(const std::string& name, const CommandArgs& args) const
{
    Command{ GetCmdSpecsByName(name), args }.Run();
}

void CommandManager::Poll()
{
    while (true)
    {
        auto cmd = GetNextCommand();
        auto result = cmd.Run();

        if (result == CommandResult::EXIT)
        {
            break;
        }
    }
}

Command CommandManager::GetNextCommand()
{
    std::string line;
    std::getline(std::cin, line);
    
    auto parsedCommand = CommandParser::FromInputText(line, *this);

    if (parsedCommand.specs == Command::Invalid.specs)
    {
        return GetNextCommand();
    }
    else
    {
        return parsedCommand;
    }
}

std::map<std::string, Ref<CommandSpecs>> CommandManager::GetAllSpecs() const
{
    return m_commands;
}