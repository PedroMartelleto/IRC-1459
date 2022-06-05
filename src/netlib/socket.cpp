#include "socket.h"

#define MAX_MSG_LEN 4096

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
	int status = getaddrinfo(address, service, &hints, &m_address);

	if (status != 0)
	{
		error("getaddrinfo");
	}

	// Creates the socket
	m_socketFile = socket(m_address->ai_family, m_address->ai_socktype, m_address->ai_protocol);
}

void Socket::Bind()
{
	// Sets the socket to reuse the address (to avoid "address already in use")
	int yes = 1;
	int result = setsockopt(m_socketFile, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	if (result == -1)
		error("setsockopt");

	// Tries to bind to each address in the addr linked list until one succeeds
	result = -1;

	while (m_address != NULL)
	{
		result = bind(m_socketFile, m_address->ai_addr, m_address->ai_addrlen);

		if (result == 0)
			break;

		m_address = m_address->ai_next;
	}

	if (result != 0)
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
	Socket newConnection = Socket();

	newConnection.m_socketFile = accept(this->m_socketFile, NULL, NULL);

	if (newConnection.m_socketFile == -1)
	{
		error("accept");
	}

	Logger::Print("Accepted connection from %s.\n", newConnection.m_address->ai_addr);

	return newConnection;
}

// Connects to the specified address
void Socket::Connect()
{
	// Tries to connect to each address in the addr linked list until one succeeds
	int result;
	while (m_address != NULL)
	{
		result = connect(m_socketFile, m_address->ai_addr, this->m_address->ai_addrlen);

		if (result == 0)
		{
			std::cout << "Connected to " << m_address->ai_addr << std::endl;
			break;
		}

		m_address = m_address->ai_next;
	}

	if (result != 0)
	{
		error("connect");
	}
}

void Socket::SendFragment(const std::string& str)
{
	int bytesSent = send(m_socketFile, str.c_str(), str.length() + 1, 0);

	if (bytesSent == -1)
	{
		error("send");
	}

	if (bytesSent == 0)
	{
		throw ConnectionClosedException();
	}
}

char* Socket::ReceiveFragment()
{
	char* buffer = new char[MAX_MSG_LEN];

	for (int i = 0; i < MAX_MSG_LEN; i++)
	{
		buffer[i] = '\0';
	}

	int bytesReceived = recv(m_socketFile, buffer, MAX_MSG_LEN, 0);

	if (bytesReceived < 0)
	{
		delete[] buffer;
		error("recv");
	}

	if (bytesReceived == 0)
	{
		delete[] buffer;
		throw ConnectionClosedException();
	}

	return buffer;
}

// Sends a string
void Socket::Send(const std::string& data)
{
	if (data.length() == 0)
	{
		return;
	}

	std::vector<std::string> fragments;
	int fragmentCount = data.length() / MAX_MSG_LEN;
	int lastFragmentSize = data.length() % MAX_MSG_LEN;

	if (lastFragmentSize > 0)
	{
		fragmentCount++;
	}

	for (int i = 0; i < fragmentCount; i++)
	{
		int fragmentSize = MAX_MSG_LEN;

		if (i == fragmentCount - 1)
		{
			fragmentSize = lastFragmentSize;
		}

		std::string fragment = data.substr(i * MAX_MSG_LEN, fragmentSize);
		fragments.push_back(fragment);
	}

	for (const auto& fragment : fragments)
	{
		SendFragment(fragment);
	}
}

// Receives a string
std::string Socket::Receive()
{
	std::stringstream stream;

	char* msg = ReceiveFragment();

	while (msg[MAX_MSG_LEN-1] != '\0')
	{
		stream << msg;
		delete[] msg;
		msg = ReceiveFragment();
	}

	stream << msg;
	delete[] msg;

	return stream.str();
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
		shutdown(m_socketFile, SHUT_RDWR);
		return;
	}

	if (stopSends)
	{
		shutdown(m_socketFile, SHUT_WR);
		return;
	}

	if (stopReceives)
	{
		shutdown(m_socketFile, SHUT_RD);
		return;
	}
}