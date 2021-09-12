#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include "ff.h"

char *file_finder(char fileToSearch[1024], char *path);

int server(char *path, int port)
{
  FILE *f;

  /* Structure describing an Internet socket address.  */
  struct sockaddr_in address;

  long int filelen;

  int addrlen = sizeof(address);
  int server_fd, connected_socket_fd, valread, a;
  int opt = 1;
  char buffer[1024] = {0};
  char filename_buffer[1024] = {0};

  char *response;
  long int bytes = 0, bytes_sent = 0;
  unsigned short int body_size = 1024;
  char *stream;
  char header[256];

  int errnum;

  /* Create a new socket of type TCP in an IPV4 domain.
     This socket is represented as a file descriptor  */
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
  {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

    /* Set the reuse of ADDRESSES and PORTS, which means
    that even if the process is restarted, the kernell will
    give to it the same previous address and port if the
    socket is in the TIME_WAIT state.  */
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                 &opt, sizeof(opt)))
  {
    perror("Can't set the options");
    exit(EXIT_FAILURE);
  }

  /*Specifing the type of address to IPV4*/
  address.sin_family = AF_INET;

  /* Setting that the socket need to be bind to all interfaces,
   it means that if you have two network interfaces, 
   you will accept connections and packets sent to those interfaces */
  address.sin_addr.s_addr = INADDR_ANY;

  /* The htons() function converts the unsigned short integer
   hostshort from host byte order to network byte order. */
  address.sin_port = htons(port);

  /* Give the socket server_fd the local address "address" (which is addr bytes long). */
  if (bind(server_fd, (struct sockaddr *)&address,
           sizeof(address)) < 0)
  {
    perror("Fail to bind address to socket file descriptor");
    exit(EXIT_FAILURE);
  }

/* Marks the socket referred to by server_fd as a passive
   socket, that is, as a socket that will be used to accept incoming
   connection requests using accept(2).

   The server_fd argument is a file descriptor that refers to a socket
   of type SOCK_STREAM or SOCK_SEQPACKET.

   The backlog argument set to 3 defines the maximum length to which the
   queue of pending connections for sockfd may grow.  If a
   connection request arrives when the queue is full, the client may
   receive an error with an indication of ECONNREFUSED or, if the
   underlying protocol supports retransmission, the request may be
   ignored so that a later reattempt at connection succeeds. */
if (listen(server_fd, 3) < 0)
  {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  /* The accept() system call is used with connection-based socket
     types (SOCK_STREAM, SOCK_SEQPACKET).  It extracts the first
     connection request on the queue of pending connections for the
     listening socket, sockfd, creates a new connected socket, and
     returns a new file descriptor referring to that socket.  The
     newly created socket is not in the listening state.  The original
     socket sockfd is unaffected by this call.

     The argument sockfd is a socket that has been created with
     socket(2), bound to a local address with bind(2), and is
     listening for connections after a listen(2). See more details
     in accept(2) man page*/
  if ((connected_socket_fd = accept(server_fd, (struct sockaddr *)&address,
                           (socklen_t *)&addrlen)) < 0)
  {
    perror("accept");
    exit(EXIT_FAILURE);
  }

  /* Read 1024 bytes into filename_buffer from connected_socket_fd */
  valread = read(connected_socket_fd, filename_buffer, 1024);

  response = file_finder(filename_buffer, path);

  f = fopen(strcat(strcat(path,"/"),response), "rb");

  a = fseek(f, 0, SEEK_END);

  // The C library function long int ftell(FILE *stream) returns the current file position of the given stream.
  filelen = ftell(f);

  // The C library function void rewind(FILE *stream) sets the file position to the beginning of the file of the given stream.
  rewind(f);

  // buffer = (char *)malloc(filelen * sizeof(char));

  if (body_size > filelen)
  {
    body_size = filelen;
  }
  while (bytes < filelen)
  {
    stream = malloc((body_size + 256) * sizeof(char)); // array to hold the result
    bzero(stream, 256 + body_size);

    fread(buffer, body_size, 1, f);

    sprintf(header, "%d", body_size);
    memcpy(stream, header, 256);
    memcpy(stream + 256, buffer, body_size);

    bytes_sent = send(connected_socket_fd, stream, body_size + 256, 0);

    if (bytes_sent == -1){

      errnum = errno;
      fprintf(stderr, "Value of errno: %d\n", errno);
      perror("Error printed by perror");
      fprintf(stderr, "The error is: %s\n", strerror(errnum));
    }
    bytes_sent = bytes_sent - 256;

    bytes = bytes + bytes_sent;

    fseek(f, bytes, SEEK_SET);
    printf("Bytes sent: %ld\n", bytes);
    if ((filelen - bytes) < body_size)
    {
      body_size = filelen - bytes;
    }
  }
  fclose(f);
  close(server_fd);
  return 0;
}