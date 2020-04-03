/* !!! PROVIDED FILE, DO NOT MODIFY !!!
 *
 * An unmodified version of this file will be used during evaluation.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

/* Copies the standard input to the standard output - NO MATTER WHAT THEY ARE!
 */
void copyFile()
{
   int c;

   while (EOF != (c = getchar()))
      putchar(c);
}

/* Opens the specified file and returns its file descriptor so you don't have
 * too - your welcome!
 */
int openFile(const char *fileName, const char *mode)
{
   int fd, flags;

   if (0 == strcmp("r", mode))
      flags = O_RDONLY;
   else if (0 == strcmp("w", mode))
      flags = O_WRONLY | O_CREAT | O_TRUNC;
   else {
      fprintf(stderr, "Unknown openFile mode %s\n", mode);
      exit(EXIT_FAILURE);
   }

   if (-1 == (fd = open(fileName, flags, 0666)))
   {
      fprintf(stderr, "cshell: ");
      perror(fileName);
      exit(EXIT_FAILURE);
   }

   return fd;
}
