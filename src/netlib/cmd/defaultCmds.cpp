#include "defaultCmds.h"

void DefaultCmds::RegisterDefaults(CommandManager* manager)
{
    manager->RegisterCommand("QUIT", {}, [](const CommandArgs& args) {
        return CommandResult::TERMINATE;
    });

    manager->RegisterCommand("HELP", { "command" }, [manager](const CommandArgs& args)
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
                return CommandResult::PRINT_HELP;
            }
        }
        else
        {
            return CommandResult::PRINT_HELP;
        }
    }, 0);
}