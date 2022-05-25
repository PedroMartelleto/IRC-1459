#include "socket.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

// Throws an error
void error(const char *msg) {
  perror(msg);
  throw msg;
}

// Constructor of the class
// address: URL or IP address of the server (must be NULL when hosting a
// service) service: port number or name of the service (e.g. "8080" or "http")
Socket::Socket(const char *address, const char *service) {
  // Creates the structure for storing the address information
  struct addrinfo hints;

  // Fills the hints structure with flags
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  // Gets the address information using the hints
  // The available addresses are stored in this->addr, which is a linked list
  int s = getaddrinfo(address, service, &hints, &this->addr);

  if (s != 0) error("getaddrinfo");

  // Creates the socket
  this->sockfd = socket(this->addr->ai_family, this->addr->ai_socktype,
                        this->addr->ai_protocol);
}

// Binds the socket to a port
void Socket::Bind() {
  // Sets the socket to reuse the address (to avoid "address already in use")
  int yes = 1;
  int s = setsockopt(this->sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

  if (s == -1) error("setsockopt");

  // Tries to bind to each address in the addr linked list until one succeeds
  s = -1;

  while (this->addr != NULL) {
    s = bind(this->sockfd, this->addr->ai_addr, this->addr->ai_addrlen);

    if (s == 0) break;

    this->addr = this->addr->ai_next;
  }

  if (s != 0) error("bind");
}

// Listens for incoming connections
void Socket::Listen() {
  int s = listen(this->sockfd, 5);

  if (s == -1) error("listen");
}

// Accepts an incoming connection
Socket Socket::Accept() {
  Socket new_conn = Socket();

  new_conn.sockfd = accept(this->sockfd, NULL, NULL);

  if (new_conn.sockfd == -1) error("accept");

  return new_conn;
}

// Connects to the specified address
void Socket::Connect() {
  // Tries to connect to each address in the addr linked list until one succeeds
  int s;
  while (this->addr != NULL) {
    s = connect(this->sockfd, this->addr->ai_addr, this->addr->ai_addrlen);

    if (s == 0) break;

    this->addr = this->addr->ai_next;
  }

  if (s != 0) error("connect");
}

// Sends a string
void Socket::Send(std::string data) {
  int bytes_sent = send(this->sockfd, data.c_str(), data.length() + 1, 0);

  if (bytes_sent == -1) error("send");

  if (bytes_sent == 0) throw ConnectionCloseException();
}

// Receives a string
std::string Socket::Receive(int max_size) {
  char buffer[max_size];
  int bytes_received = recv(this->sockfd, buffer, max_size, 0);

  if (bytes_received == -1) error("recv");

  if (bytes_received == 0) throw ConnectionCloseException();

  return std::string(buffer);
}

// Closes the connection
void Socket::Close() { this->Close(true, true); }

// Closes the connection, optionally stopping only sending or receiving
void Socket::Close(bool stop_sends, bool stop_receives) {
  if (stop_sends && stop_receives) shutdown(this->sockfd, SHUT_RDWR);

  if (stop_sends) shutdown(this->sockfd, SHUT_WR);

  if (stop_receives) shutdown(this->sockfd, SHUT_RD);
}