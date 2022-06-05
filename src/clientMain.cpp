#include "./netlib/netlib.h"

int main()
{
	// Creates a socket on localhost with port 8080
	Socket socket = Socket("127.0.0.1", "8080");

	// Connects to the server
	socket.Connect();

	// Sends lines
	while (true)
	{
		std::string line;
		std::getline(std::cin, line);
		socket.Send(line);
	}
	return 0;
}