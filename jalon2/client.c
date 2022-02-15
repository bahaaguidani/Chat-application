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
#include "common.h"
#include "msg_struct.h"

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
	//printf("Address is %s:%hu\n", ip_str, htons(atoi(port)));

}

// try to connect  <=> connect
void connecting(int fd , struct sockaddr_in addr ){

	if (-1 == connect(fd, (struct sockaddr*)&addr , sizeof(struct sockaddr_in))) {
		perror("Connect");
	}
	printf("connection to server ...  done \n") ;
	printf("[Server] : please login with /nick <your pseudo> \n") ;




}

//  receive  the data from the client  <=> rcv()
void receive(int fd, void*ptr, int size){
	if (-1 == recv(fd, ptr, size, 0) ) {
		perror("error of receiving ") ;
    exit(EXIT_FAILURE) ;
	}

}
//  send  the data from to the server <=>
void send_msg(int fd , void*ptr , int size){

	if(-1 == send(fd,ptr ,size,0)){
		printf("error of send") ;
	}

}

int main(int argc , char**argv){


  char buff[MSG_LEN];
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




    	struct message msgstruct;
    	char buff_retour[MSG_LEN] ;

      //printf("Message:");

//for(;;){
//  int nfds = 10;
  struct pollfd pollfds[2];

  pollfds[0].fd = fd_client ;
	pollfds[0].events = POLLIN;
	pollfds[0].revents = 0;

  //memset(pollfds , -1 ,nfds) ;

  pollfds[1].fd = STDIN_FILENO;
  pollfds[1].events = POLLIN;
  pollfds[1].revents = 0;





while(1){
  int n_active = 0;
  if (-1 == (n_active = poll(pollfds, 2, -1))) {
    perror("Poll");
  }

  memset(buff,0,MSG_LEN);
  memset(&msgstruct, 0, sizeof(struct message));

  if(pollfds[1].revents & POLLIN){






    fgets(buff,MSG_LEN,stdin);
    //read(STDIN_FILENO ,buff , MSG_LEN);

    // Filling structure
    msgstruct.pld_len = strlen(buff);
    strncpy(msgstruct.nick_sender, "", 0);
    msgstruct.type = ECHO_SEND;
    strncpy(msgstruct.infos, "\0", 1);



    send_msg(fd_client,&msgstruct, sizeof(struct message)) ;
    send_msg(fd_client, buff , msgstruct.pld_len) ;
    memset(buff_retour,0,MSG_LEN);
    //printf("message sent ") ;

    //if( strncmp(buff, "/nick" , 5) == 0  ||(strncmp(buff, "/who" , 4) == 0 &&  strncmp(buff, "/whois" , 6) != 0  ) ) {

pollfds[1].revents = 0 ;

}
    //printf("Message: ") ;
    // get the data from the keybord
//memset(buff_retour,0,MSG_LEN);


  //printf("pld_len: %i / nick_sender: %s / type: %s / infos: %s\n", msgstruct.pld_len, msgstruct.nick_sender, msg_type_str[msgstruct.type], msgstruct.infos);
else if(  pollfds[0].revents & POLLIN  ){


  memset(buff_retour,0,MSG_LEN);



  receive(fd_client,&msgstruct, sizeof(struct message)) ;
  receive(fd_client ,buff_retour ,msgstruct.pld_len) ;


  printf( " received : %s %s\n" , buff_retour ,msgstruct.infos) ;

  if(msgstruct.type == MULTICAST_CREATE ){
    printf("you have joined channel %s\n",msgstruct.infos );
  }
//printf( " received : %s \n" , buff_retour ) ;

pollfds[0].revents = 0 ;
      //printf("pld_len: %i / nick_sender: %s / type: %s / infos: %s\n", msgstruct.pld_len, msgstruct.nick_sender, msg_type_str[msgstruct.type], msgstruct.infos);


  }
  //memset(buff_retour,0,MSG_LEN);

}


	close(fd_client) ;
	printf("END of connection\n ") ;


	return 0 ;

}
