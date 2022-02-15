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
#include <time.h>
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

// remarque : la partie de la structure pollfd est inspirée du code de la correction d'exercice fait en enseignement intégré .


struct salon
{
	char nom_salon[NICK_LEN] ;
	int n_users ;
	struct salon*next ;
};




//  la structure d'informations des utilisateur (descripteur de fichier et nickname )
struct user{

	char pseudo [INFOS_LEN] ;
	int fd ;

	struct user* next ;



};

// structure des addresses des utilisateurs

struct list_address {

	int fd ;
	char addr [18 ] ;
	int port ;
  char time [18] ;
	struct list_address* next ;


};



// fonction qui parcours la structure d information de type user ( le parcour de la liste chainée ) pour trouver l'utilisateur à travers son nickname
int  parcour(struct user* cli , char str [] , int size ) {


	while(cli){

		if(strncmp( str,cli->pseudo , size )   ==  0  ){
			return 1 ;
		}
		cli = cli->next ;

	}
	return 0 ;
}





// free de la liste chainée user
void clear_user(struct user **p)

{

        struct user *tmp;

        while(*p)

          {

             tmp = (*p)->next;

             free(*p);

             *p = tmp;

          }

}

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










// fonction qui ajoute un  utilisateur a traves ses informations dans la chaine user

void add_pseudo(struct user** cli ,char pseudo[INFOS_LEN] , int fd  )  {

	struct user* tmp = malloc(sizeof(struct user)) ;
	if(!tmp){
		exit(EXIT_FAILURE) ;
	}
	strcpy(tmp->pseudo , pseudo )  ;

	tmp->fd = fd ;

	tmp->next = *cli ;
	*cli = tmp ;
	//free(tmp) ;
}




void delete_pseudo_from_users(struct user** head_ref, int fd)

{

// Store head node

struct user *temp = *head_ref, *prev;



// If head node itself holds the key to be deleted

if (temp != NULL && temp->fd == fd) {

*head_ref = temp->next; // Changed head

free(temp); // free old head

return;

}



// Search for the key to be deleted, keep track of the

// previous node as we need to change 'prev->next'

while (temp != NULL && temp->fd!= fd) {

prev = temp;

temp = temp->next;

}



// If key was not present in linked list

if (temp == NULL)

return;



}





void add_time(char* date_connection) {

    //memset(date_connection,0,strlen(date_connection));
    char year[5] ;
    char mounth [3] ;
    char day [3] ;


    char hour [3];
    char minutes [3] ;


    char date [11];
     char timing [6] ;
    time_t t = time(NULL);
    struct tm* time_tmp = malloc(sizeof(struct tm));

    time_tmp = gmtime(&t) ;


/*      part of the dating */
  sprintf(year,"%d" ,time_tmp->tm_year + 1900 ) ;
  sprintf(mounth,"%d" ,time_tmp->tm_mon + 1 ) ;
  sprintf(day,"%d" ,time_tmp->tm_mday ) ;

  strcat(date,year) ;
  strcat(date,"/") ;
  strcat(date,mounth) ;
  strcat(date,"/") ;
  strcat(date,day) ;


/* part of the timing                */
  sprintf(hour,"%d" ,time_tmp->tm_hour ) ;
  sprintf(minutes,"%d" ,time_tmp->tm_min ) ;

  strcat(timing , hour);
  strcat(timing, ":") ;
  strcat(timing , minutes);


/*  assemblage   */

  strcat(date_connection,date) ;
  strcat(date_connection,"@") ;
  strcat(date_connection,timing) ;

	//free(time_tmp) ;
}



void create_msgall(struct user* cli ,  char msgall [] ,char tmp[]) {

	struct user* target = cli ;
    strcat(msgall, "[");
    strcat(msgall, target->pseudo);
    strcat(msgall , "]:") ;
    strcat(msgall,tmp) ;

}

// fonction qui ajoute un  l'address d un user et son fd


void add_address(struct list_address** cli , char address [] ,int port ,int fd, char*date_connection)  {

	struct list_address* tmp = malloc(sizeof(struct list_address)) ;
	if(!tmp){
		exit(EXIT_FAILURE) ;
	}
  add_time(date_connection) ;
	strcpy(tmp->addr , address) ;
	tmp->fd = fd ;
	tmp->port = port ;

  strcpy(tmp->time , date_connection );
	tmp->next = *cli ;
	*cli = tmp ;
	//free(tmp);

}




void display (struct user *p_head)

{

   struct user *p = p_head;



   while (p != NULL)

   {

      /* afficher les données courantes */

      printf ("%s > ", p->pseudo);



      /* pointer sur le suivant */

      p = p->next;

   }

   /* afficher la fin */

   printf ("NIL\n");

 }



