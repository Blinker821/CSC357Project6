#include "logic.h"
#include "copyFile.h"

int main(int argc, char *argv[])
{
   char line[MAX_LINE_LENGTH+1];
   Stage processes[20];
   setbuf(stdout, NULL);
   while(runLoop(line, processes) != EOF)
   return 0;
}
