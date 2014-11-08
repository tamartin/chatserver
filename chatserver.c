// Taylor Martin

// Starter code from http://www.linuxhowtos.org/C_C++/socket.html 
// as suggested by Alex during class

// I could not get it working 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
  int sockfd, newsockfd, portno, fdmax, newfd, nbytes, addrlen;
  socklen_t clilen;
  char buffer[256], buffer2[256];
  struct sockaddr_in serv_addr, cli_addr;
  int a, b;
  fd_set fdset, tempfdset;
  char namelist[20][30];
  
  for (a=0; a<20; a++){
    namelist[a][0] = '\0';
  }

  if (argc < 2) {
    fprintf(stderr,"ERROR, no port provided\n");
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
  if (bind(sockfd, (struct sockaddr *) &serv_addr,
	   sizeof(serv_addr)) < 0) 
    error("ERROR on binding");
  listen(sockfd,10);

  FD_ZERO(&fdset);
  FD_ZERO(&tempfdset);
  FD_SET(sockfd, &fdset);
  fdmax = sockfd;
     
  while(1){
    bzero(buffer, 256);
    bzero(buffer2, 256);
    tempfdset = fdset;

    if(select(fdmax+1, &tempfdset, NULL, NULL, NULL) < 0) error("select error");
    for(a=0; a<fdmax; a++){
      if(FD_ISSET(a, &tempfdset)){
	if(a==sockfd){
	  addrlen = sizeof(cli_addr);
	  newfd = accept(sockfd, (struct sockaddr *) &cli_addr, &addrlen);
	  if(newfd < 0) error("accept error");
	  else {
	    FD_SET(newfd, &fdset);
	    if(newfd>fdmax) fdmax = newfd;
	  }
	}
	else {
	  if(namelist[a][0] == '\0') {
	    strcpy(namelist[a], strtok(buffer, "\n"));
	    bzero(buffer, 256);
	    strcpy(buffer, namelist[a]);
	    strcat(buffer, " has joined the chatroom\n");
	    write(a, "Welcome to Taylor's Chat Server", 31);
	  }
	  else{
	    nbytes = read(a, buffer, sizeof(buffer));
	    if(nbytes < 0) error("read error");
	    else if(nbytes==0) close(a);
	  }
	  for(b=0; b<=fdmax; b++){
		if(FD_ISSET(b, &fdset)){
		  if(b!=sockfd && b!=a){
		    strcpy(buffer2, namelist[a]);
		    strcat(buffer2, ": ");
		    strcat(buffer2, buffer);
		    if(write(b, buffer2, sizeof(buffer2)) < 0) error("write error");
		  }
		}
	  }
	}  
      }	 
    }
  }
  return 0; 
}
		
