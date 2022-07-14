#include "./netlib/netlib.h"
#include <sstream>
#include <thread>

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

	commandManager.Poll();

	server.CleanupThreads();
	listenThread.detach();

	return 0;
}