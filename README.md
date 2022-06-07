# IRC-1459

Client-server chat based on the IRC-1459 specification.

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

> HELP [cmd]

If cmd is not specified, lists all commands. Otherwise, displays the requested command's usage and description.

> SEND [msg]

Sends the requested message. Note that the message may contain spaces.

> QUIT

Exits the program.
