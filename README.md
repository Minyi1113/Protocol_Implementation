# Protocol_Implementation
Network Protocol Implementation

This project demonstrates a simple client-server communication protocol. This protocol facilitates the sending and receiving of messages between a client and a server in a stateful manner.

## Files
The project contains the following files:
```
Server.cpp: This file contains the implementation for the server.
```
Client.cpp: This file contains the implementation for the client.

## Getting Started
The program consists of two parts: the server and the client. The server listens for connections on a specified port, and the client can connect to the server by specifying its hostname and the port number.

## Prerequisites
To build and run this program, you will need:
A C++ compiler supporting C++14 or later.
The nlohmann/json library for parsing and generating JSON data.
A Linux-based system, such as Tux, MacOS, or the Windows Linux subsystem.


## Compilation and Execution
The client and server programs can be compiled and run using the following commands:

To compile Server.cpp:
g++ -std=c++11 -o server Server.cpp 
To run the server:
./server [port number]

To compile Client.cpp:
g++ -std=c++11 -o client Client.cpp 
To run the client:
./client [hostname] [port number]

Remember to replace [port number] with the port number you want your server to listen on, and [hostname] with the server's hostname or IP address. The port number provided in the client command should match the server's port number.

## Program Flow
When the server starts, it binds itself to a specific port and starts listening for incoming connections.
When the client starts, it connects to the server using its hostname and port number.
The client sends messages to the server in a predefined JSON format, which contains the message type and its payload.
The server processes the incoming messages according to their types. It responds with the appropriate message, which is then received and processed by the client.


## Notes
The protocol is stateful. Both the client and server implement and check the statefulness of the protocol.
The server binds to a hard-coded port number, and the client defaults to this port number.
The client can specify the hostname or IP address of the server, and the server can accept command line parameters or configuration files for configuration.
User interaction with the client is done through a command line interface. The user does not need to know the protocol commands for the protocol to work.

## Conclusion
This simple client-server protocol implementation provides a strong foundation for the development of more complex protocols. It demonstrates the principles of socket programming, stateful communication, and the use of JSON for message passing between a client and server.
