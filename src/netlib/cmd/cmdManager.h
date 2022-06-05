#pragma once

#include "cmd.h"

class CommandManager
{
public:
    Ref<CommandSpecs> GetCmdSpecsByName(const std::string& name) const; 

    void RegisterCommand(const std::string& name, const CommandArgs& args, int minArgs, const std::string& description,  const CommandCallback& callback);
    void RunCommand(const std::string& name, const CommandArgs& args) const;

    void Poll();
    Command GetNextCommand();

    std::map<std::string, Ref<CommandSpecs>> GetAllSpecs() const;
private:
    std::map<std::string, Ref<CommandSpecs>> m_commands;
};