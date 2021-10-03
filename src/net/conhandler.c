#include "conhandler.h"
#include <unistd.h>

// int conhandler(int socket_con_accepted, char *path_ch)
void * conhandler(void *args)
{
  char *path_ch = ((handargs *)args)->path_ch;
  int socket_con_accepted = ((handargs *)args)->socket_con_accepted;
  
  args = NULL;
  free(args);
  int valread;
  char fnbuffer[1024] = {0};
  /* Read 1024 bytes into fnbuffer from socket_con_accepted */
  valread = read(socket_con_accepted, fnbuffer, 1024);

  char *dirname =(char *)malloc(1024);
  strcpy(dirname, path_ch);

  /*Find the file in the directory, returning the filename if file
    is inside directory or return NULL if the file isn't in the
    directory.*/
  char *response;
  response = filefinder(fnbuffer, dirname);

  if(response == NULL){
    return NULL;
  }

  /*Open a file and create a new stream for it, the path is composed by
  the path passed in the command line and the file name*/
  FILE *filestream;
  filestream = fopen(strcat(strcat(dirname,"/"),response), "rb");

  /*The fseek() function sets the file position indicator for the
    stream pointed to by stream.  The new position, measured in
    bytes, is obtained by adding offset bytes to the position
    specified by whence.  If whence is set to SEEK_SET, SEEK_CUR, or
    SEEK_END, the offset is relative to the start of the file, the
    current position indicator, or end-of-file, respectively.  A
    successful call to the fseek() function clears the end-of-file
    indicator for the stream and undoes any effects of the ungetc(3)
    function on the same stream.*/
  int finalposition;
  finalposition = fseek(filestream, 0, SEEK_END);

  /*The ftell() function obtains the current value of the file
    position indicator for the stream pointed to by stream.*/
  long int filelen;
  filelen = ftell(filestream);

  /*The rewind() function sets the file position indicator for the
    stream pointed to by stream to the beginning of the file.*/
  rewind(filestream);

  unsigned short int body_size = BODY_SIZE;
  long int bytessent = 0;
  long int bytes = 0;
  char buffer[1024] = {0};
  char header[HEADER_SIZE];
  char *stream;

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
    bytessent = send(socket_con_accepted, stream, body_size + HEADER_SIZE, 0);

    if (bytessent == -1){
      fprintf(stderr, "An error occurred: %s\n", strerror(errno));
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

  /*Close socket socket_con_accepted*/
  close(socket_con_accepted);

  printf("File %s uploaded with success!\n", fnbuffer);
  fflush(stdout);
  bzero(fnbuffer, 1024);
  bytes = 0;
  bytessent = 0;
  body_size = BODY_SIZE;
  
  return NULL;
}