#include <methylish.h>

char *getCommandFromUserConsole(void) {
    char *command = malloc(256);
    if(command == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    if(fgets(command, 256, stdin) != NULL) {
        size_t len = strlen(command);
        if(len > 0 && command[len - 1] == '\n') command[len - 1] = '\0';
    }
    else {
        free(command);
        return NULL;
    }
    return command;
}

void runThatCommand(void) {
    char buffer[1024];
    char *commandFromConsole;
    printf("\e[1;32m%s\e[0m:\e[1;34m%s | \e[1;31mmethylish\e[0m> ", HostAndUserName, getcwd(buffer, sizeof(buffer)));
    commandFromConsole = getCommandFromUserConsole();
    if(commandFromConsole == NULL) return;
    if(strcmp(commandFromConsole, "exit") == 0) {
        printf("Exiting methylish shell. Goodbye!\n");
        free(commandFromConsole);
        exit(lastStatus);
    }
    else if(strncmp(commandFromConsole, "cd", 2) == 0) {
        char *path = commandFromConsole + 2;
        while(*path == ' ') path++;
        if(*path == '\0') path = getenv("HOME");
        if(chdir(path) != 0) perror("Failed to change directory");
    }
    else if(strncmp(commandFromConsole, "println", 7) == 0) {
        char *text = commandFromConsole + 7;
        while(*text == ' ') text++;
        if(*text == '"' || *text == '\'') {
            char quote = *text;
            text++;
            char *end = strrchr(text, quote);
            if(end) *end = '\0';
        }
        printf("%s\n", text);
    }
    else if(strncmp(commandFromConsole, "print", 5) == 0) {
        char *text = commandFromConsole + 5;
        while(*text == ' ') text++;
        if(*text == '"' || *text == '\'') {
            char quote = *text;
            text++;
            char *end = strrchr(text, quote);
            if(end) *end = '\0';
        }
        printf("%s", text);
    }
    else if(strcmp(commandFromConsole, "lastReturnCode") == 0) printf("%d\n", lastStatus);
    else if(strncmp(commandFromConsole, "clear", 5) == 0 || strncmp(commandFromConsole, "cls", 3) == 0) printf("\033c\n");
    else {
        switch(fork()) {
            case -1:
                exit(EXIT_FAILURE);
            case 0:
                execl("/bin/bash", "bash", "-c", commandFromConsole, (char *)NULL);
                exit(EXIT_FAILURE);
            default: {
                int status;
                wait(&status);
                if(WIFEXITED(status)) lastStatus = WEXITSTATUS(status);
                else lastStatus = -1;
            }
        }
    }
    free(commandFromConsole);
}