#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define HEADER_SIZE 256
#define BODY_SIZE 1024 

extern int errno ;

int client(char *filename, char *host, int port)
{
  int client_fd;
  /* Create a new socket of type TCP in an IPV4 domain.
     This socket is represented as a file descriptor  */
  if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("\n Socket creation error \n");
    return -1;
  }

  /* Structure describing an Internet socket address.  */
  struct sockaddr_in address;
  /*Specifing the type of address to IPV4*/
  address.sin_family = AF_INET;

  /* The htons() function converts the unsigned short integer
  hostshort from host byte order to network byte order. */
  address.sin_port = htons(port);

  /*Convert from presentation format of an Internet number in buffer
  starting at CP to the binary network format and store result for
  interface type AF in buffer starting at BUF.*/
  if (inet_pton(AF_INET, host, &address.sin_addr) <= 0)
  {
    printf("\nInvalid address/ Address not supported \n");
    return -1;
  }

  /*Open a connection on socket client_fd to peer at (struct sockaddr *)&address.
  (which sizeof(address) bytes long).
  For connectionless socket types, just set the default address to send to
  and the only address from which to accept transmissions.
  Return 0 on success, -1 for errors*/
  if (connect(client_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
  {
    printf("Connection Failed \n");
    return -1;
  }

  // send(sock, file, BODY_SIZE, 0);
  // printf("%s", strcpy(filebuffer, filename));

  /*Send over the connection the name of the file to be downloaded*/
  char filebuffer[BODY_SIZE] = {0};
  send(client_fd, strcpy(filebuffer, filename), sizeof(filebuffer), 0);

  FILE *filestream;
  filestream = fopen(filename, "wb+");


  /*Pointer to an array of HEADER_SIZE characters*/
  char(*header_ptr)[HEADER_SIZE];
  char stream[BODY_SIZE + HEADER_SIZE] = {0};
  unsigned long int stream_size;
  size_t bytes = 0;
  ssize_t bytesread;
  do{

   /*Read NBYTES into stream from client_fd. Return the
     number read, -1 for errors or 0 for EOF.*/
     bytesread = read(client_fd, stream, BODY_SIZE + HEADER_SIZE);

     if (bytesread == -1){

      fprintf(stderr, "Value of errno: %d\n", errno);
      perror("Error: ");
    }

    if(bytesread < (HEADER_SIZE + BODY_SIZE)){
      bytesread = bytesread + read(client_fd, stream + bytesread, (HEADER_SIZE + BODY_SIZE) - bytesread);
    }

    /*Use the header_ptr to point for the header part of the stream*/
    header_ptr = &stream;

    /*Determing how is the size of the stream dynamicaly using the the
    header sent by the server*/
    stream_size = atol(*header_ptr);

    /*Write bytes the the file and update the current position of the file by
    the number of bytes already written*/
    bytes = bytes + fwrite(stream + HEADER_SIZE, stream_size, 1, filestream) * stream_size;
    fseek(filestream, bytes, SEEK_SET);


    /*Turn stream a zero valued array*/
    bzero(stream, (HEADER_SIZE + BODY_SIZE));

  }while (bytesread > 0);

  printf("Bytes received: %ld\n", bytes);
  printf("File %s downloaded\n", filename);
  fflush(stdout);
  fclose(filestream);
  close(client_fd);
  return 0;
}
