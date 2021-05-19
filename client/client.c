// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define PORT 9000

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

  char newLineChar = '\n';
  char *newLinePtr = &newLineChar;
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

  while (read(sock, stream, 1024 + 256) > 0)
  {

    printf("stream read: %s\n", stream);

    header_ptr = &stream;
    stream_size = atol(*header_ptr);

    bytes = bytes + fwrite(stream + 256, stream_size, 1, f) * stream_size;
    fseek(f, bytes, SEEK_SET);

    printf("Stream size: %ld\n", stream_size);
    printf("Bytes received of stream: %ld\n", bytes);
  }
  fclose(f);
  close(sock);

  printf("%s\n", buffer);
  return 0;
}
