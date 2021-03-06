//Necessary includes for socket programming 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

// macro definition for colors used in printing: red is for server, blue is for client, green is all other information
#define REDTEXT  "\x1B[31m"
#define BLUETEXT  "\x1B[34m"
#define GREENTEXT  "\x1B[32m"

void error(const char *message){
  //output error received
  perror(message);

  //terminate program
  exit(1);
}

//get random joke from jokes array
void getJoke(char **joke) {
     // initialize the array of the jokes
    const char *jokes[16];
	jokes[1] = "How do you throw a space party? You planet.";
	jokes[2] = "How was Rome split in two? With a pair of Ceasars.";
	jokes[3] = "A Buddhist walks up to a hot dog stand and says Make me one with everything.";
	jokes[4] = "What does a clock do when it's hungry? It goes back for seconds.";
	jokes[5] = "Towels can’t tell jokes. They have a dry sense of humor.";
	jokes[6] = "To write with a broken pencil is pointless.";
	jokes[7] = "I read a book on anti-gravity. I couldn’t put it down.";
	jokes[8] = "How does Moses make coffee? Hebrews it.";
	jokes[9] = "What did the buffalo say to his son? Bison.";
	jokes[10] = "Why shouldn’t you trust atoms? They make up everything.";
	jokes[11] = "What do you call a bear with no teeth? A gummy bear.";
	jokes[12] = "What do you call a fake noodle? An impasta.";
	jokes[13] = "What do you call a spanish pig? Porque.";
	jokes[14] = "What do you call a line of rabbits marching backwards? A receding hairline.";
	jokes[15] = "How do trees access the internet? They log on.";

     // get random number to read
     int i;
     int r = rand()%16;
     //open file to get random joke
     // if they are unlucky and get 0
     if (r==0) 
     {
	     	// give the default joke answer
     		*joke="I don't feel like telling a joke right now";
     }
     else
     {
	     	// give them a random joke
		*joke = jokes[r]; 
     }
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

     //initialize jokes
     char *joke;

     //message buffer and internet address for connection
     socklen_t clilen;
     char buffer[255];
     struct sockaddr_in serv_addr, cli_addr;

     //TCP connection
     // AF_INET is used to signify internet networking connection (hence the ip and port)
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");

     //clear all text
     bzero((char *) &serv_addr, sizeof(serv_addr));
     // set up the server properties to allow for proper networking connections
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
	
      // bind the socket and the server 
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
     // make sure the that new socket is available and properly connected to be able to pass the messages
     if (newsockfd < 0) 
          error("ERROR on accept");
     while(1)
     {
	   // clear out the buffer used for the messages
           bzero(buffer,256);
	    // read from the client into the server 
           n = read(newsockfd,buffer,255);
           if (n < 0) error("ERROR reading from socket");

	   //display client message
		// check to see if the user is not ending the client/server conversation
		if (buffer[0]=='\0'|| strncmp("Bye" , buffer , 3)==0)
		{
			// write the message to the buffer
			n = write(newsockfd,"\0",1);
			// sleep is used to allow the transmission of the messages properly
			sleep(1);
		     close(newsockfd);
		     close(sockfd);
		     return 0; 
		}
		else if (strncmp("Tell me a joke" , buffer , 3)==0)
	   	   {
			getJoke(&joke);

			// print out the message coming from the client
		   	printf("%sClient: %s%s%s",BLUETEXT, buffer, REDTEXT, joke);

			//clear all buffer
		        bzero(buffer,256);
			strcpy(buffer,joke);

			n = write(newsockfd,buffer,strlen(joke));	
		   } 
		else {
			// print out the message coming from the client
		   	printf("%sClient: %s%s",BLUETEXT, buffer, REDTEXT);

			//clear all buffer
		        bzero(buffer,256);
			// get the information from stdin and copy it to the message buffer
			fgets(buffer,255,stdin);

			 //Let server write back
			 n = write(newsockfd,buffer,strlen(buffer));
		   }

           if (n < 0) error("ERROR writing to socket");

     }
     //Close sockets
     close(newsockfd);
     close(sockfd);
     return 0; 
}

