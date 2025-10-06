#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// socket/bind/listen/accept
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

// read/write/close
#include <sys/uio.h>
#include <sys/types.h>
#include <unistd.h>

#include <signal.h>

#define PROXY_SERVER_ADDRESS "23.254.202.24"   // IP for proxy server
#define PROXY_LISTEN_PORT 23657                 // port server is listening on

#define MAX_MESSAGE_LENGTH 80                   // largest anticipated message size

#define NUM_CONNECTIONS 1                       // limit amount of connections allowed

// void read_time(int client_socket, char *message_buffer);

void read_from_proxy(int client_socket, char *message_buffer);

#endif