#include "client.h"

int main() 
{
    // initalize variables
    char message_buffer[MAX_MSG_LEN + 1];
    char *message = "Hello Server";
    int client_socket;
    struct sockaddr_in servAddr;

    // add in DNS functionality

    // clear the server address
    bzero(&servAddr, sizeof(servAddr));
    servAddr.sin_addr.s_addr = inet_addr("104.168.175.100"); // put ip address
    servAddr.sin_port = htons(PORT);
    servAddr.sin_family = AF_INET;

    // create the datagram socket
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);

    // connect to the server
    if(connect(client_socket, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
    {
        printf("\n Error: Connection Failed \n");
        exit(0);
    }

    // request to send the datagram
    // no need to specify server address in send to
    // connect stores the peers ip and port
    sendto(client_socket, message, MAX_MSG_LEN, 0, (struct sockaddr*)NULL, sizeof(servAddr));

    // wait for server to respond
    recvfrom(client_socket, message_buffer, sizeof(message_buffer), 0, (struct sockaddr*)NULL, NULL);
    puts(message_buffer);

    // close file descriptor
    close(client_socket);
}


void read_time(int client_socket, char *message_buffer) 
{
    // check for connection never made
    if(client_socket == -1)
    {
        perror("Connection never made");
        return;
    }
    
    // initialize variables
    int index = 0;
    char read_char;
    
    // get input from server
    while(index < MAX_MSG_LEN)
    {
        // read single character from server
        int n = recv(client_socket, &read_char, 1, 0);

        // check for read error
        if (n == -1)
        {
            perror("read error");
            message_buffer[index] = '\0';
            return;
        }
        // check for connection closed
        else if (n == 0)
        {
            message_buffer[index] = '\0';
            return;
        }

        // check for end of message character
        if (read_char == '*')
        {
            message_buffer[index] = '\0';   // fix string state
            return;                         // process finished
        }

        // append received message to end of buffer
        message_buffer[index] = read_char;
        index++;
        message_buffer[index] = '\0';
    }
}
