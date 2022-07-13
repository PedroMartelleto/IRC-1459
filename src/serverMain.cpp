#include "./netlib/netlib.h"
#include <sstream>
#include <thread>

struct ConnectedClient {
	Ref<std::string> nickname = nullptr;
	Ref<std::thread> listener;
	Socket sock;

	bool operator==(const ConnectedClient& other) const {
		return *nickname == *other.nickname;
	}
};

bool IsValidNickname(const std::string& nickname) {
	// TODO: Validate characters (see IRC), modularize, nicks collision
	return nickname.length() <= 9;
}

int main()
{
	Logger::Print("Initializing server...\n");

	CommandManager commandManager;
	DefaultCmds::RegisterDefaults(&commandManager);
	
	// 	Logger::Print("Type /help for a list of commands.\n");
	Socket sock = Socket(NULL, "8080");

	// Binds the socket to a port
	sock.Bind();

	ThreadSafeVector<Ref<ConnectedClient>> clients;

	while (true) {
		// Listens for incoming connections
		sock.Listen();

		// Accepts an incoming connection
		Socket connectedSocket = sock.Accept();

		auto client = CreateRef<ConnectedClient>(ConnectedClient{ nullptr, nullptr, connectedSocket });
		int index = (int) clients.Size();
		clients.Push(client);
		Logger::Print("%u\n", clients.Size());

		Logger::Print("Received a new connection. Waiting for NICK...\n");

		// Spawns a listener thread that waits for messages being received from that socket.
		auto listener = CreateRef<std::thread>([index, &clients]()
		{
			Logger::Print("%d %u\n", index, clients.Size());
			auto client = clients[index];

			// Prints the lines received from the client
			while (true)
			{
				try
				{
					std::string msg = client->sock.Receive();

					if (client->nickname == nullptr)
					{
						if (msg.substr(0, 5) != "NICK ")
						{
							continue;
						}

						auto nickname = Utils::StringTrim(msg.substr(5));
						
						if (IsValidNickname(nickname))
						{
							clients.Lock();
							client->nickname = CreateRef<std::string>(nickname);
							clients.Unlock();
						}

						auto messageToBroadcast = nickname + " has joined the chat.";
						Logger::Print("%s\n", messageToBroadcast.c_str());

						clients.Iterate([&messageToBroadcast, &client](const auto& otherClient)
						{
							if (otherClient != client)
							{
								otherClient->sock.Send(messageToBroadcast);
							}
						});
					}
					else {
						std::string messageToBroadcast = *client->nickname + ": " + msg;

						Logger::Print("%s\n", messageToBroadcast.c_str());
						
						clients.Iterate([&messageToBroadcast, &client](const auto& otherClient)
						{
							if (otherClient != client)
							{
								otherClient->sock.Send(messageToBroadcast);
							}
						});
					}
				}
				catch (const ConnectionClosedException& e)
				{
					Logger::Print("Client %s disconnected.\n", client->nickname->c_str());
					clients.RemoveAt(clients.IndexOf(client));
					break;
				}
			}
		});

		clients.Lock();
		client->listener = listener;
		clients.Unlock();
	}

	return 0;
}