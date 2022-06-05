#include "./netlib/netlib.h"

Socket sock = Socket("127.0.0.1", "8080");

int main()
{
	std::cout << "Initializing CLIENT..." << std::endl;

	// Connects to the server
	sock.Connect();

	CommandManager commandManager;
	DefaultCmds::RegisterDefaults(&commandManager);

	commandManager.RegisterCommand("SEND", {"message"}, 1, "Sends a message.", [](const CommandArgs& args) {
		return CommandResult::SUCCESS;
	});

	commandManager.Poll();

	return 0;
}