#include "socket.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

// Throws an error
void error(const char *msg)
{
	perror(msg);
	throw msg;
}

Socket::Socket(const char *address, const char *service)
{
	// Creates the structure for storing the address information
	struct addrinfo hints;

	// Fills the hints structure with flags
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	// Gets the address information using the hints
	// The available addresses are stored in this->addr, which is a linked list
	int s = getaddrinfo(address, service, &hints, &this->m_address);

	if (s != 0)
		error("getaddrinfo");

	// Creates the socket
	this->m_socketFile = socket(this->m_address->ai_family, this->m_address->ai_socktype,
								this->m_address->ai_protocol);
}

void Socket::Bind()
{
	// Sets the socket to reuse the address (to avoid "address already in use")
	int yes = 1;
	int s = setsockopt(this->m_socketFile, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	if (s == -1)
		error("setsockopt");

	// Tries to bind to each address in the addr linked list until one succeeds
	s = -1;

	while (this->m_address != NULL)
	{
		s = bind(this->m_socketFile, this->m_address->ai_addr, this->m_address->ai_addrlen);

		if (s == 0)
			break;

		this->m_address = this->m_address->ai_next;
	}

	if (s != 0)
		error("bind");
}

void Socket::Listen()
{
	int s = listen(this->m_socketFile, 5);

	if (s == -1)
	{
		error("listen");
	}
}

Socket Socket::Accept()
{
	Socket new_conn = Socket();

	new_conn.m_socketFile = accept(this->m_socketFile, NULL, NULL);

	if (new_conn.m_socketFile == -1)
		error("accept");

	return new_conn;
}

// Connects to the specified address
void Socket::Connect()
{
	// Tries to connect to each address in the addr linked list until one succeeds
	int s;
	while (this->m_address != NULL)
	{
		s = connect(m_socketFile, m_address->ai_addr, this->m_address->ai_addrlen);

		if (s == 0)
		{
			break;
		}

		m_address = m_address->ai_next;
	}

	if (s != 0)
		error("connect");
}

// Sends a string
void Socket::Send(std::string data)
{
	int bytes_sent = send(this->m_socketFile, data.c_str(), data.length() + 1, 0);

	if (bytes_sent == -1)
		error("send");

	if (bytes_sent == 0)
		throw ConnectionCloseException();
}

// Receives a string
std::string Socket::Receive(int maxSize)
{
	char buffer[maxSize];
	int bytesReceived = recv(this->m_socketFile, buffer, maxSize, 0);

	if (bytesReceived == -1)
	{
		error("recv");
	}

	if (bytesReceived == 0)
	{
		throw ConnectionCloseException();
	}

	return std::string(buffer);
}

// Closes the connection
void Socket::Close()
{
	Close(true, true);
}

// Closes the connection, optionally stopping only sending or receiving
void Socket::Close(bool stopSends, bool stopReceives)
{
	if (stopSends && stopReceives)
	{
		shutdown(this->m_socketFile, SHUT_RDWR);
		return;
	}

	if (stopSends)
	{
		shutdown(this->m_socketFile, SHUT_WR);
		return;
	}

	if (stopReceives)
	{
		shutdown(this->m_socketFile, SHUT_RD);
		return;
	}
}