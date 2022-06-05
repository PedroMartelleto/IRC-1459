#include "cmdParser.h"

Command CommandParser::FromInputText(const std::string& inputText, const CommandManager& manager)
{
    std::vector<std::string> tokens = Utils::StringSplit(inputText, " ");
    
    if (tokens.size() <= 0)
    {
        return Command::Invalid;
    }

    // Sets command name to upper case
    for (auto & chr: tokens[0])
    {
        chr = toupper(chr);
    }

    std::string commandName = tokens[0];
    auto specs = manager.GetCmdSpecsByName(commandName);

    // Error detection
    if (specs == nullptr)
    {
        std::cout << "Command " << tokens[0] << " is not defined." << std::endl;
        return Command::Invalid;
    }

    if (specs->minArgs > (int)tokens.size() - 1)
    {
        std::cout << "The command " << tokens[0] << " requires " << specs->minArgs << " arguments, but only " << tokens.size()-1 << " arguments were passed.\n";
        return Command::Invalid;
    }
    
    // Creates the command object
    Command command;
    command.specs = specs;

    for (auto i = 0; i < (int)tokens.size() - 1; i++)
    {
        command.args.push_back(tokens[i + 1]);
    }

    return command;
}
