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

void CommandManager::RegisterDefaultCommand(const CommandCallback& callback) {
    m_defaultCommand = callback;
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
    try
    {
        PollEventsLoop();
    }
    catch (...)
    {
        Logger::Print("Exception in command manager. Continuing...\n");
        PollEventsLoop();
    }

    // If we are here, the user has typed Ctrl + D or /quit.
}

void CommandManager::PollEventsLoop()
{
    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line[0] == '/') {
            auto cmd = GetNextCommand(line);
            auto result = cmd.Run();

            if (result == CommandResult::EXIT)
            {
                break;
            }
        }
        else if (m_defaultCommand != nullptr)
        {
            m_defaultCommand({ line });
        }
    }
}

Command CommandManager::GetNextCommand(const std::string& line)
{
    auto parsedCommand = CommandParser::FromInputText(line.substr(1), *this);
    return parsedCommand;
}

std::map<std::string, Ref<CommandSpecs>> CommandManager::GetAllSpecs() const
{
    return m_commands;
}