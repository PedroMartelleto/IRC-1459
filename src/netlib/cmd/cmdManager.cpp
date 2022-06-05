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

void CommandManager::RegisterCommand(const std::string& name, const CommandArgs& args, const CommandCallback& callback, int minArgs)
{
    auto specs = CreateRef<CommandSpecs>();
    specs->name = name;
    specs->argNames = args;
    specs->callback = callback;
    specs->minArgs = minArgs >= 0 ? minArgs : args.size();
    m_commands[name] = specs;
}

void CommandManager::RunCommand(const std::string& name, const CommandArgs& args)
{
    Command{ GetCmdSpecsByName(name), args }.Run();
}

void CommandManager::Poll()
{
    while (true)
    {
        auto cmd = GetNextCommand();
        auto result = cmd.Run();

        if (result == CommandResult::TERMINATE)
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