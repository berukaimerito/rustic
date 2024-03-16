// #include directive is eq to import statement
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // OS API

#define PORT 8081
#define BUFFER_SIZE 1024

// In C => #define SQUARE(x) (x*x) is equivalent of lambda x: x*x

// programs entry point
int main() {

  FILE *fp = fopen("host.txt", "w");
  if (fp == NULL) {
    perror("Failed to open file");
    return EXIT_FAILURE;
  }

  int server_fd, new_socket;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);

  // Create a socket file decriptor
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed!");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  // setup the server address
  // set the address family to AF_INET (IPv4)
  address.sin_family = AF_INET;
  // accepting connections on any available interface
  address.sin_addr.s_addr = INADDR_ANY;
  // set the port number in network byte order
  address.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind fails!");
    exit(EXIT_FAILURE);
  }

  // Start listening for incoming connections
  if (listen(server_fd, 3) < 0) {
    perror("listening failed");
    exit(EXIT_FAILURE);
  }

  while (1) {
    int new_socket;
    char buffer[BUFFER_SIZE] = {0};
    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: "
                  "12\n\nHello world!";

    printf("Server listening to port %d\n", PORT);
    fprintf(fp, "Connection accepted\n");
    fflush(fp); // Flush the file stream

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0) {
      perror("Accept!");
      exit(EXIT_FAILURE);
    }
    printf("Connection accepted\n");
    // Read data from the client and print it
    // Declare a signed size type variable for the number of bytes read
    ssize_t valread;
    valread = read(new_socket, buffer, BUFFER_SIZE);
    if (valread > 0) {
      printf("Client: %s", buffer);
      send(new_socket, hello, strlen(hello), 0);
      fprintf(fp, "Client: %s\n", buffer);
      fflush(fp); // Flush the file stream after writing
      memset(buffer, 0, sizeof(buffer));
    }
    // Close the socket
    close(new_socket);
  }
  // This line is never reached in the loop, but it's good practice
  // aka resource management
  close(server_fd);
  fclose(fp);
  return 0;
}
