#pragma once

#include "cmdManager.h"

class CommandParser
{
public:
    /**
     * @brief Parses a command from a string.
     * 
     * @param inputText 
     * @param manager 
     * @return Command 
     */
    static Command FromInputText(const std::string& inputText, const CommandManager& manager);
};