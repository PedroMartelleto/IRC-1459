# IRC-1459

Implementation of the Internet Relay Chat Protocol from the RFC 1459 specification.

## Group

 - Pedro Martelleto Bressane Rezende (11795641)
 - João Marcos Cardoso da Silva (11795314)
 - Antônio Rodrigues Rigolino (11795791)
 
## Building

To build, run:

> make all

The client and server binaries will be created on the build folder.

To start the server, run:

> make rs

To start the client, run this after the server has already started:

> make rc

## Commands

There are three possible commands:

> /help [cmd]

If cmd is not specified, lists all commands. Otherwise, displays the requested command's usage and description.

> /send [msg]

Sends the requested message. Note that the message may contain spaces.

> /quit

Exits the program.
