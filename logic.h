#ifndef LOGIC_H
#define LOGIC_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "copyFile.h"

#define MAX_LINE_LENGTH 1024
#define EXIT_CASE "exit"
#define MAX_ARGS 10
#define MAX_PIPES 20

typedef struct Stage
{
   char *name, *args[10], *outName, *inName;
} Stage;

/* main control function, returns -1 if the loop needs to end */
int runLoop(char *line, Stage proccesses[]);

/* reads next line into variable */
int readLine(char *line);

/* parses the arguments in line to a stage struct */
/* returns 0 if normal, EOF if usage error        */
int parseProcess(char *line, char **curArg, Stage *curStage);
int processMiddle(char *line, char **curArg, Stage *curStage);

int execPipeline(Stage *processes, int numPipes);
int parseRedirect(char *line, char **curArg, Stage *curStage);
int executeProcess(Stage stage, int fds[]);
int child(Stage stage, int fds[]);
int parent(Stage stage, int fds[]);
void clearArgs(Stage *stage);
void clearIn();
void commandTooLongError();
void tooManyPipesError();
void tooManyArgsError(Stage *curStage);
void invalidPipeError();
void execFailure(Stage stage);
void fail();

#endif
