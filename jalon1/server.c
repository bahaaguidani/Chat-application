#include <arpa/inet.h>
#include <err.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
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

// remarque : la partie de la structure pollfd est inspirée du code de la correction d'exercice fait en enseignement intégré .




				/*       */



// the structure of information client

struct list_address {

	int fd ;
	char addr [18 ] ;
	int port ;
	struct list_address* next ;


};


// free de la liste chainée list
void clear_list(struct  list_address **p)

{

        struct  list_address *tmp;

        while(*p)

          {

             tmp = (*p)->next;

             free(*p);

             *p = tmp;

          }

}

void add_address(struct list_address** cli , char address [] ,int port ,int fd)  {

	struct list_address* tmp = malloc(sizeof(struct list_address)) ;
	if(!tmp){
		exit(EXIT_FAILURE) ;
	}

	strcpy(tmp->addr , address) ;
	tmp->fd = fd ;
	tmp->port = port ;
	tmp->next = *cli ;
	*cli = tmp ;

}



// fonction creates the socket  <=> socket()

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

void get_server_addr(const char* port , struct sockaddr_in* sockptr ){

	struct addrinfo hints ,
	*result, *rp;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (getaddrinfo(NULL, port, &hints, &result) != 0) {
		perror("getaddrinfo()");
		exit(EXIT_FAILURE);
	}

	rp = result ;
	struct sockaddr_in* tmp = (struct sockaddr_in *)(rp->ai_addr);
	sockptr->sin_family = AF_INET ;  // IPV4
	sockptr->sin_port = htons(atoi(port)) ;  // the port number
	sockptr->sin_addr = tmp->sin_addr ;

	struct in_addr local_address = sockptr->sin_addr ;//  the local adress



	printf("Binding to %s on port %hd\n",
						 inet_ntoa(local_address),
						 ntohs(sockptr->sin_port));



}
// function <=> bind()
void socket_bind(int fd , struct sockaddr_in addr){

  if (-1 == bind(fd, (struct sockaddr*)&addr ,  sizeof(struct sockaddr_in))) {
    perror("Binding failed ");
  }

}

void listening(int fd){

  if (-1 == listen(fd, BACKLOG)) {
    perror("Listen failed ");
  }
}

// function that help server to accept the connection form the client
int socket_accept(int fd , struct sockaddr*client_addr, socklen_t size , char addr [] ,int* p ){

  int client_fd ;
  client_fd = accept(fd, client_addr, &size) ;

  if (-1 == client_fd ) {
    perror("Accept failed ");
  }


  struct sockaddr_in *sockptr = (struct sockaddr_in *)(&client_addr);
  struct in_addr client_address = sockptr->sin_addr;

  strcpy(addr ,inet_ntoa(client_address)) ;
  *p =  ntohs(sockptr->sin_port );


	return client_fd ;
}
// receive the data from the client
void receive(int fd, void*ptr){
	if (-1 == recv(fd, ptr, MSG_LEN, 0) ) {
		perror("error of receiving ") ;
	}
}

//  send  the data from the client
void send_msg(int fd , char*buff ){

	if(-1 == send(fd,buff ,strlen(buff),0)){
		printf("error of sending ") ;
	}
}


int main(int argc , char**argv){




  struct list_address* list = malloc(sizeof(struct list_address)) ;
	char buff[MSG_LEN];
	const char* port = argv[1];
	int fd_server ;

	struct sockaddr_in server_addr ; // structure address of the server

	if (argc != 2) {
		printf("Usage: ./server port_number\n");
		exit(EXIT_FAILURE);
	}



	fd_server = create_socket(AF_INET , SOCK_STREAM ,0 ) ; // create the socket with ipv4 protocol and tcp protocol on transport layer

	 get_server_addr(port,&server_addr) ;// get the server address

	socket_bind( fd_server, server_addr) ;  // binding

	listening(fd_server ) ; // listenning




	int nfds = 10;
	struct pollfd pollfds[nfds];

	// Init first slot with listening socket
	pollfds[0].fd = fd_server ;
	pollfds[0].events = POLLIN;
	pollfds[0].revents = 0;
	// Init remaining slot to default values
	for (int i = 1; i < nfds; i++) {
		pollfds[i].fd = -1;
		pollfds[i].events = 0;
		pollfds[i].revents = 0;
	}

	// server loop

	while (1) {
		// Block until new activity detected on existing socket
		int n_active = 0;
		if (-1 == (n_active = poll(pollfds, nfds, -1))) {
			perror("Poll");
		}
		//printf("[SERVER] : %d active socket\n", n_active);

		// Iterate on the array of monitored struct pollfd
		for (int i = 0; i < nfds; i++) {

			// If listening socket is active => accept new incoming connection
			if (pollfds[i].fd == fd_server && pollfds[i].revents & POLLIN) {
				// accept new connection and retrieve new socket file descriptor

        int fd_client ;
      	char ad [15] ;
      	int port_client  = 0 ;
				struct sockaddr_in client_addr;
				socklen_t size = sizeof(client_addr);
				fd_client = socket_accept(fd_server , (struct sockaddr 	*)&client_addr ,size,ad,&port_client) ;


        	add_address(&list , ad ,port_client , fd_client ) ;


				// store new file descriptor in available slot in the array of struct pollfd set .events to POLLIN
				for (int j = 0; j < nfds; j++) {
					if (pollfds[j].fd == -1) {
						pollfds[j].fd = fd_client ;
						pollfds[j].events = POLLIN;
						break;


					}
				}

				// Set .revents of listening socket back to default
				pollfds[i].revents = 0;
				


			} else if (pollfds[i].fd != fd_server && pollfds[i].revents & POLLHUP) { // If a socket previously created to communicate with a client detects a disconnection from the client side
				// display message on terminal
				printf("client on socket %d has disconnected from server\n", pollfds[i].fd);
				// Close socket and set struct pollfd back to default
				close(pollfds[i].fd);
				pollfds[i].events = 0;
				pollfds[i].revents = 0;
			} else if (pollfds[i].fd != fd_server && pollfds[i].revents & POLLIN) { // If a socket different from the listening socket is active


					// clean the message
					memset(buff,0,MSG_LEN);

					// rcv()
					receive(pollfds[i].fd , buff) ;
					if( strncmp(buff, "/quit" , 5) == 0 ){
					// close the connection

					close(pollfds[i].fd) ;
					pollfds[i].fd = -1  ;
					pollfds[i].events = -1 ;




				}

					printf("received  : %s\n" ,buff ) ;
					//send()
					send_msg(pollfds[i].fd, buff) ;

					printf("message sent !\n");
					pollfds[i].revents = 0;


			}
		}
	}



	close(fd_server) ;


	return 0 ;
}
