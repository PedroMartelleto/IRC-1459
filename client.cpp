#include <netdb.h>
#include <sys/socket.h>

#include <iostream>
#include <string>

#include "socket.h"

int main() {
  // Creates a socket on localhost with port 8080
  Socket sock = Socket("127.0.0.1", "8080");

  // Connects to the server
  sock.Connect();

  // Sends lines
  while (true) {
    std::string line;
    std::getline(std::cin, line);
    sock.Send(line);
  }
  return 0;
}