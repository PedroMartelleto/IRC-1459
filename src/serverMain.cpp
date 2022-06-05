#include "./netlib/netlib.h"

Socket sock = Socket(NULL, "8080");
ThreadSafeQueue<std::string> queue;

int main()
{
	std::cout << "Initializing SERVER..." << std::endl;

	// Binds the socket to a port
	sock.Bind();

	// Listens for incoming connections
	sock.Listen();

	// Accepts an incoming connection
	Socket connectedSocket = sock.Accept();

	auto listenerThread = std::thread();

	// Prints the lines received from the client
	while (true)
	{
		try
		{
			std::string s = connectedSocket.Receive();
			std::cout << s << std::endl;
			connectedSocket.Send(s);
		}
		catch (const ConnectionClosedException& e)
		{
			std::cout << "Connection closed" << std::endl;
			break;
		}
	}

	return 0;
}