#include <methylish.h>

char *getCommandFromUserConsole(void) {
    char *command = (char *)malloc(256 * sizeof(char));
    if(command == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    printf("methylish> ");
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
    char *commandFromConsole = getCommandFromUserConsole();
    if(commandFromConsole == NULL) {
        printf("methylish> ");
        return;
    }
    if(strcmp(commandFromConsole, "exit") == 0) {
        printf("Exiting methylish shell. Goodbye!\n");
        free(commandFromConsole);
        exit(0);
    }
    else if(strcmp(commandFromConsole, "lastReturnCode") == 0) {
        printf("%d\n", lastStatus);
    }
    else {
        switch (fork()) {
            case -1:
                exit(EXIT_FAILURE);
            case 0:
                execl("/bin/sh", "sh", "-c", commandFromConsole, (char *)NULL);
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