#include <arpa/inet.h>
#include <err.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BACKLOG 20
#define MSG_LEN 1024


/*                   the client /server model :
 
 	SERVER  :                           CLIENT 
 	
 	socket()			     socket()  
 	bind() 
 	listen() 
 	accept() 			     connect() 
 	recv()			             send()
 	send()  			     recv() 
 

*/


// remarque : chaque fonction dans le code est esuivalente à l'une des primitives présentées au dessus 

// function that creates the socket  <=> socket() 
int create_socket(int domain , int type , int protocol){

  int fd ;
  fd  = socket(domain , type , protocol);
  int yes = 1;
	if (-1 == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
}
	return fd ;
}

// get the address of client <=> getaddrinfo()
void get_client_addr(const char* port ,char*hostname, struct sockaddr_in* sockptr ){

struct addrinfo hints, *result, *rp;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
	if (getaddrinfo(hostname, port, &hints, &result) != 0) {
		perror("getaddrinfo()");
		exit(EXIT_FAILURE);
	}
	rp = result ;
	struct sockaddr_in* tmp = (struct sockaddr_in *)(rp->ai_addr);
	sockptr->sin_family = AF_INET ; 
	sockptr->sin_port = htons(atoi(port)) ; 
	sockptr->sin_addr = tmp->sin_addr ; 
	char *ip_str = inet_ntoa(tmp->sin_addr);
	printf("Address is %s:%hu\n", ip_str, htons(atoi(port)));
	
}

// try to connect  <=> connect 
void connecting(int fd , struct sockaddr_in addr ){
	
	if (-1 == connect(fd, (struct sockaddr*)&addr , sizeof(struct sockaddr_in))) {
		perror("Connect");
	}
	printf("connection successed\n") ; 
	
				
				
}

//  receive  the data from the client  <=> rcv()
void receive(int fd, void*ptr){
	if (-1 == recv(fd, ptr, MSG_LEN, 0) ) {
		perror("error of receiving ") ;
	}
	
}
//  send  the data from to the server <=> 
void send_msg(int fd , char*buff ){
	
	if(-1 == send(fd,buff ,strlen(buff),0)){
		printf("error of send") ;
	}
	
}

int main(int argc , char**argv){


	char buff[MSG_LEN];
	char buff_retour[MSG_LEN] ; 
	char *hostname = argv[1];
	char *port = argv[2];
	int fd_client ; 
	struct sockaddr_in client_addr;
	if (argc != 3) {
		printf("Usage: ./client hostname port_number\n");
		exit(EXIT_FAILURE);
	}
	fd_client  = create_socket(AF_INET , SOCK_STREAM ,0 ) ; // socket()
	
	get_client_addr(port,hostname,&client_addr) ;       //getaddrinfo()
	
	connecting(fd_client,client_addr) ;	           //connect ()
	
	
	
		while(0 != strncmp(buff, "/quit" , 5)) {
			// clean the message 
			memset(buff,0,MSG_LEN); 
			printf("Message:");
			// get the data from the keybord 
			fgets(buff,MSG_LEN,stdin);
		 	
		 	// send the message 
			send_msg(fd_client , buff) ; 
			
			//clean the retour message 
			memset(buff_retour,0,MSG_LEN);
			printf("message sent !\n");
			
			// rcv()
			receive(fd_client , buff_retour) ; 
		
			
			printf("received  : %s\n" , buff_retour) ; 
		
		
		
		}
		
	
	
	close(fd_client) ;
	printf("END of connection\n ") ; 
	
	return 0;
}
		
		
		
		


	
