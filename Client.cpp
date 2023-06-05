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
#include "json.hpp"

using json = nlohmann::json;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void processMessage(json msg) {
  std::string type = msg["type"];
  if (type == "CONNECTION_ACK") {
    std::cout << "Connection established.\n";
  } else if (type == "MESSAGE_ACK") {
    std::cout << "Your message was acknowledged.\n";
  }
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    if (argc < 3) {
       std::cerr<<"usage "<<argv[0]<<" hostname port\n";
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        std::cerr<<"ERROR, no such host\n";
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    std::string username, message;
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);
    
    
    while (true) {
	  	std::cout << "Enter message: ";
	  	std::getline(std::cin, message);
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
	  	n = write(sockfd, msg_str.c_str(), msg_str.size());
	  	if (n < 0) 
	     	error("ERROR writing to socket");
	  	bzero(buffer, 256);
	  	n = read(sockfd, buffer, 255);
	  	if (n < 0) 
	     	error("ERROR reading from socket");
	  	json response = json::parse(buffer);
	  	processMessage(response);
    }
    close(sockfd);
    return 0;
}
