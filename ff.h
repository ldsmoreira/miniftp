#include <dirent.h>

#define NORMAL_COLOR "\x1B[0m"
#define GREEN "\x1B[32m"
#define BLUE "\x1B[34m"


char *file_finder(char fileToSearch[1024], char *path)
{
  printf("Arquivo que entra em filefinder: %x", fileToSearch);
  /*DIR is a type representing a directory stream.*/
  DIR *dirstream = opendir(path);

  /*The dirent structure describes an entry in a directory*/
  struct dirent *direntry;

  char newLineChar = '\n';
  char *newLinePtr = &newLineChar;

  /*The opendir() function opens a directory stream corresponding to
    the directory name, and returns a pointer to the directory
    stream.  The stream is positioned at the first entry in the
    directory.*/

  if (dirstream == NULL) // opendir return NULL if can't open the directory path
  {
    printf("Can't open the directory %s", path);
    return 0;
  }

  /*The readdir() function returns a pointer to a dirent structure
    representing the next directory entry in the directory stream
    pointed to by dirstream.  It returns NULL on reaching the end of the
    directory stream or if an error occurred.
    In the glibc implementation, the dirent structure is defined as
    follows:

    struct dirent {
        ino_t          d_ino;       //Inode number
        off_t          d_off;       //Not an offset; see below
        unsigned short d_reclen;    //Length of this record
        unsigned char  d_type;      //Type of file; not supported by all filesystem types
        char           d_name[256]; //Null-terminated filename
        };*/
  while ((direntry = readdir(dirstream)) != NULL)
  { 
    /*Using strcspn to avoid strings \n terminated*/
    // fileToSearch[strcspn(fileToSearch, newLinePtr)] = '\0';

    /*Searching for a file that matches fileToSea*/
    if (strcmp(fileToSearch, direntry->d_name) == 0)
    {
      printf("File found\n");
      return fileToSearch;
    }
  }

  /*Close the directory stream dirstream.
    Return 0 if successful, -1 if not.*/
  closedir(dirstream);
  return NULL;
}
