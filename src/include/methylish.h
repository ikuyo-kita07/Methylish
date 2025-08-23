#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// global variables
extern int lastStatus;
extern char *HostAndUserName;

// Functions:
char *getCommandFromUserConsole();
void runThatCommand(void);