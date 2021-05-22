#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define PORT 9000

extern int errno ;

int main(int argc, char const *argv[])
{

  int sock = 0, valread;
  struct sockaddr_in serv_addr;

  char(*header_ptr)[256];
  char file[1024] = {0};
  char buffer[1024] = {0};
  char stream[1280] = {0};

  unsigned long int stream_size;
  FILE *f;
  size_t bytes = 0;
  ssize_t bytes_read;
  char newLineChar = '\n';
  char *newLinePtr = &newLineChar;

  int errnum;

  fgets(file, sizeof(file), stdin);

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("\n Socket creation error \n");
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  // Convert IPv4 and IPv6 addresses from text to binary form
  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
  {
    printf("\nInvalid address/ Address not supported \n");
    return -1;
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    printf("\nConnection Failed \n");
    return -1;
  }

  send(sock, file, 1024, 0);
  printf("a %s", buffer);

  file[strcspn(file, newLinePtr)] = '\0';
  f = fopen(file, "wb+");
  printf("%s\n", file);
  do{
     bytes_read = read(sock, stream, 1024 + 256);

     if (bytes_read == -1){

      errnum = errno;
      fprintf(stderr, "Value of errno: %d\n", errno);
      perror("Error printed by perror");
      fprintf(stderr, "The error is: %s\n", strerror(errnum));
    }

    if(bytes_read < 1280){
      bytes_read = bytes_read + read(sock, stream + bytes_read, 1280 - bytes_read);
    }
    // printf("stream read: %s\n", stream);

    header_ptr = &stream;
    stream_size = atol(*header_ptr);

    bytes = bytes + fwrite(stream + 256, stream_size, 1, f) * stream_size;
    fseek(f, bytes, SEEK_SET);

    printf("Stream size: %ld\n", stream_size);
    printf("Bytes received of stream: %ld\n", bytes);
    printf("Bytes read: %ld\n", bytes_read);
    bzero(stream, 1280);
    // bytes_read = read(sock, stream, 1024 + 256);
  }while (bytes_read > 0);

  fclose(f);
  close(sock);

  printf("%s\n", buffer);
  return 0;
}
