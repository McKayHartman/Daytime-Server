#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// socket bind, listen, accept
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

// DNS lookup
#include <netdb.h>

// read/write/close
#include <sys/uio.h>
#include <sys/types.h>
#include <unistd.h>

#include <signal.h>

#define DAYTIME_HOST "time.nst.gov"
#define DAYTIME_PORT "13"


#define MAX_MESSAGE_LENGTH 80

void read_time(int client_socket);
