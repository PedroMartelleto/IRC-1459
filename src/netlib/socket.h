#pragma once

#include "commons.h"
#include <string>

// Wrapper for socket functions
class Socket
{
public:
	/**
	 * @brief Construct a new Socket:: Socket object
	 * 
	 * @param address URL or IP address of the server (must be NULL when hosting a service).
	 * @param service port number or name of the service (e.g. "8080" or "http").
	 */
	Socket(const char *address, const char *service);

	// Functions used hosting a service on a port
	void Bind();	 // Binds the socket to a port
	void Listen();	 // Listens for incoming connections
	Socket Accept(); // Accepts an incoming connection

	// Functions used for communication with a server
	int Connect(); // Connects to the specified address

	// Functions used for sending and receiving data
	void Send(const std::string& msg);
	std::string Receive();

	// Gets the IP address of the enpoint of this socket
	std::string GetIP();

	// Functions used for closing the connection
	// Optionally stops only sending or receiving
	void Close();
	void Close(bool stopSends, bool stopReceives);

	Socket& operator=(const Socket& other);

	Socket& operator=(Socket& other);

	bool IsValidSocket() const;

private:
	// Private constructor used when manually creating a socket
	Socket() {}

	/**
	 * @brief Internal use. Sends a string to the connected server.
	 * 
	 * @param fragment string to send.
	 * @param length length of the string, maximum of 4096 bytes.
	 */
	void SendFragment(const char* fragment, int fragmentSize);

	/**
	 * @brief Internal use. Receives a string from the connected server.
	 */
	char* ReceiveFragment();

	// Socket file descriptor
	int m_socketFile = 0;

	// Address info for the socket
	struct addrinfo *m_address = nullptr;
};

// Exception thrown when the connection is closed after calling Receive()
class ConnectionClosedException : public std::exception
{
public:
	ConnectionClosedException() : std::exception() {}
};