#pragma once

#include "commons.h"

struct SocketCommandSpec {
    std::string name;
    std::vector<std::string> args;
    std::string description;
};

class CommandManager;
class Socket;

namespace SocketCmds {
    void RegisterSocketCommands(CommandManager* manager, Socket* socket, std::vector<SocketCommandSpec> specs);
}