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
#include <netdb.h> 
#include "json.hpp"  // for JSON manipulation

// nlohmann::json is a modern C++ library for parsing and manipulating JSON data.
using json = nlohmann::json;

// function to display error messages and terminate the program
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

// Function to process incoming JSON messages based on their type
void processMessage(json msg) {
  std::string type = msg["type"];
  if (type == "CONNECTION_ACK") {
    // Connection acknowledgment message
    std::cout << "Connection established.\n";
  } else if (type == "MESSAGE_ACK") {
    // Message acknowledgment message
    std::cout << "Your message was acknowledged.\n";
  }
}

// Main program
int main(int argc, char *argv[])
{
    // Defining necessary variables
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    // Checking for command line argument (hostname and port number)
    if (argc < 3) {
       std::cerr<<"usage "<<argv[0]<<" hostname port\n";
       exit(0);
    }
    // Convert port number from string to integer
    portno = atoi(argv[2]);
    // Creating a new socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    // Get server details
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        std::cerr<<"ERROR, no such host\n";
        exit(0);
    }
    // Clearing the server address structure
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    // Copying server details
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    // Connect to the server
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    
    // Variables to store user input
    std::string username, message;
    
    // Ask for username
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);
    
    // Main loop to keep the client running
    while (true) {
      // Ask for a message to send
      std::cout << "Enter message: ";
      std::getline(std::cin, message);
      // Construct the JSON message
      json msg = {
        {"version", 1},
        {"type", "CHAT_MESSAGE"},
        {"payload", {
              {"username", username},
              {"message", message},
              {"messageId", rand()},  // Assign a random ID to each message
              {"timestamp", ""},
              {"readReceipt", false},
              {"messageReactions", json::array()}
        }}
      };
      std::string msg_str = msg.dump();
      // Send the message
      n = write(sockfd, msg_str.c_str(), msg_str.size());
      if (n < 0) 
        error("ERROR writing to socket");
      // Clear the buffer
      bzero(buffer, 256);
      // Read the server response
      n = read(sockfd, buffer, 255);
      if (n < 0) 
        error("ERROR reading from socket");
      // Parse the response as a JSON object
      json response = json::parse(buffer);
      // Process the response
      processMessage(response);
    }
    // Close the socket
    close(sockfd);
    return 0;
}


