#pragma once

#include <sstream>
#include <thread>
#include "threads/logger.h"
#include "socket.h"
#include "ircMessageInterpreter.h"


struct ConnectedClient
{
	std::string nickname;
	Socket sock;
	Ref<std::thread> listener = nullptr;

	std::string channel;
	
	bool isMuted;
	bool isOperator;

	bool operator==(const ConnectedClient& other) const;
	ConnectedClient& operator=(const ConnectedClient& other);
	ConnectedClient& operator=(ConnectedClient& other);
};