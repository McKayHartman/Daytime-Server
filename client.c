#include "client.h"

int main() {
    char message_buffer[MAX_MESSAGE_LENGTH + 1];    // for reading time
    int client_socket = -1;                         // client-side conenction socket
   
    // address information for DNS lookup
    int status;
    struct addrinfo hints;
    struct addrinfo *server_info;
    struct addrinfo *traverse;

    memset(&hints, 0, sizeof(hints));   // check for empty struct
    hints.ai_family = AF_UNSPEC;        // allow IPv4 and IPv6 configurations
    hints.ai_socktype = SOCK_STREAM;    // TCP connection specified
    hints.ai_flags = 0;                 // client connection format

    // attempt dns lookup
    status = getaddrinfo(DAYTIME_HOST, DAYTIME_PORT, &hints, &server_info);
    // check for failed lookup
    if (status != 0) {
        fprintf(stderr, "gai error: %s\n", gai_strerror(status));
        exit(1);
    }
    
    // procedure with successful DNS lookup 
    printf("Daytime client running\n");

    // loop for candidates in list
    for (traverse = server_info; traverse != NULL; traverse = traverse->ai_next) {
        // attempt to make socket (ipv6 or ipv4)
        client_socket = socket(
                                traverse->ai_family,        // IP version
                                traverse->ai_socktype,      // TCP only, specified at struct creation
                                traverse->ai_protocol);     // socket protocol number
        // check for invalid socket creation
        if (client_socket == -1) {
            perror("socket creation failed");
            continue;   // skip to next iteration
        }
        // attempt to connect using a valid socket, check for invalid connection
        if (connect(client_socket, traverse->ai_addr, traverse->ai_addrlen) == -1) {
            perror("connection failure");
            close(client_socket);
            continue;   // skip iteration
        }
        // leave loop if successful connection made
        break;
    }

    // try for connection read
    read_time(client_socket, message_buffer);
    
    // display message to user
    printf("%s\n", message_buffer);

    close(client_socket);       // close connection
    freeaddrinfo(server_info);  // free nodes in list
    
    printf("Connection closed\n");

    return 0;
}

void read_time(int client_socket, char *message_buffer) {
    // check for connection never made
    if (client_socket == -1) {
        perror("connection never made");
        return;
    }
    
    int i = 0;      // for message traversing
    
    // get input from server
    while (i < MAX_MESSAGE_LENGTH) {
        char read_char;
        // read single character from server
        int n = recv(client_socket, &read_char, 1, 0);
        // check for read error
        if (n == -1) {
            perror("read error");
            message_buffer[i] = '\0';
            return;
        }
        // check for connection closed
        else if (n == 0) {
            message_buffer[i] = '\0';
            return;
        }

        // check for end of message character
        if (read_char == '*') {
            message_buffer[i] = '\0';   // fix string state
            return;                     // process finished
        }

        // append received message to end of buffer
        message_buffer[i] = read_char;
        i++;
        message_buffer[i] = '\0';
    }
}
