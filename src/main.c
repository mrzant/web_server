#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <bcm2835.h>
#include "server.h"

int main(int argc, char *argv[]){

	if( (argc > 2) || !(PORT = atoi(argv[1]))) {
		
		fprintf(stderr, "Entered too many arguments or invalid argument!");
		exit(EXIT_FAILURE);
	}

	int sockfd, *ptr;
	if (!bcm2835_init()){
		fprintf(stderr, "UNABLE TO INITIALIZE GPIO LIBRARY!");
		exit(EXIT_FAILURE);
	}

	if ( (pthread_attr_init(&attr)) != false) {
		fprintf(stderr,"UNABLE TO INITIALIZE THREAD ATTRIBUTE!");
		exit(EXIT_FAILURE);
	}
	if ( (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) != false){
		fprintf(stderr,"UNABLE TO SET DETACH STATE ATTRIBUTE!");
		exit(EXIT_FAILURE);
	}
	
	ptr = make_socket();
	
	/* Listen for connections */
	if( (listen(*(ptr+0), MAXPENDING)) != false) {
		
		perror("IN LISTEN!");
		exit(EXIT_FAILURE);	
	}

	while(true) {

			/* Accept a connection on a socket */
		if( (sockfd = accept(*(ptr+0), (struct sockaddr *)&name, (socklen_t*)(ptr+1))) == -1) {

			perror("IN ACCEPT!");
			exit(EXIT_FAILURE);
		}

		/* Start a new thread per client */
		if ( (pthread_create(&thread, &attr, &function, (void*) sockfd)) != false){
			fprintf(stderr, "THREAD NOT CREATED");
			exit(EXIT_FAILURE);
		}
	}
	exit(EXIT_SUCCESS);
}
