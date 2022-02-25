/* librairie standard ... */
#include <stdlib.h>
/* pour getopt */
#include <unistd.h>
/* déclaration des types de base */
#include <sys/types.h>
/* constantes relatives aux domaines, types et protocoles */
#include <sys/socket.h>
/* constantes et structures propres au domaine UNIX */
#include <sys/un.h>
/* constantes et structures propres au domaine INTERNET */
#include <netinet/in.h>
/* structures retournées par les fonctions de gestion de la base de
données du réseau */
#include <netdb.h>
/* pour les entrées/sorties */
#include <stdio.h>
/* pour la gestion des erreurs */
#include <errno.h>

void build_message(char *message, int size, char character,int i){
	sprintf(message,"%*d",5,i);
	for (int j=0;j<4;j++){
		if (message[j]==' '){
			message[j] = '-'; 
		} 
	} 
	for (int j=5;j<size;j++){
		message[j] = character;
	}	
}


void display_message_s(char *message, int size,int i){
	printf("Envoi n°%d (%d) [%s]\n" , i, size, message);
}

void display_message_r(char *message, int size,int i){
	printf("Reception n°%d (%d) [%s]\n" , i, size, message);
}

void construire_message(char *message, char motif, int lg) {
	int i;
	for (i=0;i<lg;i++) message[i] = motif;}

void afficher_message(char *message, int lg) {
	int i;
	printf("message construit : ");
	for (i=0;i<lg;i++) printf("%c", message[i]); printf("\n");}

