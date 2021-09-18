#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include "../utils/ff.h"

#define HEADER_SIZE 256
#define BODY_SIZE 1024

char *filefinder(char fileToSearch[1024], char *dirpath);

int server(char *path, int port)
{
  FILE *filestream;

  /* Structure describing an Internet socket address.  */
  struct sockaddr_in address; 

  long int filelen;

  int addrlen = sizeof(address);
  int server_fd, connected_socket_fd, valread, finalposition;
  int opt = 1;
  char buffer[1024] = {0};
  char fnbuffer[1024] = {0};
  char *dirname = malloc(1024);

  char *response, *dirname_buffer;
  long int bytes = 0, bytessent = 0;
  unsigned short int body_size = BODY_SIZE;
  char *stream;
  char header[HEADER_SIZE];

  int errnum;

  strcpy(dirname, path);

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
  while ((connected_socket_fd = accept(server_fd, (struct sockaddr *)&address,
                           (socklen_t *)&addrlen)) > 0){
  // {
  //   perror("accept");
  //   exit(EXIT_FAILURE);
  // }

  /* Read 1024 bytes into fnbuffer from connected_socket_fd */
  valread = read(connected_socket_fd, fnbuffer, 1024);

  strcpy(path, dirname);

  /*Find the file in the directory, returning the filename if file
    is inside directory or return NULL if the file isn't in the
    directory.*/
  response = filefinder(fnbuffer, path);

  if(response == NULL){
    return -1;
  }

  /*Open a file and create a new stream for it, the path is composed by
  the path passed in the command line and the file name*/
  filestream = fopen(strcat(strcat(path,"/"),response), "rb");

  /*The fseek() function sets the file position indicator for the
    stream pointed to by stream.  The new position, measured in
    bytes, is obtained by adding offset bytes to the position
    specified by whence.  If whence is set to SEEK_SET, SEEK_CUR, or
    SEEK_END, the offset is relative to the start of the file, the
    current position indicator, or end-of-file, respectively.  A
    successful call to the fseek() function clears the end-of-file
    indicator for the stream and undoes any effects of the ungetc(3)
    function on the same stream.*/
  finalposition = fseek(filestream, 0, SEEK_END);

  /*The ftell() function obtains the current value of the file
    position indicator for the stream pointed to by stream.*/
  filelen = ftell(filestream);

  /*The rewind() function sets the file position indicator for the
    stream pointed to by stream to the beginning of the file.*/
  rewind(filestream);

  if (body_size > filelen)
  {
    body_size = filelen;
  }
  while (bytes < filelen)
  {
    stream = malloc((body_size + HEADER_SIZE) * sizeof(char)); // prepare stream to receive the transfer protocol
    bzero(stream, HEADER_SIZE + body_size);                    // turn every byte in stream as 0x00

    fread(buffer, body_size, 1, filestream);                   // read body_size bytes from filestream into buffer
                                                               // byte by byte
    sprintf(header, "%d", body_size);                          // write body_size at header stream as an array of char
    memcpy(stream, header, HEADER_SIZE);                       // copy header to the beggining of the protocol stream
    memcpy(stream + HEADER_SIZE, buffer, body_size);           // copy buffer with the body of the message after header

    /*The system calls send() are used to transmit a message to another socket.*/
    bytessent = send(connected_socket_fd, stream, body_size + HEADER_SIZE, 0);

    if (bytessent == -1){

      errnum = errno;
      fprintf(stderr, "An error occurred: %s\n", strerror(errnum));
    }

    bytessent = bytessent - HEADER_SIZE;

    /*Marker of how many bytes was sent*/
    bytes = bytes + bytessent;

    /*Set the value of bytes sent as offset in filestream*/
    fseek(filestream, bytes, SEEK_SET);

    /*Dynamic set body_size*/
    if ((filelen - bytes) < body_size)
    {
      body_size = filelen - bytes;
    }
  }
  /*Close file stream*/
  fclose(filestream);

  /*Close socket connected_socket_fd*/
  close(connected_socket_fd);

  printf("File %s uploaded with success!", fnbuffer);
  fflush(stdout);
  bzero(fnbuffer, 1024);
  bytes = 0;
  bytessent = 0;
  body_size = BODY_SIZE;
  }
  return 0;
}