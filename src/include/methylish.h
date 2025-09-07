#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdarg.h>

// global variables
extern int lastStatus;
extern bool throwLogsToSTD;
extern char *HostAndUserName;
extern const char *logfile;

// enums! yes!
enum elogLevel {
    INFO,
    WARN,
    ERROR,
    DEBUG,
    ABORT
};

// Functions:
char *getCommandFromUserConsole(void);
void runThatCommand(void);
void safeClean(void* Pointer);
void abortInstance(const char *service, const char *format, ...);
void MethylishLog(enum elogLevel loglevel, const char *service, const char *message, ...);
void handle_sigint(int signum);