void main (int argc, char **argv)
{
	int c;
	extern char *optarg;
	extern int optind;
	int nb_message = -1; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
	int source = -1 ; /* 0=puits, 1=source */
	int protocole = 0; /* 0 = TCP, 1 = UDP par défaut TCP si il n'y a pas le -u dans les options*/ 
	int size =-1;
	while ((c = getopt(argc, argv, "pn:sul:")) != -1) {
		switch (c) {
		case 'p':
			if (source == 1) {
				printf("usage: cmd [-p|-s][-n ##]\n");
				exit(1);
			}
			source = 0;
			break;

		case 's':
			if (source == 0) {
				printf("usage: cmd [-p|-s][-n ##]\n");
				exit(1) ;
			}
			source = 1;
			break;

		case 'n':
			nb_message = atoi(optarg);
			break;

		case 'u':
			printf("UDP is used\n");
			protocole = 1;
			break;

		case 'l':
			size = atoi(optarg);
			break;

		default:
			printf("usage: cmd [-p|-s][-n ##]\n");
			break;
		}
	}

	if (source == -1) {
		printf("usage: cmd [-p|-s][-n ##]\n");
		exit(1) ;
	}

	if (source == 1)
		printf("on est dans le source\n");


	else
		printf("on est dans le puits\n");


	if (nb_message != -1) {
		if (source == 1)
			printf("nb de tampons à envoyer : %d\n", nb_message);
		else
			printf("nb de tampons à recevoir : %d\n", nb_message);
	} else {
		if (source == 1) {
			nb_message = 10 ;
			printf("nb de tampons à envoyer = 10 par défaut\n");
		} else
		printf("nb de tampons à envoyer = infini\n");

	}

	if (size == -1){ 
		size = 30;
		/* Valeur par défaut*/
	}

	if (protocole == 1) {

		/*UDP is used */
		int port = atoi(argv[argc-1]);
		port = htons(port);

		char* message = malloc(sizeof(char)*30);


		/* si on est dans la source*/
		if (source == 1){ 
			
			char *machine_dest = argv[argc-2];

			char characters[26] = "abcdefghijklmnopqrstuvwxyz";
			
			int sock;

			if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
				{ printf("échec de création du socket\n") ;
				exit(1) ; 
				}
			struct hostent *hp ;
			struct sockaddr_in adr_distant;

			memset((char *)& adr_distant, 0, sizeof(adr_distant)) ;
			adr_distant.sin_family = AF_INET ; /* domaine Internet*/
			adr_distant.sin_port = port ; /* n° de port */

			if ((hp = gethostbyname(machine_dest)) == NULL)
				{ printf("erreur gethostbyname\n") ;
				exit(1) ; }
				
			memcpy( (char*)&(adr_distant.sin_addr.s_addr),
						hp->h_addr,
						hp->h_length ) ;

			printf("SOURCE : longueur du message émis %d, n° de port local %d, nombre de message %i , protocole de transport utilisé UDP, nom de la machine destinataire %s\n",size,port,nb_message,machine_dest);
			/*socket built, ready to send messages*/

			int i;
			for (i=0; i<nb_message; i++){
			build_message(message,size,characters[i],i);
			display_message_s(message,size,i);

			int envoi;
			if ((envoi = sendto(sock, message, size, 0, (struct sockaddr*) &adr_distant, sizeof(adr_distant))) == -1)
				{ printf("échec de l'envoi\n") ;
				exit(1) ; 
				}
			}		
			printf("server side completed, we now close the socket\n");
			if (close(sock)==-1) {
				printf("Could not close socket\n");
				exit(1);
			}
		}

		/* on est dans le puit */
		else{

			int sock;
			if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
				{ printf("échec de création du socket\n") ;
				exit(1) ; 
				}
			struct sockaddr_in adr_local;
			memset((char *)& adr_local, 0, sizeof(adr_local)) ;

			adr_local.sin_family = AF_INET ; /* domaine Internet */
			adr_local.sin_port = port ; /* n° de port */
			adr_local.sin_addr.s_addr = INADDR_ANY;
			
			if (bind(sock, (struct sockaddr*)&adr_local, sizeof(adr_local))==-1){
				printf("Could not bind\n");
				exit(1);
			} 
		
			/* we can now receive the message with recvfrom*/

			
			int max_size = 200;
			if (nb_message==-1){ 
				nb_message = 999;
			} 

			printf("PUIT : longueur du message lu %d, n° de port local %d, nombre de message %i , protocole de transport utilisé UDP\n",size,port,nb_message);

			int size;

			struct sockaddr_in adr_sender;
			int size_adr_sender = sizeof(adr_sender);

			for (int i=0; i<nb_message; i++){
				size = recvfrom(sock, message, max_size, 0,(struct sockaddr*)&adr_sender, &size_adr_sender);

				if (size ==-1){
					printf("Could not receive message\n");
					exit(1);
				} 
				display_message_r(message,size,i);
			} 

			if (close(sock)==-1) {
				printf("Could not close socket\n");
				exit(1);
			}	 

		}
	}
	else{ 

		int port = atoi(argv[argc-1]);
		port = htons(port);

		char* message = malloc(sizeof(char)*30);

		/* si on est dans la source*/
		if (source == 1){ 
			
			char *machine_dest = argv[argc-2];

			char characters[26] = "abcdefghijklmnopqrstuvwxyz";
			if (size == -1){ 
				printf("on passe size a 30 par défaut\n");
				size = 30;
			}
			int sock;

			if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
				{ printf("échec de création du socket\n") ;
				exit(1) ; 
				}
			struct hostent *hp ;
			struct sockaddr_in adr_distant;

			memset((char *)& adr_distant, 0, sizeof(adr_distant)) ;
			adr_distant.sin_family = AF_INET ; /* domaine Internet*/
			adr_distant.sin_port = port ; /* n° de port */

			if ((hp = gethostbyname(machine_dest)) == NULL)
				{ printf("erreur gethostbyname\n") ;
				exit(1) ; }
				
			memcpy( (char*)&(adr_distant.sin_addr.s_addr),
						hp->h_addr,
						hp->h_length ) ;

			printf("SOURCE : longueur du message émis %d, n° de port local %d, nombre de message %i , protocole de transport utilisé TCP, nom de la machine destinataire %s\n",size,port,nb_message,machine_dest);
			/*socket built, ready to connect*/

			if (connect(sock,(struct sockaddr *)&adr_distant, sizeof(adr_distant)) == -1) {
	  			printf("echec de la connexion \n");
	    		exit(1);
			}			

			/* On est connectés on peut envoyer */

			int i;
			int envoi;
			for (i=0;i<nb_message;i++){
				build_message(message,size,characters[i],i);
				display_message_s(message,size,i);
				if ((envoi = write(sock, message, size))== -1)
					{ printf("échec de l'envoi\n") ;
					exit(1) ; 
					}
			}

			if (shutdown(sock,1)==-1) { //sock,1 car on est dans la source.
				printf("Could not close socket\n");
				exit(1);
			}
		}

		/* on est dans le puit */
		else{

			int sock_bis;
			struct sockaddr_in adr_client ; 
			int lg_adr_client = sizeof(adr_client);

			int max = 99;
			int lg_max = 30;
			int lg_rec; /*longueur du message recu*/
			if (nb_message == -1){ 
				nb_message = 999;
			}
			printf("PUIT : longueur du message lu %d, n° de port local %d, nombre de message %i , protocole de transport utilisé UDP\n",size,port,nb_message);

			int sock;
			if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
				{ printf("échec de création du socket\n") ;
				exit(1) ; 
				}
			struct sockaddr_in adr_local;
			memset((char *)& adr_local, 0, sizeof(adr_local)) ;

			adr_local.sin_family = AF_INET ; /* domaine Internet */
			adr_local.sin_port = port ; /* n° de port */
			adr_local.sin_addr.s_addr = INADDR_ANY;
			 
		
			if (bind(sock, (struct sockaddr*)&adr_local, sizeof(adr_local)) == -1){
				printf("Could not bind\n");
				exit(1);
			} 
		
			/* Dimensionne la file d'attente des demandes et on les accepte */
			listen(sock,5);

			if ((sock_bis=accept(sock, (struct sockaddr *)&adr_client, &lg_adr_client)) == -1) { 
				printf("Echec du accept \n");
				exit(1);
			}

			int i;
			for (i=0; i<nb_message;i++){
				if((lg_rec = read(sock_bis,message,lg_max)) <0){ 
				printf("échec du read\n");
				exit(1);
				}
				if (lg_rec == 0){
					break;
				} 
				display_message_r(message,lg_rec,i);
			}
		
		if (shutdown(sock,0)==-1) { // sock,0 car on est dans le puit
			printf("Could not close socket\n");
			exit(1);
		}
		} 


	}
}


