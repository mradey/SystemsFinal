//Necessary includes for socket programming 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#define SERV_TCP_PORT 8000 /* server's port number */
#define MAX_SIZE 80

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
  int sockfd, newsock_fd;

  //port number
  int port_num;  

  //variable to check if function succesfful
  int n;

  //message buffer to store messages 
  char buffer[1023];

  //internet address for connection
  struct sockaddr_in server_address, client_address;
  socklen_t client;

  //TCP connection
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd<0){
    error("Error opening the socket");
  }
  //clear all text
  bzero((char *) &server_address, sizeof(server_address));

  port_num = atoi(argv[1]);

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(port_num);

  if(bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address))<0){
    error("Binding failed");
  }

  //listen for messages from max 10 clients 
  listen(sockfd, 10);
  client = sizeof(client_address);

  //Connection accepted and new file descriptor
  newsock_fd = accept(sockfd, (struct sockaddr *) &client_address, &client);

  if(newsock_fd < 0){
    error("Did not accept");
  }

  while(1){
    bzero(buffer, 1024);
    n = read(newsock_fd, buffer, 1023);
    if(n <0){
      error("Error reading");
    }

    //display client message
    printf("Client : %s\n", buffer);

    //clear buffer
    bzero(buffer, 1023);
    fgets(buffer, 1023, stdin);

    //let server write back
    n = write(newsock_fd, buffer, strlen(buffer));

    if(n < 0){
      error("Error writing"):
    }

    //when done wrote end connecction
    int i = strncmp("done", buffer, 4);
    if(i ==0){
      break;
    }
  }
  close(newsock_fd);
  close(sockfd);
  return 0;
}

