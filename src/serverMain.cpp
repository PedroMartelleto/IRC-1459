#include "./netlib/netlib.h"

int main()
{
	// Creates a socket on localhost with port 8080
	Socket socket = Socket(NULL, "8080");

	// Binds the socket to a port
	socket.Bind();

	// Listens for incoming connections
	socket.Listen();

	// Accepts an incoming connection
	Socket connectedSocket = socket.Accept();

	// Prints the lines received from the client
	while (true)
	{
		try
		{
			std::string s = connectedSocket.Receive(1024);
			std::cout << s << std::endl;
			connectedSocket.Send(s);
		}
		catch (const ConnectionCloseException& e)
		{
			std::cout << "Connection closed" << std::endl;
			break;
		}
	}

	return 0;
}