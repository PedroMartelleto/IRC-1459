#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "socket.h"

int main() {
  // Creates a socket on localhost with port 8080
  Socket sock = Socket(NULL, "8080");

  // Binds the socket to a port
  sock.Bind();

  // Listens for incoming connections
  sock.Listen();

  // Accepts an incoming connection
  Socket connected_sock = sock.Accept();

  // Prints the lines received from the client
  while (true) {
    try {
      std::string s = connected_sock.Receive(1024);
      std::cout << s << std::endl;
      connected_sock.Send(s);
    } catch (ConnectionCloseException e) {
      std::cout << "Connection closed" << std::endl;
      break;
    }
  }

  return 0;
}