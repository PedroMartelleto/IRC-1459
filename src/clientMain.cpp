#include "./netlib/netlib.h"

Ref<Socket> socket = CreateRef<Socket>("127.0.0.1", "8080");

int main()
{
	// Connects to the server
	socket->Connect();

	CommandManager commandManager;
	DefaultCmds::RegisterDefaults(&commandManager);

	commandManager.RegisterCommand("SEND", {"message"}, 1, "Sends a message.", [](const CommandArgs& args) {
		return CommandResult::SUCCESS;
	});

	commandManager.Poll();

	return 0;
}