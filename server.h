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

// My stuff
#include <string.h>
#include <time.h>
#include <netdb.h>

/* Function prototypes */
void* handle_client(void* arg);

char* get_daytime();

// From Beej's Guide to Network Programming
int getaddrinfo(const char *node,   // e.g. "www.example.com" or IP
                const char *service,  // e.g. "http" or port number
                const struct addrinfo *hints,
                struct addrinfo **res);

/* Preprocessor directives */
#define SERVER_ADDR "127.0.0.1" // loopback ip address
#define PORT 23657              // port the server will listen on

#define FALSE 0
#define TRUE !FALSE

#define NUM_CONNECTIONS 5       // number of pending connections in the connection queue

