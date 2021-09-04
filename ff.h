#include <dirent.h>

#define NORMAL_COLOR "\x1B[0m"
#define GREEN "\x1B[32m"
#define BLUE "\x1B[34m"


char *file_finder(char fileToSearch[1024], char *path)
{
  struct dirent *de; // Ponteiro para a entrada do diretorio

  char newLineChar = '\n';
  char *newLinePtr = &newLineChar;

  // newLinePtr = &newLineChar;

  DIR *dr = opendir(path);

  if (dr == NULL) // opendir retorna NULL se nao conseguir abrir o diretorio
  {
    printf("Nao foi possivel abrir o diretoriao!");
    return 0;
  }

  while ((de = readdir(dr)) != NULL)
  {

    fileToSearch[strcspn(fileToSearch, newLinePtr)] = '\0';
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
