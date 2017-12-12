/* A simple server in the internet domain using TCP the port number is passed as an argument */

#include"sorter_server.h"

void error(char *msg){
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[]){
	int sockfd, newsockfd, portno, clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	//check for proper args
	if (argc < 3){
		fprintf(stderr,"ERROR, no -p parameter or port provided\n");
		return(1);
	}
	if(strcmp(argv[1], "-p") != 0){
		fprintf(stderr,"ERROR, no -p parameter given\n");
		return(1);
	}

	//create a socket(int domain, int type, int protocol)
	//domain is IPv4
	//type is TCP
	//protocol is 0 for default
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	//check to see socket correctly opened
	if (sockfd < 0){
		error("ERROR opening socket");
	}

	//clear the buffer in memory to 0s
	bzero((char *) &serv_addr, sizeof(serv_addr));

	//set portnumber to user given. Server and client portno must match
	portno = atoi(argv[2]);

	/*setup the host_addr structure for bind call*/
	//server byte order, IPv4
	serv_addr.sin_family = AF_INET;
	//filled with host's IP address
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	//convert integer value to network order	
	serv_addr.sin_port = htons(portno);

	// bind(int fd, struct sockaddr *local_addr, socklen_t addr_
	// bind() passes file descriptor, the address 
	// and the length of the address 
	// This bind() call will bind  the socket to the current IP address on port, portno
     
	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
		error("ERROR on binding");
	}

	// This listen() call tells the socket to listen to the incoming connections.
     	// The listen() function places all incoming connection into a backlog queue
     	// until accept() call accepts the connection.
     	// Here, we set the maximum size for the backlog queue to 32.
	// Backlog queue shouldn't grow to a large size.
	listen(sockfd,32);
	clilen = sizeof(cli_addr);

	// This accept() function will write the connecting client's address info 
    	// into the the address structure and the size of that structure is clilen.
   	// The accept() returns a new socket file descriptor for the accepted connection.
     	// So, the original socket file descriptor can continue to be used 
     	// for accepting new connections while the new socker file descriptor is used for
     	// communicating with the connected client.
	// TODO: Do threading here.
	// If multiple client wants to connect, the server replaces old client with new client and sends back an error
	newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);

	if (newsockfd < 0){
		error("ERROR on accept");
	}

	//clear buffer
	bzero(buffer,256);

	//read the message inside the socket sent from client
	n = read(newsockfd,buffer,255);

	if (n < 0){
		error("ERROR reading from socket");
	}

	printf("Here is the message: %s\n",buffer);

	//send back the message to client
	n = write(newsockfd,"I got your message",18);

	if (n < 0){
		error("ERROR writing to socket");
	}

	return 0;
}
