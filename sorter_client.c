#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>


void error(char * msg) {
        perror(msg);
        exit(1);
}

int main(int argc, char * argv[]) {
        int sockfd, portno, n;
        struct sockaddr_in serv_addr;
        struct hostent * server;

        char buffer[256];

	//Check to see args numbers are correct
        if (argc < 3) {
                fprintf(stderr, "usage %s hostname port\n", argv[0]);
                return (0);
        }

	//set portno. to specified between 2000 and 65655(?)
        portno = atoi(argv[2]);

	//create a socket(int domain, int type, int protocol)
	//domain is IPv4
	//type is TCP
	//protocol is 0 for default
        sockfd = socket(AF_INET, SOCK_STREAM, 0);

	//check to see socket was correctly opened
        if (sockfd < 0) {
                error("ERROR opening socekt");
        }

	//host name: "visitor.cs.rutgers.edu" = "visitor"
        server = gethostbyname(argv[1]);

	//check if hostname exists
        if (server == NULL) {
                fprintf(stderr, "Error, no such host");
                return (0);
        }

	//clears the memory and initializes to 0s
        bzero((char * ) & serv_addr, sizeof(serv_addr));
	
	//set domain 
        serv_addr.sin_family = AF_INET;

	//copy the bytesequency from src to destination 
        bcopy((char * ) server -> h_addr, (char * ) & serv_addr.sin_addr.s_addr, server -> h_length);

	//assign portno
        serv_addr.sin_port = htons(portno);

        if (connect(sockfd, & serv_addr, sizeof(serv_addr)) < 0) {
                error("ERROR connecting");
        }
	
        printf("Please enter the message: ");
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);

	//send message to server, not using send(...) 
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) {
                error("ERROR writing to socket");
        }

	//clean the string
        bzero(buffer, 256);

	//read response of the server
        n = read(sockfd, buffer, 255);
        if (n < 0) {
                error("ERROR reading from socket");
        }
        printf("%s\n", buffer);

        return 0;
}
