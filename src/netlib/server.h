#pragma once

#include <sstream>
#include <thread>
#include "threads/logger.h"
#include "socket.h"
#include "ircMessageInterpreter.h"

/**
 * @brief The server forms the backbone of IRC, providing a point to which
   clients may connect to to talk to each other, and a point for other
   servers to connect to, forming an IRC network.  The only network
   configuration allowed for IRC servers is that of a spanning tree [see
   Fig. 1] where each server acts as a central node for the rest of the
   net it sees.


                           [ Server 15 ]  [ Server 13 ] [ Server 14]
                                 /                \         /
                                /                  \       /
        [ Server 11 ] ------ [ Server 1 ]       [ Server 12]
                              /        \          /
                             /          \        /
                  [ Server 2 ]          [ Server 3 ]
                    /       \                      \
                   /         \                      \
           [ Server 4 ]    [ Server 5 ]         [ Server 6 ]
            /    |    \                           /
           /     |     \                         /
          /      |      \____                   /
         /       |           \                 /
 [ Server 7 ] [ Server 8 ] [ Server 9 ]   [ Server 10 ]

                                  :
                               [ etc. ]
                                  :
 * 
 */

// TODO: Bugs

// TODO: Client
//			- Handle server errors
//			- /join command

// TODO: Server
//			- Should check if msg was received (retry 5 times) (basic version implemented inside the Socket::SendFragment function, check later if it works)
//			- Channels

// TODO: Admin clients
//			- /kick, /mute, /unmute, /whois

// TODO: Invites


struct ConnectedClient
{
	std::string nickname;
	Socket sock;
	Ref<std::thread> listener = nullptr;

	bool operator==(const ConnectedClient& other) const;
	ConnectedClient& operator=(const ConnectedClient& other);
	ConnectedClient& operator=(ConnectedClient& other);
};

class Server
{
public:
	friend class ServerInterpreter;

	static bool IsValidNickname(const std::string& nickname);
	static std::string CreateTemporaryNickname();

	Server(int port);
	
	void CreateValidationFunctions();
	void CleanupThreads();
	void Listen();

	void SendToClient(const std::string& msg, const std::string& nickname);
	void Broadcast(const std::string& msg, const Ref<ConnectedClient>& excludeClient = nullptr);
	bool IsNicknameUnique(const std::string& nickname);

	Ref<std::thread> SpawnConnectionListener(const std::string& temporaryNickname);
private:
	static int s_defaultPort;

	std::map<std::string, Ref<ConnectedClient>> m_clients;
	std::mutex m_clientsMutex;

	std::vector<Ref<std::thread>> m_deletedThreads;
	std::mutex m_deletedThreadsMutex;

	int m_port;
	Socket m_sock;	
};