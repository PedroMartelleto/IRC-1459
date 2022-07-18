#include "defaultCmds.h"
#include <signal.h>
#include <unistd.h>

void DefaultCmds::RegisterDefaults(CommandManager* manager)
{
    signal(SIGINT, SIG_IGN);

    manager->RegisterCommand("quit", {}, 0, "Exits the program.", [](const CommandArgs& args) {
        return CommandResult::EXIT;
    });

    manager->RegisterCommand("help", { "command" }, 0, "Prints a list of commands or the description of one command.", [manager](const CommandArgs& args)
    {
        if (args.size() == 0)
        {
            std::cout << "Available commands:" << std::endl;
            for (const auto& specs : manager->GetAllSpecs())
            {
                specs.second->PrintNameAndDescription();
            }

            return CommandResult::SUCCESS;
        }
        else if (args.size() == 1)
        {
            auto specs = manager->GetCmdSpecsByName(args[0]);

            if (specs != nullptr)
            {
                specs->PrintAllInfo();
                return CommandResult::SUCCESS;
            }
            else
            {
                std::cout << "Unknown command: " << args[0] << std::endl;
                return CommandResult::ERR_PRINT_HELP;
            }
        }
        else
        {
            return CommandResult::ERR_PRINT_HELP;
        }
    });
}