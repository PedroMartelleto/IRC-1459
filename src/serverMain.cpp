#include "./netlib/netlib.h"
#include <sstream>

Socket sock = Socket(NULL, "8080");

int main()
{
	std::cout << "Initializing SERVER..." << std::endl;

	auto commandManager = CommandManager();
	DefaultCmds::RegisterDefaults(&commandManager);

	// Binds the socket to a port
	sock.Bind();

	// Listens for incoming connections
	sock.Listen();

	// Accepts an incoming connection
	Socket connectedSocket = sock.Accept();

	// Listener thread that waits for messages being received.
	auto listener = std::thread([connectedSocket]()
	{
		// Prints the lines received from the client
		while (true)
		{
			try
			{
				std::string msg = connectedSocket.Receive();
				Logger::Print(":client %s\n", msg);
				connectedSocket.Send(msg);
			}
			catch (const ConnectionClosedException& e)
			{
				Logger::Print("Connection closed.\n");
				break;
			}
		}
	});

	// In the main thread, we wait for SEND commands.
	commandManager.RegisterCommand("SEND", {"message"}, 1, "Sends a message.", [connectedSocket](const CommandArgs& args) {
		std::stringstream message;
		message << args[0];
		for (int i = 1; i < args.size(); ++i) {
			message << " " << args[i];
		}
		connectedSocket.Send(message.str());
		return CommandResult::SUCCESS;
	});

	commandManager.Poll();

	listener.join();

	return 0;
}