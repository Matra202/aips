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

void build_message(char *message, int size, char character, int j){
	int i;
	for (i=0; i<size; i++) {
		if (i<4){
			message[i]= '-';
		}
		else if (i==4) {
			message[i]=j +'1';
		}
		else{
		message[i] = character;
		}
	}
}

void display_message(char *message, int size){
	int i;
	for (i=0; i<size; i++){
		printf("%c" , message[i]);
	}
	printf("\n");
}

void create_local_socket(int port , int type, int protocole){
	int sock;
	if (protocole == 1) {
		if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
			{ printf("échec de création du socket\n") ;
			exit(1) ; 
			}
		struct sockaddr_in adr_local;
		memset((char *)& adr_local, 0, sizeof(adr_local)) ;

		adr_local.sin_family = AF_INET ; /* domaine Internet */
		adr_local.sin_port = port ; /* n° de port */
		adr_local.sin_addr.s_addr = INADDR_ANY;
	}
}

int create_distant_socket(int port, int type, int protocole){
	int sock;
	if (protocole == 1) {
		if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
			{ printf("échec de création du socket\n") ;
			exit(1) ; 
			}
		struct hostent *hp ;
		struct sockaddr_in adr_distant;

		memset((char *)& adr_distant, 0, sizeof(adr_distant)) ;
		adr_distant.sin_family = AF_INET ; /* domaine Internet*/
		adr_distant.sin_port = port ; /* n° de port */

		if ((hp = gethostbyname("dumas.insa-tlse.fr")) == NULL)
			{ printf("erreur gethostbyname\n") ;
			exit(1) ; }
			
		memcpy( (char*)&(adr_distant.sin_addr.s_addr),
					hp->h_addr,
					hp->h_length ) ;

		printf("socket built, ready to send messages\n");

	}
	else{}
	return sock;
}
//void address_socket(socket){}


void sender_side(int lg_mesg, int port, int protocole, char* adr){
	printf("SOURCE : longueur du message émis , n° de port local, valeur des options, protocole de transport utilisé, nom de la machine destinataire \n");
}

void receiver_side(int lg_mesg, int port, int protocole){
	printf("PUITS : longueur du message lu, n° de port local, valeur des options, protocole de transport utilisé \n");
	int sock ;
	struct sockaddr_in adr_local ; /* adresse du socket local */
	int lg_adr_local = sizeof(adr_local) ;
}

void main (int argc, char **argv)
{
	int c;
	extern char *optarg;
	extern int optind;
	int nb_message = -1; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
	int source = -1 ; /* 0=puits, 1=source */
	int protocole = 0; /* 0 = TCP, 1 = UDP par défaut TCP si il n'y a pas le -u dans les options*/ 
	while ((c = getopt(argc, argv, "pn:su")) != -1) {
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
		//create_socket(domain,type,protocole);


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

	/*printf("SOURCE : Envoi n° xxxxx (yyyyy) [*…*] ");
	printf("PUITS: Réception n°xxxxx (yyyyy) [*…*] ");*/
	int port = atoi(argv[argc-1]);
	char* message = malloc(sizeof(char)*30);
	char characters[26] = "abcdefghijklmnopqrstuvwxyz";
	int size = 30;
	int i;
	for (i=0; i<nb_message; i++){
		build_message(message,size,characters[i],i);
		printf("Envoi n°%d (%d) [%s]\n" , i+1 , size, message);
	}
}

