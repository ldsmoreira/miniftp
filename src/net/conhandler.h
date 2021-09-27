#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include "../utils/filefinder.h"

#define HEADER_SIZE 256
#define BODY_SIZE 1024

int conhandler(int socket_con_accepted, char *path_ch);