#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// socket bind, listen, accept
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

// DNS lookup
#include <netdb.h>

// read/write/close
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <signal.h>

#define DAYTIME_HOST "time.nst.gov"
#define DAYTIME_PORT "13"

#define MAX_MESSAGE_LENGTH 80

void get_time(int client_socket);
