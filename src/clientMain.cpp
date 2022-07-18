#include "./netlib/netlib.h"

int main()
{
	Logger::Print("Initializing client...\n");
	
	CommandManager commandManager;
	DefaultCmds::RegisterDefaults(&commandManager);
	
	Ref<Socket> sock = nullptr;
	
	// Listener thread that waits for messages from the server.
	Ref<std::thread> listener = nullptr;
	bool hasNickname = false;
	std::mutex replyCallbacksMutex;
	std::vector<std::function<void(int)>> replyCallbacks;

	// In the main thread, we wait for SEND and CONNECT commands.

	commandManager.RegisterCommand("connect", { "serverAddress" }, 1, "Connects to a server.", [&commandManager, &listener, &sock, &replyCallbacks, &replyCallbacksMutex](const CommandArgs& args)
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

		listener = CreateRef<std::thread>([&sock, &replyCallbacks, &replyCallbacksMutex, server, port]()
		{
			// Prints the lines received from the client
			while (true)
			{
				try
				{
					const std::string msg = sock->Receive();
					const auto trimmedMsg = Utils::StringTrim(msg);

					// Checks for server respones with 3 digits.
					if (trimmedMsg.size() == 3)
					{
						try
						{
							// Interprets the response and prints it.
							int code = std::stoi(trimmedMsg);

							if (code < 1 || code > 1000)
							{
								Logger::Print("Server responded with invalid reply code: %s\n", trimmedMsg.c_str());
								continue;
							}
							
							if (code >= 1 && code < 400)
							{
								if (RPL_NAMES.find(trimmedMsg) != RPL_NAMES.end())
								{
									Logger::Print("[%d] %s\n", code, RPL_NAMES[trimmedMsg].c_str());
								}
								else
								{
									Logger::Print("Server responded with [%d].\n", code);
								}
							}
							else
							{
								if (ERR_NAMES.find(trimmedMsg) != ERR_NAMES.end())
								{
									Logger::Print("[%d] %s\n", code, ERR_NAMES[trimmedMsg].c_str());
								}
								else
								{
									Logger::Print("Server responded with error [%d].\n", code);
								}
							}

							if (replyCallbacks.size() > 0)
							{
								replyCallbacksMutex.lock();
								auto callback = replyCallbacks[replyCallbacks.size() - 1];
								replyCallbacks.pop_back();
								replyCallbacksMutex.unlock();
								callback(code);
							}

							continue;
						}
						catch (const std::exception& e) {}
					}

					// Otherwise, just echoes the message.
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

	commandManager.RegisterDefaultCommand([&sock, &hasNickname, &replyCallbacks, &replyCallbacksMutex](const CommandArgs& args) {
		if (sock == nullptr)
		{
			Logger::Print("You are not connected to a server.\n");
			return CommandResult::ERR;
		}

		if (!hasNickname) {
			sock->Send("NICK " + args[0]);
			replyCallbacksMutex.lock();
			replyCallbacks.push_back([&hasNickname](int replyCode) {
				if (replyCode < 400)
				{
					hasNickname = true;
				}
			});
			replyCallbacksMutex.unlock();
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