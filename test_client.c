#include "server.h"

/************************************************************************
 * MAIN
 ************************************************************************/
int main() {
    char input[100];                    // buffer for user input
    int client_socket;                  // client side socket
    struct sockaddr_in client_address;  // client socket naming struct
    char c;
    
    
    // create an unnamed socket, and then name it
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    // create addr struct
    client_address.sin_family = AF_INET;
    client_address.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    client_address.sin_port = htons(PORT);
    
    // connect to server socket
    if (connect(client_socket, (struct sockaddr *)&client_address, sizeof(client_address)) == -1) {
        perror("Error connecting to server!\n");
        exit(EXIT_FAILURE);
    }



    
    // buffer will store the daytime string
    char buffer[256];
    // read function retrieves the string and stores it in buffer
    int n = read(client_socket, buffer, sizeof(buffer) - 1);
    // add null terminator so no garbage output
    buffer[n] = '\0';
    // print to terminal
    printf("%s\n", buffer);
    // exit
    return EXIT_SUCCESS;
}