#include "socketCmds.h"
#include "socket.h"
#include "cmd/cmdManager.h"

namespace SocketCmds {
    void RegisterSocketCommands(CommandManager* manager, Socket* socket, std::vector<SocketCommandSpec> specs)
    {
        for (const auto& spec : specs)
        {
            manager->RegisterCommand(spec.name, spec.args, spec.args.size(), spec.description, [spec, &socket](const CommandArgs& args) 
            {
                if (socket == nullptr)
                {
                    Logger::Print("You must be connected to a server.\n");
                    return CommandResult::ERR;
                }

                socket->Send(Utils::StringUpperCase(spec.name) + " " + Utils::StringJoin(args, " "));
                return CommandResult::SUCCESS;
            });
        }
    }
}