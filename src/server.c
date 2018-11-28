//Necessary includes for socket programming 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *message){
  //output error received
  perror(message);

  //terminate program
  exit(1);
}

//2 parameters: filename and port number
int main(int argc, char *argv[])
{
    //Check if user has provided port number
     if(argc < 2){
      fprintf(stderr, "Need port number");
      exit(1);
     }
     //file descriptors
     int sockfd, newsockfd;
     
     //port number
     int portno;
     
     //check if function successful
     int n;

     //message buffer and internet address for connection
     socklen_t clilen;
     char buffer[255];
     struct sockaddr_in serv_addr, cli_addr;

     //TCP connection
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");

     //clear all text
     bzero((char *) &serv_addr, sizeof(serv_addr));
     
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     
     //listen for messages from max 5 clients
     listen(sockfd,5);
     clilen = sizeof(cli_addr);

     //Connection accepted and new file descriptor
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);

     if (newsockfd < 0) 
          error("ERROR on accept");
     while(1)
     {
           bzero(buffer,256);
           n = read(newsockfd,buffer,255);
           if (n < 0) error("ERROR reading from socket");

	   //display client message
           printf("Client: %s\n",buffer);

	   //clear all buffer
           bzero(buffer,256);
           fgets(buffer,255,stdin);

	   //Let server write back
           n = write(newsockfd,buffer,strlen(buffer));

           if (n < 0) error("ERROR writing to socket");

	   //write Bye to end connection
           int i=strncmp("Bye" , buffer, 3);
           if(i == 0)
               break;
     }
     //Close sockets
     close(newsockfd);
     close(sockfd);
     return 0; 
}
