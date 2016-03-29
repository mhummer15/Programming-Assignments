#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#define HTTPPORT 80

char *findIP(char *host);

int main( int argc, char *argv[] ) {
   int sockfd, newsockfd, portno, clilen, remSock, htmlStart, i = 0;
   char buffer[8192], *tok_str, *ip, *host, *content, *request = "GET / HTTP/1.1\r\n\r\n";
   char buf[BUFSIZ +1];
   struct sockaddr_in serv_addr, cli_addr, rem_addr;
   int  n, m;
   FILE *fp;
   
   /* Creating a socket */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      printf("ERROR while opening the socket");
      exit(1);
   }
   
   /* Initializing the socket structure */

   portno = 45455;
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   
   /* Binding the socket to the port*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      printf("ERROR while binding");
      exit(1);
   }
      
   /* Begin listening for incoming connections
   */
   
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   
   /* Accept connection request from the client */
   newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
	
   if (newsockfd < 0) {
      printf("ERROR on accept");
      exit(1);
   }
   
   /* Begin communication */
   bzero(buffer,8192);
   n = read( newsockfd,buffer,8192 );
   if (n < 0) {
      printf("ERROR reading from socket");
      exit(1);
   }
   tok_str = strtok(buffer, " /"); //buffer is the message read from the client
   while ((tok_str != NULL)&& (i < 2))// ' ' and '/' are delimiters
   {
	   //printf("%s\n", tok_str);
	   if (i == 1)
		host = tok_str;			//save the 2nd token as the webpage
	   tok_str = strtok(NULL, " /");
	   i++;
   }
   //printf("REQUESTED PAGE: %s\n", host);
   fp = fopen (host, "r");
   if (fp == NULL)	//if file is not on server
	{
	   fp = fopen(host, "w");	//open file for writing
	   printf("%s", request);	//Print get request
	   ip = findIP(host);		//find the ip of the host the client requested
	   printf("%s", ip);
	   remSock = socket(AF_INET, SOCK_STREAM, 0);	//create a socket to connect to the remote server
	   if (remSock < 0)
	   {
		   printf("ERROR creating remote socket");
		   exit(1);
	   }
	   rem_addr.sin_family = AF_INET;
	   rem_addr.sin_addr.s_addr = inet_addr(ip);
	   rem_addr.sin_port = htons(HTTPPORT);  //connect on port 80 for http request
	   if (connect(remSock, (struct sockaddr*)&rem_addr, sizeof(rem_addr)) < 0)
	   {
		   printf("ERROR attempting to connect to remote server");
		   exit(1);
	   }
	   n = write(remSock, request, strlen(request)); //send get request to remote server
	   if (n < 0)
	   {
		   printf("ERROR sending http request");
		   exit(1);
	   }
	   
	   memset(buf, 0, sizeof(buf));
	   htmlStart = 1;
	   while ((n = recv(remSock, buf, BUFSIZ, 0)) > 0)	//receive response from server
	   {
		  content = buf; //store response in content variable
		  if (htmlStart) 
		  {
			  //printf("%s", content);
			  m = write(newsockfd, content, strlen(content)); //write content to client
			  fputs(content, fp);	//write content to file for later access
			  fflush(fp);
		  }
		  memset (buf, 0, n);
	  }
	   if (n < 0)
	   {
		   printf("ERROR reading response");
		   exit(1);
	   }
	   close(remSock);
	   close(newsockfd);
	   close(sockfd);
   }
   else
   {
	   content = (char*) malloc(BUFSIZ);	//allocate buffer to read from file
	   while((fgets(content, BUFSIZ, fp)) != NULL)
	   {
			//printf("%s", content);
			n = write(newsockfd, content, strlen(content));
		}
		free(content);
	}
   
   if (n < 0) {
      printf("ERROR writing to socket");
      exit(1);
   }
   close(sockfd);
   close(newsockfd);
   fflush(fp);
   fflush(stdout);
      
   return 0;
}


//function to find the ip of a given host
char *findIP(char *host)
{
	struct hostent *hent;
	int iplen = 15;
	char *ip = (char *) malloc(iplen + 1);
	memset(ip, 0, iplen +1);
	if ((hent = gethostbyname(host)) == NULL)
	{
		herror("Can't get host IP");
		exit(1);
	}
	if (inet_ntop(AF_INET, (void *) hent->h_addr_list[0], ip, iplen) == NULL)
	{
		perror("Unable to resolve host");
		exit(1);
	}
	return ip;
};
