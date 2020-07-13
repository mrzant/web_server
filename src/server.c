#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include "server.h"
#include "sensor.h"

/* HTTP Responses*/
const char *const GET = "GET";
const char *const MNA = "HTTP/1.1 405 Method Not Allowed\nAllow: GET\nContent-Type: text/plain\n\nStatus: 405 Method Not Allowed\nOnly GET Method Allowed\n";
const char *const OK = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\nHello world!\n";
const char *const PNF = "HTTP/1.1 404 Not Found\nContent-Type: text/plain\n\nStatus: 404 Page Not Found\nThe requested resource was not found on this server\n";

/* Mutex initializer*/
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;


/* Creates a socket and returns file descriptor*/
int *make_socket(void){

	static int arr[2];

	/* Creates and endpoint for a two-way communication via IPv4 protocol*/
	if( (arr[0] = socket(AF_INET, SOCK_STREAM, false)) < false) {

		perror("IN SOCKET CREATION!");
		exit(EXIT_FAILURE);
	}

	name.sin_family = AF_INET; /* IP */
	name.sin_port = htons(PORT); /* Address port */
	name.sin_addr.s_addr = htonl(INADDR_LOOPBACK); /*Internet address "localhost"*/
	arr[1] = sizeof(name);

	/* Associate and reserve a port for a socket */
	if( (bind(arr[0], (struct sockaddr*)&name, arr[1])) < false){

		perror("IN BINDING!");
		exit(EXIT_FAILURE);
	}

	return arr; 
}

/* Extracts string indices for a route from incoming HTTP request */
int *string_indices(char *string){

	int i,j;
	boolean FLAG = false;
	static int indices[2];
	
	/* Loop over incoming string (request) */
	for(i = 0; i < strlen(string); i++) {
	
		/* Checks for a new line */
		if ( string[i] == '\n') {

			for(j = 0; j < i; j++) {
	
				/* Checks for a root*/
				if (string[j] == '/') {

					indices[0] = j;
					FLAG = true;
				}
				/* Checks for an end of route */
				else if (FLAG && string[j] == ' ') {

					indices[1] = j-1;
					return indices;
				}
			}
		}
	}
	fprintf(stderr, "UNABLE TO EXTRACT INDICES");
	exit(EXIT_FAILURE);
}

/* Extracts the route according to returned indices from @string_indices */
char *slice_string(int start, int end, char *string) {
		
		int i;
		char *substring = malloc( sizeof(char) * ((end-start)+2));

		/* create a substring (route) from incoming request*/
		for ( i = start; i <= end; i++) substring[i-start] = string[i];

		substring[i-start] = '\0';
		
		return substring;
}

/* Checks if the route matches with existing resources */
int check_route(char *route) {

	if ( ((strcmp("/sensor", route)) == false) || ((strcmp("/", route)) == false) ) return true;

	else return false;
}

/* Start routine function of a thread */
void *function(void *ptr){

	int sockfd = (int) ptr;
	int n, *indices, FLAG = 0;
	char buffer[MAX], *found;
	char SEN[MAX] = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\n";

	bzero(buffer, MAX);

	/* Reads streaming message */
	if( ( n = read(sockfd, buffer, MAX)) < false) {
		perror("IN READ!");
		exit(EXIT_FAILURE);
	}

	else {

		/* Checks GET method, otherwise write 405 */
		if ((strstr(buffer, GET)) == NULL) {
			/* Writes stream message */
			if ( (write(sockfd, MNA, strlen(MNA))) < false) perror("IN WRITE STREAM MESSAGE!"); 
			/* Close socket*/
			if ( (close(sockfd)) < false) perror("UNABLE TO CLOSE SOCKET!");
			pthread_exit(NULL);
		} else {

			/* Check route*/
			indices = string_indices(buffer);
			found = slice_string(indices[0], indices[1], buffer);

			/* If /<sensor> route*/
			if ( (check_route(found)) && (strlen(found) > true) ) {

				/* Lock mutex */
				pthread_mutex_lock(&mutex1);

				/* Read sensor value */
				while(!FLAG) {
					FLAG = sensor();
					sleep(1);
				}
				
				/* Concatenate HTTP response headers with message (sensor values) body*/
				strcat(SEN, temp_hum);
				/* Writes stream message */
				if( (write(sockfd, SEN, strlen(SEN))) < false){
					perror("IN WRITE STREAM MESSAGE!");
				}

				if( (close(sockfd)) < false) perror("UNABLE TO CLOSE SOCKET");
	
				/* Unlock mutex */
				pthread_mutex_unlock(&mutex1);
				/* Terminates thread */
				pthread_exit(NULL);

			}

			/* Else if root route */
			else if ( (check_route(found)) && (strlen(found) == true)) {

				if( (write(sockfd, OK, strlen(OK))) < false) perror("IN WRITE STREAM MESSAGE!");
				if ( (close(sockfd)) < false) perror("UNABLE TO CLOSE SOCKET!");
				pthread_exit(NULL);

			} else {

				/* The requested resource does not exist on this web server, return 404 */
				if ( (write(sockfd, PNF, strlen(PNF))) < false) perror("IN WRITE STREAM MESSAGE!");
				if ( (close(sockfd)) < false) perror("UNABLE TO CLOSE SOCKET!");
				pthread_exit(NULL);
			}
		}
	}
}


