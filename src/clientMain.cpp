#include "./netlib/netlib.h"

int main()
{
	Logger::Print("Initializing client...\n");
	
	Socket sock = Socket("127.0.0.1", "8080");

	// Connects to the server
	sock.Connect();

	CommandManager commandManager;
	DefaultCmds::RegisterDefaults(&commandManager);
	
	// Listener thread that waits for messages from the server.
	auto listener = std::thread([&sock]()
	{
		// Prints the lines received from the client
		while (true)
		{
			try
			{
				std::string msg = sock.Receive();
				Logger::Print(":server %s\n", msg.c_str());
			}
			catch (const ConnectionClosedException& e)
			{
				Logger::Print("Connection closed.\n");
				break;
			}
		}
	});

	// In the main thread, we wait for SEND commands.
	commandManager.RegisterCommand("SEND", {"message"}, 1, "Sends a message.", [&sock](const CommandArgs& args) {
		std::stringstream message;
		message << args[0];
		for (int i = 1; i < (int)args.size(); ++i) {
			message << " " << args[i];
		}
		sock.Send(message.str());
		return CommandResult::SUCCESS;
	});

	commandManager.Poll();

	listener.join();

	return 0;
}