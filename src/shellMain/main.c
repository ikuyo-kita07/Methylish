#include <methylish.h>

// last return status of the commands:
int lastStatus = 0;

// Host and User Name
char *HostAndUserName = NULL;

int main() {
    HostAndUserName = malloc(500);
    if(HostAndUserName == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    FILE *fp = popen("echo $(whoami)@$(hostname)", "r");
    if(fp == NULL) {
        free(HostAndUserName);
        exit(EXIT_FAILURE);
    }
    if(fgets(HostAndUserName, 500, fp) != NULL) {
        size_t len = strlen(HostAndUserName);
        if(len > 0 && HostAndUserName[len - 1] == '\n') HostAndUserName[len - 1] = '\0';
    }
    pclose(fp);
    while(true) runThatCommand();
    return lastStatus;
}