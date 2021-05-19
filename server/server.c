// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <dirent.h>

#define NORMAL_COLOR "\x1B[0m"
#define GREEN "\x1B[32m"
#define BLUE "\x1B[34m"

#define PORT 9000

char *fileFinder(char fileToSearch[1024])
{
  struct dirent *de; // Ponteiro para a entrada do diretorio

  char newLineChar = '\n';
  char *newLinePtr = &newLineChar;

  // newLinePtr = &newLineChar;

  DIR *dr = opendir(".");

  if (dr == NULL) // opendir retorna NULL se nao conseguir abrir o diretorio
  {
    printf("Nao foi possivel abrir o diretoriao!");
    return 0;
  }

  while ((de = readdir(dr)) != NULL)
  {

    fileToSearch[strcspn(fileToSearch, newLinePtr)] = '\0';
    printf("%s\n", BLUE, de->d_name);
    if (strcmp(fileToSearch, de->d_name) == 0)
    {
      printf("%s\n", de->d_name);
      printf("Arquivo encontrado\n");
      // return de->d_name;
      return fileToSearch;
    }
    // printf("Arquivo nao encontrado!");
  }

  closedir(dr);
  return "Arquivo nao encontrado";
}

int main(int argc, char const *argv[])
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
  int bytes = 0;
  unsigned short int body_size = 1024;
  char *stream;
  char header[256];

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
  address.sin_port = htons(PORT);

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

  response = fileFinder(buffer2);

  f = fopen(response, "rb");

  a = fseek(f, 0, SEEK_END);

  // The C library function long int ftell(FILE *stream) returns the current file position of the given stream.
  filelen = ftell(f);

  // The C library function void rewind(FILE *stream) sets the file position to the beginning of the file of the given stream.
  rewind(f);

  // buffer = (char *)malloc(filelen * sizeof(char));

  printf("%s\n", buffer);
  printf("%s\n", buffer2);
  printf("oiiiiiii");
  if (body_size > filelen)
  {
    body_size = filelen;
  }
  while (bytes < filelen)
  {
    stream = malloc((body_size + 256) * sizeof(char)); // array to hold the result
    bzero(stream, 256 + 1024);
    // printf("%ld\n", sizeof(body_size));
    // body_size_ptr = (unsigned short int *)malloc(body_size * sizeof(char));
    // *body_size_ptr = body_size;
    fread(buffer, body_size, 1, f);

    sprintf(header, "%d", body_size);
    memcpy(stream, header, 256);
    memcpy(stream + 256, buffer, body_size);

    // memcpy(stream, body_size_ptr, sizeof(body_size));
    // memcpy(stream + 256, buffer, body_size * sizeof(char));

    bytes = bytes + (send(new_socket, stream, body_size + 256, 0) - 256);

    fseek(f, bytes, SEEK_SET);
    printf("Bytes sent: %d\n", bytes);
    if ((filelen - bytes) < body_size)
    {
      body_size = filelen - bytes;
    }
  }
  fclose(f);
  close(server_fd);
  printf("Hello message sent\n");
  return 0;
}
