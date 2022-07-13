#pragma once

#include "cmd.h"

class CommandManager
{
public:
    Ref<CommandSpecs> GetCmdSpecsByName(const std::string& name) const; 

    /**
     * @brief Registers a default action for when the user does not start with a slash.
     * @param callback The default action called.
     */
    void RegisterDefaultCommand(const CommandCallback& callback);
    void RegisterCommand(const std::string& name, const CommandArgs& args, int minArgs, const std::string& description, const CommandCallback& callback);
    void RunCommand(const std::string& name, const CommandArgs& args) const;

    void Poll();
    Command GetNextCommand(const std::string& line);

    std::map<std::string, Ref<CommandSpecs>> GetAllSpecs() const;
private:
    std::map<std::string, Ref<CommandSpecs>> m_commands;
    CommandCallback m_defaultCommand = CommandCallback();
};