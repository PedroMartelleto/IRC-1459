#include "./netlib/netlib.h"

int main()
{
	Logger::Print("Initializing client...\n");
	
	bool hasNickname = false;

	std::string server = "127.0.0.1";
	std::string port = "8080";

	Socket sock = Socket(server.c_str(), port.c_str());

	// Connects to the server
	sock.Connect();

	CommandManager commandManager;
	DefaultCmds::RegisterDefaults(&commandManager);
	
	Logger::Print("Enter your nickname: ");

	// Listener thread that waits for messages from the server.
	auto listener = std::thread([&sock, server, port]()
	{
		// Prints the lines received from the client
		while (true)
		{
			try
			{
				std::string msg = sock.Receive();
				Logger::Print("%s\n", msg.c_str());
			}
			catch (const ConnectionClosedException& e)
			{
				Logger::Print("Connection with server %s:%s closed.\n", server.c_str(), port.c_str());
				break;
			}
		}
	});

	// In the main thread, we wait for SEND commands.
	commandManager.RegisterDefaultCommand([&sock, &hasNickname](const CommandArgs& args) {
		if (!hasNickname) {
			sock.Send("NICK " + args[0]);
			hasNickname = true;
		}
		else {
			sock.Send(args[0]);
		}

		return CommandResult::SUCCESS;
	});

	commandManager.Poll();

	listener.join();

	return 0;
}