// preprocessor directives
#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>          // printf
#include <stdlib.h>         // exit
#include <strings.h>
#include <sys/types.h>
#include <arpa/inet.h>      // inet_addr, htons, htonl, inet_pton, sockaddr_in
#include <sys/socket.h>     // socket, connect, send, recv
#include <netinet/in.h>
#include <unistd.h>         // close


// #define DAYTIME_HOST "time.nst.gov"
#define PORT 5000 
#define MAX_MSG_LEN 80


#endif // CLIENT_H
