#include "./netlib/netlib.h"


int main()
{
	// Creates a socket on localhost with port 8080
	//Socket socket = Socket("127.0.0.1", "8080");

	// Connects to the server
	//socket.Connect();

	CommandManager commandManager;
	DefaultCmds::RegisterDefaults(&commandManager);

	commandManager.RegisterCommand("SEND", {"message"}, [](const CommandArgs& args) {
		return CommandResult::SUCCESS;
	});

	commandManager.Poll();

	return 0;
}