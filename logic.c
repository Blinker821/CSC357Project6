#include "logic.h"

int runLoop(char *line, Stage processes[])
{
   int i = 0, eofStatus = 0, numPipes = 0, status = 0;
   char *curArg = NULL;

   printf(":-) ");
   eofStatus = readLine(line);
   if(eofStatus == EOF && strlen(line) == 0)
   {
      printf("exit\n");
      return EOF;
   }
   if(eofStatus == -2)
      status = EOF;
   curArg = strtok(line, " ");
   if(!strcmp(curArg, EXIT_CASE))
      return EOF;
   while(i < MAX_PIPES && status != EOF &&
      EOF != (status = parseProcess(line, &curArg, processes + i++))
      && curArg != NULL)
      curArg = strtok(NULL, " ");
   if(i == MAX_PIPES)
      tooManyPipesError();

   numPipes = i;
   if(status != EOF)
      execPipeline(processes, numPipes);
   return eofStatus;
}

int readLine(char *line)
{
   int c = getchar(), i = 0;
   while(c != '\n' && c != EOF && i < MAX_LINE_LENGTH)
   {
      line[i++] = c;
      c = getchar();
   }
   line[i] = '\0';
   if(c == EOF)
      return EOF;
   else if(i == MAX_LINE_LENGTH)
   {
      commandTooLongError();
      return -2;
   }
   return i;
}

int parseProcess(char *line, char **curArg, Stage *curStage)
{
   int i = 0;
   curStage->outName = NULL;
   clearArgs(curStage);

   if(*curArg == NULL || !strcmp(*curArg, "|"))
   {
      invalidPipeError();
      return EOF;
   }
   while(*curArg != NULL && strlen(*curArg) == 1 &&
      (*curArg[0] == '<' || *curArg[0] == '>'))
      parseRedirect(line, curArg, curStage);

   curStage->name = *curArg;

   i = processMiddle(line, curArg, curStage);
   if(i == MAX_ARGS)
   {
      tooManyArgsError(curStage);
      return EOF;
   }
   else if(i == 0)
   {
      invalidPipeError();
      return EOF;
   }
   curStage->args[i] = NULL;
   return 0;
}

int processMiddle(char *line, char **curArg, Stage *curStage)
{
   int i = 0;
   while(*curArg != NULL && strcmp(*curArg, "|") && i < MAX_ARGS)
   {
      if(strlen(*curArg) == 1 && (*curArg[0] == '<' || *curArg[0] == '>'))
         parseRedirect(line, curArg, curStage);
      else
      {
         curStage->args[i++] = *curArg;
         *curArg = strtok(NULL, " ");
      }
   }
   return i;
}

int parseRedirect(char *line, char **curArg, Stage *curStage)
{
   if(!strcmp(*curArg, "<"))
   {
      *curArg = strtok(NULL, " ");
      curStage->inName = *curArg;
   }
   else
   {
      *curArg = strtok(NULL, " ");
      curStage->outName = *curArg;
   }
   *curArg = strtok(NULL, " ");
   return 0;
}

int execPipeline(Stage *processes, int numPipes)
{
   int fds[3] = {0, 0, 0}, i = 0;
   for(i = 0; i < numPipes - 1; i++)
      executeProcess(processes[i], fds);
   fds[1] = STDOUT_FILENO;
   executeProcess(processes[i], fds);

   for(i = 0; i < numPipes; i++)
   {
      int status = 0;
      wait(&status);
      if(status == -1)
         fail();
   }
   return 0;
}

int executeProcess(Stage stage, int fds[])
{
   int pid = 0;

   if(fds[1] != STDOUT_FILENO && pipe(fds) == -1)
      fail();
   if(-1 ==(pid = fork()))
      fail();
   else if(pid == 0)
      child(stage, fds);
   else
      parent(stage, fds);
   return 0;
}

int child(Stage stage, int fds[])
{
   if(stage.inName != NULL)
   {
      close(fds[2]);
      fds[2] = openFile(stage.inName, "r");
   }
   if(stage.outName != NULL)
   {
      close(fds[1]);
      fds[1] = openFile(stage.outName, "w");
   }
   if(fds[1] != 1)
      close(fds[0]);
   if(dup2(fds[2], STDIN_FILENO) == -1)
      fail();
   if(dup2(fds[1], STDOUT_FILENO) == -1)
      fail();
   if(execvp(stage.name, stage.args) == -1)
      execFailure(stage);
   return 1;
}

int parent(Stage stage, int fds[])
{
   if(fds[1] != STDIN_FILENO && fds[1] != STDOUT_FILENO)
      close(fds[1]);
   if(fds[2] != STDIN_FILENO && fds[2] != STDOUT_FILENO)
      close(fds[2]);
   fds[2] = fds[0];
   return 0;
}

void clearArgs(Stage *stage)
{
   int i = 0;
   for(i = 0; i < 10; i++)
      stage->args[i] = NULL;
}

void clearIn()
{
   while(getchar() != EOF);
}

void commandTooLongError()
{
   fprintf(stderr, "cshell: Command line too long\n");
   clearIn();
}

void tooManyPipesError()
{
   fprintf(stderr, "cshell: Too many commands\n");
}

void tooManyArgsError(Stage *curStage)
{
   fprintf(stderr, "cshell: %s: Too many arguments\n", curStage->name);
}

void invalidPipeError()
{
   fprintf(stderr, "cshell: Invalid pipe\n");
}

void execFailure(Stage stage)
{
   fprintf(stderr, "cshell: ");
   perror(stage.name);
   close(STDIN_FILENO);
   close(STDOUT_FILENO);
   exit(EXIT_FAILURE);
}

void fail()
{
   perror("Unintended Failure");
   exit(EXIT_FAILURE);
}
