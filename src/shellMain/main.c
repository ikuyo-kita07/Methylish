#include <methylish.h>

// last return status of the commands:
int lastStatus = 0;

// Host and User Name
char *HostAndUserName = NULL;

// logfile:
const char *logfile = "methylish.log";

// throws logs to STD OUT/ERR when set to true:
bool throwLogsToSTD = false;

int main() {
    FILE *clearTheLogFile = fopen(logfile, "w");
    if(clearTheLogFile) fclose(clearTheLogFile);
    MethylishLog(INFO, "init: Methylish started!!");
    HostAndUserName = malloc(100);
    if(HostAndUserName == NULL) exit(EXIT_FAILURE);
    FILE *fp = popen("echo $(whoami)@$(hostname)", "r");
    if(fp == NULL) {
        free(HostAndUserName);
        exit(EXIT_FAILURE);
    }
    if(fgets(HostAndUserName, 100, fp) != NULL) {
        size_t len = strlen(HostAndUserName);
        if(len > 0 && HostAndUserName[len - 1] == '\n') HostAndUserName[len - 1] = '\0';
    }
    pclose(fp);
    while(true) runThatCommand();
    if(HostAndUserName) safeClean((void**)&HostAndUserName);
    return lastStatus;
}