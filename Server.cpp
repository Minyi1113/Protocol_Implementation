// Including required libraries
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "json.hpp"  // for JSON manipulation

// nlohmann::json is a modern C++ library for parsing and manipulating JSON data.
using json = nlohmann::json;

// function to display error messages and terminate the program
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

// Function to process incoming JSON messages based on their type
void processMessage(int socket, json msg) {
  std::string type = msg["type"];
  
  if (type == "CONNECTION_REQUEST") {
    // Process connection request
    // Create JSON response with connection acknowledgment
    json response = {
      {"version", 1},
      {"type", "CONNECTION_ACK"},
      {"payload", {}}
    };
    std::string response_str = response.dump();
    // Send the response
    write(socket, response_str.c_str(), response_str.size());
  } else if (type == "CHAT_MESSAGE") {
    // Process chat message
    // Display the message on console
    std::cout << "Received message from " << msg["payload"]["username"] << ": " << msg["payload"]["message"] << "\n";
    // Create JSON response with message acknowledgment
    json response = {
      {"version", 1},
      {"type", "MESSAGE_ACK"},
      {"payload", {{"messageId", msg["payload"]["messageId"]}}}
    };
    std::string response_str = response.dump();
    // Send the response
    write(socket, response_str.c_str(), response_str.size());
  }
}

// Main program
int main(int argc, char *argv[])
{
    // Defining necessary variables
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    
    // Checking for command line argument (port number)
    if (argc < 2) {
        std::cerr<<"ERROR, no port provided\n";
        exit(1);
    }
    // Creating a new socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket");
    // Clearing the server address structure
    bzero((char *) &serv_addr, sizeof(serv_addr));
    // Convert port number from string to integer
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    // Binding the socket to a port number
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
    // Listen for connections
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    // Accepting a connection
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) 
        error("ERROR on accept");
    // Main loop to keep the server running
    while (1) {
      // Clearing the buffer
      bzero(buffer,256);
      // Reading incoming messages
      n = read(newsockfd,buffer,255);
      if (n < 0) error("ERROR reading from socket");
      // Parse the message as a JSON object
      json msg = json::parse(buffer);
      // Process the message
      processMessage(newsockfd, msg);
      // Set read receipt to true and send response
      msg["readReceipt"] = true;
      std::string response = msg.dump();
      n = write(newsockfd,response.c_str(),response.size());
      if (n < 0) error("ERROR writing to socket");
    }
    // Close the sockets
    close(newsockfd);
    close(sockfd);
    return 0; 
}


