#include "proxy_server.h"

int main()
{
    int server_socket;      // socket to reach - listening point
    pthread_t thread_id;    // allow for thread creation

    // create a listening point
    server_socket = create_server_socket();

    // listen on socket
    if (listen(server_socket, NUM_CONNECTIONS) != 0)
    {
        perror("Error in listening socket");
        exit(EXIT_FAILURE);
    }

    // show proxy server listening
    printf("Server listening on port %d\n", PROXY_LISTEN_PORT);

    // server control loop
    while (true)
    {
        // allocate unique client socket - scope exists in current state only
        int *client_socket = (int *)malloc(sizeof(int));
        // wait for client connection
        *client_socket = accept(server_socket, NULL, NULL);

        printf("\nServer with PID %d: accepted client\n", getpid());

        // try thread create
        if (pthread_create(&thread_id, NULL, handle_client, (void *)client_socket) != 0)
        {
            // thread creation failure
            perror("Error creating thread");
            exit(EXIT_FAILURE);
        }
        // try separating thread - execute without waiting
        if (pthread_detach(thread_id) != 0)
        {
            perror("Thread detaching error");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

int connect_to_server(struct addrinfo **server_info)
{
    // default socket value to failure
    int upstream_socket = -1;
    // address information for DNS lookup
    int status;
    struct addrinfo hints;
    struct addrinfo *traverse;

    memset(&hints, 0, sizeof(hints));   // check for empty struct
    hints.ai_family = AF_UNSPEC;        // allow IPv4 and IPv6 configurations
    hints.ai_socktype = SOCK_STREAM;    // TCP connection specified
    hints.ai_flags = 0;                 // client connection format

    // attempt dns lookup
    status = getaddrinfo(DAYTIME_HOST, DAYTIME_PORT, &hints, server_info);
    // check for failed lookup
    if (status != 0)
    {
        fprintf(stderr, "gai error: %s\n", gai_strerror(status));
        exit(1);
    }

    // loop for candidates in list
    for (traverse = *server_info; traverse != NULL; traverse = traverse->ai_next)
    {
        // attempt to make socket (ipv6 or ipv4)
        upstream_socket = socket(
                                traverse->ai_family,        // IP version
                                traverse->ai_socktype,      // TCP only, specified at struct creation
                                traverse->ai_protocol);     // socket protocol number
        // check for invalid socket creation
        if (upstream_socket == -1)
        {
            perror("socket creation failed");
            continue;   // skip to next iteration
        }
        // attempt to connect using a valid socket, check for invalid connection
        if (connect(upstream_socket, traverse->ai_addr, traverse->ai_addrlen) == -1)
        {
            perror("connection failure");
            close(upstream_socket);
            continue;   // skip iteration
        }
        // leave loop if successful connection made
        break;
    }

    // procedure with successful DNS lookup and connection
    printf("Connection to Daytime Server made\n");

    // send back socket
    return upstream_socket;
}

int create_server_socket()
{
    int server_socket = -1;             // set with failure value by default
    struct sockaddr_in server_address;  // used to name listening socket
    int yes = 1;                        // for editing options

    // ignore SIGPIE
    signal(SIGPIPE, SIG_IGN);

    // create unnamed socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // change options - avoid "Address already in use" error
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // bind socket
    server_address.sin_family        = AF_INET;             // accept IP addresses
    server_address.sin_addr.s_addr   = htonl(INADDR_ANY);   // accpet clients on any interface
    server_address.sin_port          = htons(PROXY_LISTEN_PORT);  // port to listen on

    // binding unnamed socket
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) != 0)
    {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    return server_socket;
}

ssize_t get_daytime(char *message_buffer)
{
    int upstream_socket;        // for requests to NIST server
    int num_read_attempts = 0;  // track occurances of Connection Reset by Peer errors
    ssize_t bytes_received = 0; // track size of message received from NIST

    // output parameter for DNS lookup/server connection
    struct addrinfo *server_info = NULL;

    // loop for number of attempts
    while (num_read_attempts < MAX_READ_ATTEMPTS)
    {
        // log attempt to connect
        fprintf(stderr, "\nConnection attempt %d\n", num_read_attempts + 1);
        // attempt connection
        upstream_socket = connect_to_server(&server_info);
        // check for failure
        if (upstream_socket == -1)
        {
            fprintf(stderr, "Upstream socket creation failure\n");
            // free allocated list
            freeaddrinfo(server_info);
            // return invalid value as failure
            return -1;
        }

        // try for connection read - assign message size
        bytes_received = read_time(upstream_socket, message_buffer);

        num_read_attempts++;        // update state

        close(upstream_socket);     // close upstream connection
        freeaddrinfo(server_info);  // free nodes in list

        // check for success
        if (bytes_received > 0)
        {
            // successful read - exit loop
            break;
        }

        // display failure to connect message
        fprintf(stderr, "Connection attempt failed\n");

        // pause execution - prevent overload requests
        sleep(SLEEP_TIME);
    }

    printf("Connection closed\n");

    // return message size
    return bytes_received;
}

void *handle_client(void* arg)
{
    int client_socket = *((int*)arg);           // the socket connected to the client
    char message_buffer[MAX_MESSAGE_LENGTH];    // for storing time message

    // used to check size of sent string
    ssize_t num_bytes;

    // deallocate client file descriptor
    free(arg);

    // attempt to get daytime
    num_bytes = get_daytime(message_buffer);
    
    // check for valid message received
    if (num_bytes != -1)
    {
        // pass message to client
        send_daytime(client_socket, num_bytes, message_buffer);
    }

    // end of loop - detail no more data to send
    shutdown(client_socket, 1);

    // close client socket
    close(client_socket);

    // close calling thread
    pthread_exit(NULL);
}

ssize_t read_time(int read_socket, char *message_buffer)
{
    // check for connection never made
    if (read_socket == -1)
    {
        perror("connection never made");
        return -1;
    }
    
    ssize_t bytes_read = 0; // total bytes read through loop
    ssize_t curr_read = 0;  // bytes read per iteration

    // loop for message length - reserve space for null terminator
    while (bytes_read < MAX_MESSAGE_LENGTH - 1)
    {
        curr_read = 0;
        // attempt to read from NIST server
        curr_read = recv(read_socket, message_buffer + bytes_read,
                                        MAX_MESSAGE_LENGTH - bytes_read, 0);

        // check for receive error - not from congestion
        if (curr_read < 0)
        {
            perror("Receive error");
            return -1;
        }
        // check for early connection close
        if (curr_read == 0)
        {
            fprintf(stderr, "Connection closed by host\n");
            return -1;
        }

        // loop for new read data
        for (ssize_t i = 0; i < curr_read; i++)
        {
            // check for end of message
            if (message_buffer[bytes_read + i] == '*')
            {
                message_buffer[bytes_read + i] = '\0';
                return bytes_read + i;
            }
        }

        // update message buffer occupied space
        bytes_read += curr_read;
        // fix state
        message_buffer[bytes_read] = '\0';
    }
    // give sender size of message
    return bytes_read;
}

void send_daytime(int client_socket, ssize_t num_bytes, char *message_buffer)
{
    ssize_t bytes_sent = 0; // track message size sent through loops
    ssize_t curr_send = 0;  // track message size sent per loop

    // loop while sent data is incomplete
    while (bytes_sent < num_bytes)
    {
        curr_send = send(client_socket, message_buffer + bytes_sent,
                                                    num_bytes - bytes_sent, 0);
        // check for send failure
        if (curr_send <= 0)
        {
            perror("Send error");
            break;
        }
        // update message size
        bytes_sent += curr_send;
    }
}