// fonction qui compte le nombre d element de la structure user
int len(struct user* cli ) {

	int n = 0 ;
	while(cli){

		n++ ;
		cli = cli->next ;

	}
	return n ;

}


//  cette fonction copie une partie  la chaine de caractere src dans dest à partir d un offset
void   cpy( char*dest , const char *src ,int offset)
           {
               int  i; int j ;
               //&& src[i] != '\0';
	//memset(dest,0,strlen(dest)) ;
               for (i = offset+1  , j=0  ; i < strlen(src)  ; i++ , j++){
                   dest[j] = src[i];

	}
           }



// fonction qui trouve  la structure d un utilisateur à partir de son pseudo

struct user* get_user_from_server ( struct user* cli , char str[] ){


	struct user* target = cli ;
	while(target != NULL ) {
		if(strncmp(str,target->pseudo  , strlen(str )) == 0 ){
			return target ;
		}

		target = target->next ;

	}
	return  target;

}



struct user* get_user_from_server_with_fd( struct user* cli , int fd ){


	struct user* target = cli ;
	while(target->next != NULL ) {
		if( fd == target->fd  ){
			return target ;
		}

		target = target->next ;

	}
	return  target;

}



// fonction qui trouve la structure list_adress d un user à travers son fd

struct list_address* get_user_from_list ( struct list_address* cli , int fd ){


	struct list_address* target = cli ;
	while(target != NULL ) {
		if(fd == target->fd ){
			return target ;
		}

		target = target->next ;

	}
	return  target;

}

// fonction qui affiche les nickname de touts les elements de la liste chainee

void display_users(struct user* cli ,char str[]) {
	  struct user *p = cli ;

	while(p->next != NULL ) {
		if(strcmp(p->pseudo , "") == 0){
			strcat(str , "" ) ;
		}

		strcat(str , "-" ) ;
		strcat(str , p->pseudo) ;
		strcat(str , "\n") ;

		p = p->next ;

	}
	strcat(str ,"") ;

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




  // display client connection information
  struct sockaddr_in *sockptr = (struct sockaddr_in *)(&client_addr);
  struct in_addr client_address = sockptr->sin_addr;


  strcpy(addr ,inet_ntoa(client_address)) ;
  *p =  ntohs(sockptr->sin_port );

  printf("Connection succeeded and client used %s:%hu \n", inet_ntoa(client_address), ntohs(sockptr->sin_port));

  //printf("client_fd = %d\n", client_fd);0

	return client_fd ;
}

// receive the data from the client
void receive(int fd, void*ptr,int size){
	if (-1 == recv(fd, ptr, size, 0) ) {
		perror("error of receiving ") ;
	}
}

//  send  the data from the client
void send_msg(int fd , void*ptr ,int size){

	if(-1 == send(fd,ptr ,size,0)){
		printf("error of sending ") ;
	}
}



