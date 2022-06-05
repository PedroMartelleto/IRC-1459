#pragma once

#include "cmd.h"

class CommandManager
{
public:
    Ref<CommandSpecs> GetCmdSpecsByName(const std::string& name) const; 

    void RegisterCommand(const std::string& name, const CommandArgs& args, const CommandCallback& callback, int minArgs = -1);
    void RunCommand(const std::string& name, const CommandArgs& args);

    void Poll();
    Command GetNextCommand();

    std::map<std::string, Ref<CommandSpecs>> GetAllSpecs() const;
private:
    std::map<std::string, Ref<CommandSpecs>> m_commands;
};