#include "./netlib/netlib.h"
#include <sstream>
#include <thread>

// TODO: Validate characters (see IRC)

// TODO: How will ping work?

// TODO: Client
//			- Error handling
//			- Automate command sends
//			- /join, /nick command

// TODO: Server
//			- Should check if msg was received (retry 5 times)
//			- Channels

// TODO: Admin clients
//			- /kick, /mute, /unmute, /whois

// TODO: Commands
//			- Handle Ctrl + C, replace with Ctrl + D

// TODO: Invites

int main()
{
	Logger::Print("Initializing server...\n");

	CommandManager commandManager;
	DefaultCmds::RegisterDefaults(&commandManager);

	// 	Logger::Print("Type /help for a list of commands.\n");
	Server server(8080);

	auto listenThread = std::thread([&server]() {
		server.Listen();
	});

	// TODO: How to clean up if server is sleeping?

	commandManager.Poll();

	server.CleanupThreads();
	listenThread.detach();

	return 0;
}