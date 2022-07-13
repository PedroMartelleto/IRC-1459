#include "server.h"
#include "commons.h"
#include <random>
#include <time.h>

// MARK: - ConnectedClient

bool ConnectedClient::operator==(const ConnectedClient& other) const
{
    return nickname == other.nickname;
}

ConnectedClient& ConnectedClient::operator=(const ConnectedClient& other)
{
    nickname = other.nickname;
    listener = other.listener;
    sock = other.sock;
    return *this;
}

ConnectedClient& ConnectedClient::operator=(ConnectedClient& other)
{
    nickname = other.nickname;
    listener = other.listener;
    sock = other.sock;
    return *this;
}


// MARK: - Server

int Server::s_defaultPort = 8080;

bool Server::IsValidNickname(const std::string& nickname)
{
	// TODO: Validate characters (see IRC), modularize
	return nickname.length() <= 9;
}

std::string Server::CreateTemporaryNickname()
{
    std::stringstream stream;

    for (int i = 0; i < 9; i++)
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

void Server::Broadcast(const std::string& msg, const Ref<ConnectedClient>& excludeClient)
{
    Logger::Print("%s\n", msg.c_str());
    
    m_clientsMutex.lock();

    for (auto& [_, client] : m_clients)
    {
        if (client != excludeClient)
        {
            client->sock.Send(msg);
        }
    }

    m_clientsMutex.unlock();
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
        
        m_clientsMutex.lock();

		m_clients[temporaryNickname] = client;
        client->listener = SpawnConnectionListener(temporaryNickname);

        m_clientsMutex.unlock();

		Logger::Print("Received a new connection. Waiting for NICK...\n");
    }
}

Ref<std::thread> Server::SpawnConnectionListener(const std::string& temporaryNickname)
{
    auto client = m_clients[temporaryNickname];

    // Spawns a listener thread that waits for messages being received from that socket.
    return CreateRef<std::thread>([client, this, temporaryNickname]()
    {
        bool hasTemporaryNickname = true;

        // Prints the lines received from the client
        while (true)
        {
            try
            {
                std::string msg = client->sock.Receive();
                bool isNickMessage = msg.substr(0, 5) == "NICK ";

                if (hasTemporaryNickname && !isNickMessage)
                {
                    continue;
                }

                if (isNickMessage)
                {
                    auto nickname = Utils::StringTrim(msg.substr(5));
                    
                    if (Server::IsValidNickname(nickname) && Server::IsNicknameUnique(nickname))
                    {
                        m_clientsMutex.lock();
                        
                        auto oldNickname = client->nickname;

                        client->nickname = nickname;
						m_clients[client->nickname] = client;
						m_clients.erase(temporaryNickname);
                        m_clientsMutex.unlock();

                        if (hasTemporaryNickname)
                        {
						    Broadcast(nickname + " has joined the chat.", client);
                        }
                        else
                        {
                            Broadcast(oldNickname + " changed their nickname to " + nickname + ".", nullptr);
                        }

						hasTemporaryNickname = false;
                    }
                }
                else
                {
                    Broadcast(client->nickname + ": " + msg, client);
                }
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