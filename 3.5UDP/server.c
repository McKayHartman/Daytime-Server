#include "server.h"
#include <sys/socket.h>


/* ************************************************************************* */
/* MAIN                                                                      */
/* ************************************************************************* */

int main(int argc, char** argv) {
  int server_socket;                 // descriptor of server socket
  struct sockaddr_in server_address, client_address; // for naming the server's listening socket
  int yes = 1;


  // ----------------------------------------------------------
  // ignore SIGPIPE, sent when client disconnected
  // ----------------------------------------------------------
  signal(SIGPIPE, SIG_IGN);

  // ----------------------------------------------------------
  // create unnamed network socket for server to listen on
  // ----------------------------------------------------------
  if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    perror("Error creating socket");
    exit(EXIT_FAILURE);
  }

  // lose the pesky "Address already in use" error message
  if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  // ----------------------------------------------------------
  // bind the socket
  // ----------------------------------------------------------
  server_address.sin_family      = AF_INET;           // accept IP addresses
  server_address.sin_addr.s_addr = htonl(INADDR_ANY); // accept clients on any interface
  server_address.sin_port        = htons(PORT);       // port to listen on

  // binding unnamed socket to a particular port
  if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) != 0) {
    perror("Error binding socket");
    exit(EXIT_FAILURE);
  }

  char buffer[128];
  socklen_t len = sizeof(client_address);
  int n = recvfrom(server_socket, buffer, sizeof(buffer),
                   0, (struct sockaddr*)&client_address, &len); //receive message from server
  printf("Server Recvfrom: %d\n", n);
  buffer[n] = '\0';
  puts(buffer);

  // send the response
  sendto(server_socket, get_daytime(), 128, 0,
         (struct sockaddr*)&client_address, sizeof(client_address));
}




// Function to get the daytime string in NIST format
// Format: "JJJJJ YY-MM-DD HH:MM:SS TT L H msADV UTC(NIST) OTM"
char* get_daytime() {
  // get time from system as UNIX timestamp
  time_t now = time(NULL);
  // convert to NIST format

  // buffer will hold the time string
  char buffer[256];
  // get julian date
  int julian_date = (now / 86400) + 40587;
  // get the broken down time from c library time.h
  struct tm *broken_down_time = gmtime(&now);
  // format the struct into a string
  char stamp[64];
  strftime(stamp, sizeof(stamp), "%y-%m-%d %H:%M:%S", broken_down_time);
  // compose the final string (using 0s for the special fields)
  snprintf(buffer, sizeof(buffer), "%d %s 00 0 0 0.0 UTC(NIST) *", julian_date, stamp);
  // allocate memory for the string to be returned
  char* daytime = malloc(strlen(buffer) + 1);
  // copy the string into the allocated memory
  strcpy(daytime, buffer);
  // return the string
  return daytime;
}



