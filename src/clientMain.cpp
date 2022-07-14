#include "./netlib/netlib.h"

// TODO: Validate characters (see IRC)

// TODO: Client
//			- Receive errors
//			- /join command

// TODO: Server
//			- Respond with errors
//			- Should check if msg was received (retry 5 times)
//			- Channels

// TODO: Admin clients
//			- /kick, /mute, /unmute, /whois

// TODO: Commands
//			- Handle Ctrl + C, replace with Ctrl + D

// TODO: Invites

int main()
{
	Logger::Print("Initializing client...\n");
	
	CommandManager commandManager;
	DefaultCmds::RegisterDefaults(&commandManager);
	
	Ref<Socket> sock = nullptr;
	
	// Listener thread that waits for messages from the server.
	Ref<std::thread> listener = nullptr;
	bool hasNickname = false;

	// In the main thread, we wait for SEND and CONNECT commands.

	commandManager.RegisterCommand("connect", { "serverAddress" }, 1, "Connects to a server.", [&commandManager, &listener, &sock](const CommandArgs& args)
	{
		if (sock != nullptr)
		{
			Logger::Print("You are already connected to a server.\n");
			return CommandResult::ERR;
		}

		std::string port = "8080";
		auto server = args[0];

		if (server.find(":") != std::string::npos)
		{
			std::vector<std::string> tokens = Utils::StringSplit(server, ":");
			server = tokens[0];
			port = tokens[1];
		}
		else
		{
			port = "8080";
		}

		sock = CreateRef<Socket>(server.c_str(), port.c_str());
		sock->Connect();

		Logger::Print("Enter your nickname: ");

		// Registers simple socket commands that are easily converted to the IRC protocol and sent over the socket.
		SocketCmds::RegisterSocketCommands(&commandManager, sock.get(), {
			SocketCommandSpec { "ping", { "nickname" }, "Pings the nickname specified. The other user must be connected to the same server." },
			SocketCommandSpec { "nick", { "nickname" }, "Updates the user's nickname." }
		});

		listener = CreateRef<std::thread>([&sock, server, port]()
		{
			// Prints the lines received from the client
			while (true)
			{
				try
				{
					std::string msg = sock->Receive();
					Logger::Print("%s\n", msg.c_str());
				}
				catch (const ConnectionClosedException& e)
				{
					Logger::Print("Connection with server %s:%s closed.\n", server.c_str(), port.c_str());
					break;
				}
			}
		});

		return CommandResult::SUCCESS;
	});

	commandManager.RegisterCommand("ping", {}, 0, "Sends a ping to the server.", [&sock](const CommandArgs& args)
	{
		if (sock == nullptr)
		{
			Logger::Print("You are not connected to a server.\n");
			return CommandResult::ERR;
		}
		
		sock->Send("PING");
		return CommandResult::SUCCESS;
	});

	commandManager.RegisterDefaultCommand([&sock, &hasNickname](const CommandArgs& args) {
		if (sock == nullptr)
		{
			Logger::Print("You are not connected to a server.\n");
			return CommandResult::ERR;
		}

		if (!hasNickname) {
			sock->Send("NICK " + args[0]);
			hasNickname = true;
		}
		else {
			sock->Send(args[0]);
		}

		return CommandResult::SUCCESS;
	});

	commandManager.Poll();

	if (listener != nullptr)
	{
		listener->detach();
	}

	return 0;
}