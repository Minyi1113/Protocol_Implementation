#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "json.hpp"

using json = nlohmann::json;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void processMessage(int socket, json msg) {
  std::string type = msg["type"];
  if (type == "CONNECTION_REQUEST") {
    // Process connection request
    json response = {
      {"version", 1},
      {"type", "CONNECTION_ACK"},
      {"payload", {}}
    };
    std::string response_str = response.dump();
    write(socket, response_str.c_str(), response_str.size());
  } else if (type == "CHAT_MESSAGE") {
    // Process chat message
    std::cout << "Received message from " << msg["payload"]["username"] << ": " << msg["payload"]["message"] << "\n";
    json response = {
      {"version", 1},
      {"type", "MESSAGE_ACK"},
      {"payload", {{"messageId", msg["payload"]["messageId"]}}}
    };
    std::string response_str = response.dump();
    write(socket, response_str.c_str(), response_str.size());
  }
}


int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2) {
        std::cerr<<"ERROR, no port provided\n";
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) 
        error("ERROR on accept");
    while (1) {
      bzero(buffer,256);
      n = read(newsockfd,buffer,255);
      if (n < 0) error("ERROR reading from socket");
      json msg = json::parse(buffer);
      processMessage(newsockfd, msg);
      std::cout << "Received message from " << msg["username"] << ": " << msg["message"] << "\n";
      msg["readReceipt"] = true;
      std::string response = msg.dump();
      n = write(newsockfd,response.c_str(),response.size());
      if (n < 0) error("ERROR writing to socket");
    }
    close(newsockfd);
    close(sockfd);
    return 0; 
}

