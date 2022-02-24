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


void display_message(char *message, int size,int nb_message){
	int i;
	for (i=0; i<nb_message; i++){
		//build_message(message,size,characters[i],i);
		printf("Envoi n°%d (%d) [%s]\n" , i+1 , size, message);
	}
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
	printf("PUITS: Réception n°xxxxx (yyyyy) [*…*] ");
	printf("PUITS : longueur du message lu, n° de port local, valeur des options, protocole de transport utilisé \n");*/

	if (protocole == 1) {
		printf("we only support TCP with v2 please use v1 for UDP\n");
	}
	else{ 

		int port = atoi(argv[argc-1]);
		port = htons(port);

		char* message = malloc(sizeof(char)*30);

		/* si on est dans la source*/
		if (source == 1){ 
			
			char *machine_dest = argv[argc-2];

			char characters[26] = "abcdefghijklmnopqrstuvwxyz";
			int size = 30;
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

			printf("SOURCE : longueur du message émis %d, n° de port local %d, valeur des options, protocole de transport utilisé TCP, nom de la machine destinataire %s\n",size,port,machine_dest);
			/*socket built, ready to connect*/

			if (connect(sock,(struct sockaddr *)&adr_distant, sizeof(adr_distant)) == -1) {
	  			printf("echec de la connexion \n");
	    		exit(1);
			}			

			/* On est connectés on peut envoyer */

			int i;
			int envoi;
			for (i=0;i<nb_message;i++){
				construire_message(message,characters[i],size);
				afficher_message(message,size);
				if ((envoi = write(sock, message, size))== -1)
					{ printf("échec de l'envoi\n") ;
					exit(1) ; 
					}
			}

			printf("server side completed, we now close the socket\n");
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

			int max = 10;
			int lg_max = 30;
			int lg_rec; /*longueur du message recu*/
			nb_message = 10;

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
				afficher_message(message,lg_rec);
			}
		
		if (shutdown(sock,0)==-1) { // sock,0 car on est dans le puit
			printf("Could not close socket\n");
			exit(1);
		}
		} 


	}
}


