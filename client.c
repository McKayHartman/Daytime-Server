#include "client.h"

int main() {
  char message_buffer[MAX_MESSAGE_LENGTH + 1]; // for reading time
  int client_socket = -1;                      // client-side conenction socket
  struct sockaddr_in client_address;           // store address and port

  // address information for DNS lookup
  int status;
  struct addrinfo hints;
  struct addrinfo *server_info;
  struct addrinfo *traverse;

  memset(&hints, 0, sizeof(hints)); // check for empty struct
  hints.ai_family = AF_UNSPEC;      // allow IPv4 and IPv6 configurations
  hints.ai_socktype = SOCK_STREAM;  // TCP connection specified
  hints.ai_flags = AI_PASSIVE;      // fill source IP

  // attempt dns lookup
  status = getaddrinfo(DAYTIME_HOST, DAYTIME_PORT, &hints, &server_info);
  // check for failed lookup
  if (status != 0) {
    fprintf(stderr, "gai error: %s\n", gai_strerror(status));
    exit(1);
  }

  // procedure with successful DNS lookup
  printf("Daytime client running\n");

  printf("status is: %d\n", status);

  // loop for candidates in list
  for (traverse = server_info; traverse != NULL; traverse = traverse->ai_next) {
    // attempt to make socket (ipv6 or ipv4)
    client_socket =
        socket(traverse->ai_family,    // IP version
               SOCK_STREAM,            // TCP only, specified at struct creation
               traverse->ai_protocol); // socket protocol number
    // check for invalid socket creation
    if (client_socket == -1) {
      perror("socket creation failed");
      continue; // skip to next iteration
    }
    // attempt to connect using a valid socket, check for invalid connection
    if (connect(client_socket, traverse->ai_addr, traverse->ai_addrlen) == -1) {
      perror("connection failure");
      close(client_socket);
      continue; // skip iteration
    }
    // leave loop if successful connection made
    break;
  }

  // try for connection read
  get_time(client_socket);

  // create open TCP socket endpoint
  // client_socket = socket(AF_INET, SOCK_STREAM, 0);
  // specify corresponding struct members
  // client_address.sin_family = AF_INET;
  // client_address.sin_addr.s_addr = inet_addr(

  // free list of address infos
  freeaddrinfo(server_info);

  return 0;
}

void get_time(int client_socket) {
  // check for connection never made
  if (client_socket == -1) {
    perror("connection never made");
    return;
  }

  bool keep_going = true; // sentinel for end of message found
  int i = 0;              // for message traversing
  char message_buffer[MAX_MESSAGE_LENGTH + 1]; // hold message from server
  // get input from server
  while (i <= MAX_MESSAGE_LENGTH && keep_going == true) {
    char read_char;
    // read single character from server
    int n = recv(client_socket, &read_char, 1, NULL);
    // check for read error
    if (n == -1) {
      perror("read error");
      return;
    }
    // check for connection closed
    else if (n == 0) {
      perror("connection closed");
      return;
    }
    // append received message to end of buffer
    message_buffer[i] = read_char;
  }
}