void server( int fd_server , const char* port) {



	struct user *cli = malloc(sizeof(struct user)) ;
	struct list_address* list = malloc(sizeof(struct list_address)) ;

	struct message msgstruct;

	char buff[MSG_LEN];
	char buff_retour[MSG_LEN] ;





	int nfds = 10;
	struct pollfd pollfds[nfds];
	int n = 0 ;



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
				n++ ;

        char date_connection [16];
        memset(date_connection,0,16);
				add_address(&list , ad ,port_client , fd_client , date_connection ) ;


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
				//free(info) ;


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
					memset(&msgstruct, 0, sizeof(struct message));

					receive(pollfds[i].fd ,&msgstruct, sizeof(struct message)) ;
					receive(pollfds[i].fd , buff , msgstruct.pld_len) ;
					printf("le pseudo a partir de la structure est : %s\n" , msgstruct.infos) ;

          printf("received  : %s\n" ,buff ) ;
          //int k = 0  ;



					if( strncmp(buff, "/nick" , 5) == 0 )
					{
					char tmp [INFOS_LEN] ;
					memset(tmp ,0,INFOS_LEN) ;
					char*msg = " [Server] : Welcome on the chat" ;
					char*msg2 = "you have changed your pseudo to : " ;
					char*error1 = "error ! this pseudo is already taken " ;
					cpy(tmp ,buff ,5 ) ;

					struct user* target = NULL ;
					target = get_user_from_server_with_fd(cli , pollfds[i].fd) ;



					// le cas d'erreur
					if(parcour(cli , tmp , INFOS_LEN ) == 1  )    {
					memset(buff_retour,0,MSG_LEN);
					msgstruct.pld_len = strlen(error1)+1 ;

					strcpy(buff_retour ,error1) ;
					}


				else {

					memset(buff_retour,0,MSG_LEN);


					msgstruct.type = NICKNAME_NEW;

					if(strcmp(target->pseudo , "" ) != 0 ) {

						strcpy(msgstruct.nick_sender , target->pseudo ) ;
						delete_pseudo_from_users(&cli , pollfds[i].fd) ;
						strcpy(buff_retour , msg2) ;


					}
					else{


							strcpy(buff_retour , msg) ;



				}
			}
 				strcpy(msgstruct.infos,tmp);
				msgstruct.pld_len = strlen(buff_retour )+1 ;

        send_msg(pollfds[i].fd ,&msgstruct, sizeof(struct message)) ;

        send_msg(pollfds[i].fd  , buff_retour , msgstruct.pld_len) ;


						add_pseudo(&cli , msgstruct.infos,pollfds[i].fd  ) ;

        pollfds[i].revents = 0;

			}
				memset(buff_retour,0, INFOS_LEN);




			if( strncmp(buff, "/who" , 4) == 0 &&  strncmp(buff, "/whois" , 6) != 0   ) {
			char tmp [MSG_LEN ] ;
			memset(tmp,0,MSG_LEN);
			char* online = "[SERVER] : online users are : \n"  ;
			memset(buff_retour,0,MSG_LEN);

			display_users(cli , tmp) ;


			//strcpy(buff_retour,online  ) ;
			strcat(buff_retour,online  ) ;
			strcat( buff_retour,tmp ) ;


			msgstruct.type = NICKNAME_LIST;
			msgstruct.pld_len = 	strlen(buff_retour);

      send_msg(pollfds[i].fd ,&msgstruct, sizeof(struct message)) ;

      send_msg(pollfds[i].fd  , buff_retour , msgstruct.pld_len) ;

      pollfds[i].revents = 0;

	}


	memset(buff_retour,0,MSG_LEN) ;

					if( strncmp(buff, "/whois" , 6) == 0   )
					{
					memset(buff_retour,0, MSG_LEN);
					struct user* user_target = NULL ;
					struct list_address* same_target = NULL  ;

					char tmp [INFOS_LEN] ;
					char p [6] ;
					memset(tmp ,0,INFOS_LEN) ;


					 cpy(tmp ,buff ,6 ) ;

					 // le cas d'erreur
					  if( parcour(cli , tmp , INFOS_LEN ) != 1      )  {
							memset(buff_retour,0, INFOS_LEN);
						char*error = "error ! we don't have any user with this pseudo " ;


					msgstruct.pld_len = 	strlen(error) +1   ;
						strncpy(buff_retour , error , strlen(error) ) ;

					}

				else {

					user_target =  get_user_from_server(cli , tmp) ; // utilisateur souhaitée


					same_target = get_user_from_list (list ,user_target->fd ) ; // la structure d address de l utilisateur souhaitée


			msgstruct.type = NICKNAME_INFOS ;
			msgstruct.pld_len = 	INFOS_LEN ;


			strcat(buff_retour ,user_target->pseudo ) ;
			strcat(buff_retour, "connected since " ) ;
      strcat(buff_retour, same_target->time ) ;
			strcat(buff_retour, " with IP address " ) ;
			strcat(buff_retour, same_target->addr ) ;

			sprintf(p ,"%d" ,same_target->port ) ;
			strcat(buff_retour , " and port number " ) ;
			strcat(buff_retour, p  );

			strcat(buff_retour , "\n") ;
					}

          send_msg(pollfds[i].fd ,&msgstruct, sizeof(struct message)) ;

          send_msg(pollfds[i].fd  , buff_retour , msgstruct.pld_len) ;

          //pollfds[i].revents = 0;

				}

        if( strncmp(buff, "/msgall" , 7) == 0 )
        {

					//client_fd = pollfds[i].fd ;
						memset(buff_retour,0, MSG_LEN);
          	printf("nb de client est %d \n" , n ) ;
          char tmp [MSG_LEN] ;
						memset(tmp ,0,MSG_LEN) ;


        struct user* targe = NULL ;

          char msgall[MSG_LEN] ;
						memset(msgall ,0,MSG_LEN )  ;


          targe = get_user_from_server_with_fd(cli ,pollfds[i].fd)  ;

          printf("le target pseudo est %s\n" , targe->pseudo);
          strcat(msgall, "[");
					strncat(msgall, targe->pseudo, strlen(targe->pseudo) - strlen("\n"));
					//strcat(msgall, targe->pseudo ) ;
					strcat(msgall , "]:") ;


					strcpy(msgstruct.infos , "");

					cpy(tmp ,buff ,7) ;

					strcat(msgall,tmp) ;
					printf("le message total est %s\n", msgall);

					strcpy(buff_retour ,msgall ) ;


					printf("le messag all :%s\n",buff_retour);


					msgstruct.type = BROADCAST_SEND ;
					msgstruct.pld_len = strlen(msgall) + 1 ;

          for(int k = 1 ; k <= n ; k++){


            if(k != i){



            send_msg(pollfds[k].fd ,&msgstruct, sizeof(struct message)) ;

    				send_msg(pollfds[k].fd  , buff_retour , msgstruct.pld_len) ;
            //pollfds[k].revents = 0;

          //  printf("received  : %s\n" ,buff ) ;s
            }
						//memset(buff_retour,0, MSG_LEN);
          }
        }


				        if( strncmp(buff, "/msg" , 4) == 0 &&   strncmp(buff, "/msgall" , 7) != 0 )
				        {
									memset(buff_retour,0, MSG_LEN);
									char tmp1[MSG_LEN] ;
									char tmp2[MSG_LEN] ;

									//char pseudo[MSG_LEN] ;
									char msg[MSG_LEN] ;
									char pseudo[MSG_LEN] ;
									char msg_unicast[MSG_LEN] ;
									memset(tmp1 ,0,MSG_LEN) ;
									memset(tmp2 ,0,MSG_LEN) ;
									memset(pseudo ,0,MSG_LEN) ;
									memset(msg ,0,MSG_LEN) ;
									memset(msg_unicast , 0 ,MSG_LEN) ;


										cpy(tmp1 ,buff ,4) ;
										cpy(tmp2 , buff ,4 );
										printf("le tmp :%s\n" ,tmp1);

										struct user* target_send =   NULL ;// malloc(sizeof(struct user)) ;
										struct user* target_rcv =   NULL ;//

										printf("le retour de strtok :%s\n" , strtok(tmp1 , " "));
										//char*pseudo = strtok(tmp ,"") ;
										strcpy(pseudo , strtok(tmp1 , "") )  ;

										printf("le tmp apres strtok est :%s\n" , tmp1 ) ;

										if( parcour(cli , tmp1 , strlen(pseudo) ) == 0 ){
											memset(buff_retour ,0,MSG_LEN);
											char*error = "you can't send your message because we don't have any user with this pseudo "  ;
											strcpy(buff_retour , error) ;
											msgstruct.pld_len = strlen(error) + 1 ;

																					send_msg(pollfds[i].fd ,&msgstruct, sizeof(struct message)) ;

																					send_msg(pollfds[i].fd  , buff_retour , msgstruct.pld_len) ;

										}
										else{

										target_send = get_user_from_server(cli , pseudo) ;
										target_rcv = get_user_from_server_with_fd(cli , pollfds[i].fd );
										cpy(msg_unicast , tmp2,strlen(tmp1)) ;
										printf("le messsage unicast est :%s\n", msg_unicast);

										strcat(msg, "[");
										strncat(msg, target_rcv->pseudo, strlen(target_rcv->pseudo) - strlen("\n"));
										//strcat(msgall, targe->pseudo ) ;
										strcat(msg , "]:") ;

										strcat(msg,msg_unicast) ;
										strcat(buff_retour ,msg) ;

										//cpy(buff_retour ,tmp ,strlen(pseudo)) ;
										printf("le buff retour de /msg est :%s",buff_retour);
										msgstruct.type = UNICAST_SEND ;
										msgstruct.pld_len = strlen(buff_retour) + 1 ;


										send_msg(target_send->fd ,&msgstruct, sizeof(struct message)) ;

										send_msg(target_send->fd  , buff_retour , msgstruct.pld_len) ;

									}

									}
									if( strncmp(buff, "/" , 1) != 0 ){
										send_msg(pollfds[i].fd ,&msgstruct, sizeof(struct message)) ;

										send_msg(pollfds[i].fd , buff , msgstruct.pld_len) ;
									}
									memset(buff_retour , 0 ,MSG_LEN);



                display(cli);


                printf("len de cli est :%d \n",len(cli));
        					printf("le nombre de client est %d \n" , n ) ;

					printf("pld_len: %i / nick_sender: %s / type: %s / infos: %s\n", msgstruct.pld_len, msgstruct.nick_sender, msg_type_str[msgstruct.type], msgstruct.infos);


					// rcv()
					//receive(pollfds[i].fd , buff) ;
					if( strncmp(buff, "/quit" , 5) == 0 ){
					// close the connection

					close(pollfds[i].fd) ;


					clear_user(&cli) ;
					clear_list(&list) ;
					pollfds[i].fd = -1  ;
					pollfds[i].events = -1 ;

				}


					printf("received  : %s\n" ,buff ) ;
					//send()
					 memset(buff_retour,0,MSG_LEN);

					printf("message sent !\n");

			}
		}

		//clear_user(&cli) ;
	//clear_list(&list) ;

	}


close(fd_server) ;


}






int main(int argc , char**argv){






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



	server(fd_server , port) ; // accept



	return 0 ;
}
