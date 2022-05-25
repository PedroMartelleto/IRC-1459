#pragma once

#include <netdb.h>
#include <sys/socket.h>

#include <string>

// Wrapper for socket functions
class Socket {
 public:
  // Constructor of the class
  // address: URL or IP address of the server
  //         (must be NULL when hosting a service)
  // service: port number or name of the service (e.g. "8080" or "http")
  Socket(const char *address, const char *service);

  // Functions used hosting a service on a port
  void Bind();      // Binds the socket to a port
  void Listen();    // Listens for incoming connections
  Socket Accept();  // Accepts an incoming connection

  // Functions used for communication with a server
  void Connect();  // Connects to the specified address

  // Functions used for sending and receiving data
  void Send(std::string s);
  std::string Receive(int max_size);

  // Functions used for closing the connection
  // Optionally stops only sending or receiving
  void Close();
  void Close(bool stop_sends, bool stop_receives);

 private:
  // Private constructor used when manually creating a socket
  Socket() {}

  // Socket file descriptor
  int sockfd;

  // Address info for the socket
  struct addrinfo *addr;
};

// Exception thrown when the connection is closed after calling Receive()
class ConnectionCloseException : public std::exception {
 public:
  ConnectionCloseException() : std::exception() {}
};