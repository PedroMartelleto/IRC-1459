#include "server.h"
#include "commons.h"
#include "serverInterpreter.h"
#include <random>
#include <time.h>
#include <signal.h>
#include <set>

// MARK: - Server

int Server::s_defaultPort = 8080;

const std::set<char> forbiddenNicknameCharacters = {
    (char)7, SIGTERM, SIGINT, ' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', ':', ';', '<', '=', '>', '?', '@', '[', '\\', ']', '^', '_', '`', '{', '|', '}', '~'
};

const std::set<char> forbiddenChannelCharacters = {
    (char)7, SIGTERM, SIGINT, ' ', ','
};

bool Server::IsValidNickname(const std::string& nickname)
{
    if (nickname.length() < 1)
    {
        return false;
    }

    for (char c : nickname)
    {
        if (std::find(forbiddenNicknameCharacters.begin(), forbiddenNicknameCharacters.end(), c) != forbiddenNicknameCharacters.end())
        {
            return false;
        }
    }

    return nickname.length() <= 50;
}

bool Server::IsValidChannelName(const std::string& channel)
{
    if (channel.length() < 1 || channel.length() > 50 || (channel[0] != '#' && channel[0] != '&'))
    {
        return false;
    }

    for (char c : channel)
    {
        if (std::find(forbiddenChannelCharacters.begin(), forbiddenChannelCharacters.end(), c) != forbiddenNicknameCharacters.end())
        {
            return false;
        }
    }

    return channel.length() <= 50;
}

std::string Server::CreateTemporaryNickname()
{
    std::stringstream stream;

    for (int i = 0; i < 50; i++)
    {
        // Random char or digit
        char x = (char) (rand() % 10 + '0');
        int r = rand() % 3;
        
        if (r == 0)
        {
            x = (char) (rand() % 26 + 'a');
        }
        else if (r == 1)
        {
            x = (char) (rand() % 26 + 'A');
        }

        stream << x;
    }

    return stream.str();
}

Server::Server(int port) :
    m_port(port <= 0 || port > 65535 ? s_defaultPort : port),
    m_sock(Socket(NULL, std::to_string(m_port).c_str()))
{
    srand(time(NULL));

    if (port <= 0 || port > 65535)
    {
        Logger::Print("Invalid port number. Using default port %d.\n", m_port);
    }

    m_sock.Bind();
}

void Server::Broadcast(const std::string& msg)
{
    Logger::Print("%s\n", msg.c_str());
    
    for (auto& [_, client] : m_clients)
    {
        client->sock.Send(msg);   
    }
}

void Server::BroadcastChannel(const std::string& channel, const std::string& msg)
{
    Logger::Print("%s\n", msg.c_str());
    
    const Ref<Channel>& c = m_channels.at(channel);

    for (auto& user : c->users)
    {
        m_clients.at(user.nickname) -> sock.Send(msg);
    }
}

bool Server::IsNicknameUnique(const std::string& nickname)
{
    bool result = true;

    m_clientsMutex.lock();

    result = m_clients.find(nickname) == m_clients.end();

    m_clientsMutex.unlock();

    return result;
}

void Server::CleanupThreads()
{
    m_deletedThreadsMutex.lock();
    
    for (auto& thread : m_deletedThreads)
    {
        thread->join();
    }
    
    m_deletedThreadsMutex.unlock();
}

void Server::Listen()
{
    while (true)
	{
		// Listens for incoming connections
		m_sock.Listen();

		// Accepts an incoming connection
		Socket connectedSocket = m_sock.Accept();

        auto temporaryNickname = CreateTemporaryNickname();

        // Adds a new client to the list of connected clients
		auto client = CreateRef<ConnectedClient>(ConnectedClient{ temporaryNickname, connectedSocket });
        
        // Spawns a thread to listen for incoming messages from the new client
        m_clientsMutex.lock();

		m_clients[temporaryNickname] = client;
        client->listener = SpawnConnectionListener(temporaryNickname);

        m_clientsMutex.unlock();

		Logger::Print("Received a new connection. Waiting for NICK...\n");
    }
}

void Server::SendToClient(const std::string& msg, const std::string& nickname)
{
	Logger::Print("%s\n", msg.c_str());
    m_clients[nickname]->sock.Send(msg);
}

Ref<std::thread> Server::SpawnConnectionListener(const std::string& temporaryNickname)
{
    auto client = m_clients[temporaryNickname];

    // Spawns a listener thread that waits for messages being received from that socket.
    return CreateRef<std::thread>([client, this, temporaryNickname]()
    {
        ServerInterpreter interpreter(this, client);
        interpreter.RegisterMessages();

		while (true)
		{
			try
			{
				interpreter.Interpret(client->sock.Receive());
			}
			catch (const ConnectionClosedException& e)
            {
                Logger::Print("Client %s disconnected.\n", client->nickname.c_str());
                m_clientsMutex.lock();
                auto listenerThread = client->listener;
                m_clients.erase(client->nickname);
                m_clientsMutex.unlock();

                m_deletedThreadsMutex.lock();
                m_deletedThreads.push_back(listenerThread);
                m_deletedThreadsMutex.unlock();

                break; // Ensures that this thread is terminated
            }
		}
    });
}
