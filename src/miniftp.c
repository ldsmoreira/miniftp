#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include "net/server.h"
#include "net/client.h"

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PATH "."
#define DEFAULT_PORT 9000

extern int errno;

int client(char *file, char *host, int port);
int server(char *path, int port);

int main (int argc, char **argv)
{
  char *host, *path, *file;
  char *fvalue = NULL;
  char *pvalue = NULL;
  char *rvalue = NULL;
  char *dvalue = NULL;
  int index, option, port;
  int sflag = 0;
  int cflag = 0;

  opterr = 0;

  while ((option = getopt(argc, argv, "scf:p:r:d:")) != -1)
    switch (option)
    {
      case 's':
        sflag = 1;
        break;
      case 'c':
        cflag = 1;
        break;
      case 'f':
        fvalue = optarg;
        break;
      case 'p':
        pvalue = optarg;
        break;
      case 'r':
        rvalue = optarg;
        break;
      case 'd':
        dvalue = optarg;
        break;
      case '?':
        if (isprint(optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
              "Unknown option character `\\x%x'.\n", optopt);
        return 1;
      default:
        abort();
    }

  for (index = optind; index < argc; index++)
    printf ("Non-option argument %s\n", argv[index]);

  if(cflag){
    file = fvalue;
    if(rvalue){
      host = rvalue;
    }else{
      host = DEFAULT_HOST;
    }
    if(pvalue){
      port = atoi(pvalue);
    }else{
      port = DEFAULT_PORT;
    }
    client(file, host, port);

  }else if (sflag)
  {
    if(dvalue){
      path = dvalue;
    }else{
      path = DEFAULT_PATH;
    }
    if(pvalue){
      port = atoi(pvalue);
    }else{
      port = DEFAULT_PORT;
    }
    printf("Starting server at %s\n", path);
    server(path, port);
  }
}