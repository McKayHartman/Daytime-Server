#ifndef PROXY_SERVER_H
#define PROXY_SERVER_H

// shared packages and constants for client-proxy connection
#include "connection.h"

#include <string.h>
#include <errno.h>

// DNS lookup
#include <netdb.h>

// threading
#include <pthread.h>

#define DAYTIME_HOST "time.nist.gov"
#define DAYTIME_PORT "13"

#define MAX_READ_ATTEMPTS 3 // retries for connection reset by peer
#define SLEEP_TIME 1        // time between conenction retries (seconds)

int connect_to_server(struct addrinfo **server_info);

int create_server_socket();

ssize_t get_daytime(char *message_buffer);

void *handle_client(void *arg);

ssize_t read_time(int client_socket, char *message_buffer);

void send_daytime(int client_socket, ssize_t num_bytes, char *message_buffer);

#endif
