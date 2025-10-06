#include <signal.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <time.h>
#include <netdb.h>

/* Function prototypes */
char* get_daytime();


/* Preprocessor directives */
#define SERVER_ADDR "127.0.0.1" // loopback ip address
#define PORT 5000 // port the server will listen on

#define FALSE 0
#define TRUE !FALSE

#define NUM_CONNECTIONS 5       // number of pending connections in the connection queue

