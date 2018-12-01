#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

// macro definition for colors used in printing: red is for server, blue is for client, green is all other information
#define REDTEXT  "\x1B[31m"
#define BLUETEXT  "\x1B[34m"
#define GREENTEXT  "\x1B[32m"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    // make sure that the correct arguments are passed in for the client
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    // create the socket: AF_INET is an internet networking setup
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    // attaches the passed in parameter of the ip address to the server to connect the client and server
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    // clears out the memory in the server_address to allow for configuration of the server
    bzero((char *) &serv_addr, sizeof(serv_addr));
    // configure the server address settings 
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    // connect to the properly set up server from the client
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    // print out the pid and directory for the proc file to the user for use in the kernel module
    printf("The Client PID is %d\n", getpid());
    printf("The Client proc directory is /proc/%d/fd/0\n", getpid());
    // begin showing the client text in blue
    printf("%sClient: ", BLUETEXT);
    while(1)
    {
        bzero(buffer,256);
        fgets(buffer,255,stdin);
	// write from the stdin buffer from the user to the socket for the server to receive it
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) 
             error("ERROR writing to socket");
        bzero(buffer,256);
	// read the message comming back from the server on the socket into the buffer 
        n = read(sockfd,buffer,255);
        if (n < 0) 
             error("ERROR reading from socket");
	// check to see if the user is ending the client/server conversation
	if (buffer[0]=='\0'|| strncmp("Bye" , buffer , 3)==0)
	{
		break;
	}
	else
	{
		// print out the message coming from the server
        	printf("%sServer: %s%s", REDTEXT, buffer, BLUETEXT);
	}
    }
    // make sure to shut the socket upon request to ensure no networking errors occur
    close(sockfd);
    return 0;
}
