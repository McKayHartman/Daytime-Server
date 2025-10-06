#include "connection.h"

int main()
{
    char message_buffer[MAX_MESSAGE_LENGTH];    // reserve space for connection output
    int client_socket;                          // receive endpoint
    struct sockaddr_in client_address;          // client socket naming struct

    // create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    // fill address struct
    client_address.sin_family       = AF_INET;
    client_address.sin_addr.s_addr  = inet_addr(PROXY_SERVER_ADDRESS);
    client_address.sin_port         = htons(PROXY_LISTEN_PORT);

    // attempt proxy server connection
    if (connect(client_socket, (struct sockaddr *)&client_address, sizeof(client_address)) == -1)
    {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }

    // read_time(client_socket, message_buffer);
    read_from_proxy(client_socket, message_buffer);

    printf("%s\n", message_buffer);

    return 0;
}

void read_from_proxy(int client_socket, char *message_buffer)
{
    // check for invalid socket
    if (client_socket == -1)
    {
        perror("Socket error");
        return;
    }

    ssize_t bytes_read = 0;
    ssize_t curr_read = 0;

    // loop while sent data is incomplete
    do
    {
        curr_read = recv(client_socket, message_buffer + bytes_read,
                                        MAX_MESSAGE_LENGTH - bytes_read, 0);
        // check for send failure
        if (curr_read < 0)
        {
            perror("Send error");
            break;
        }
        // update message size
        bytes_read += curr_read;
    } while (curr_read != 0);   // terminate on graceful connection close
}