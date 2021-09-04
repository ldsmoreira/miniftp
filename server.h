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
  long int filelen;
  int a;
  int server_fd, new_socket, valread;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};
  char buffer2[1024] = {0};

  char *response;
  long int bytes = 0, bytes_sent = 0;
  unsigned short int body_size = 1024;
  char *stream;
  char header[256];

  int errnum;

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
  {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Forcefully attaching socket to the port 8080
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                 &opt, sizeof(opt)))
  {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  // Forcefully attaching socket to the port 8080
  if (bind(server_fd, (struct sockaddr *)&address,
           sizeof(address)) < 0)
  {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  if (listen(server_fd, 3) < 0)
  {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                           (socklen_t *)&addrlen)) < 0)
  {
    perror("accept");
    exit(EXIT_FAILURE);
  }

  valread = read(new_socket, buffer2, 1024);

  response = file_finder(buffer2, path);

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

    bytes_sent = send(new_socket, stream, body_size + 256, 0);

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