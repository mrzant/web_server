#ifndef _SERVER_H
#define _SERVER_H

#include <netinet/in.h>
#include <pthread.h>
 
#define MAXPENDING 5
#define MAX 256
int PORT;

/* bools */
typedef enum{false, true} boolean;

/* struct for tcp/ip */
struct sockaddr_in name;

/* thread ids */
pthread_t thread, thread_s;

/* thread attribute */
pthread_attr_t attr;

/* function declarations*/
int *make_socket(void);
int *string_indices(char *string);
char *slice_string(int start, int end, char *string);
int check_route(char *route);
void *function(void *ptr);
int sensor(void);

#endif
