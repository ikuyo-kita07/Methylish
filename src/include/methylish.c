#include <methylish.h>

char *getCommandFromUserConsole(void) {
    char *command = malloc(256);
    if(command == NULL) {
        abortInstance("Failed to allocate memory for user input.");
    }
    if(fgets(command, 256, stdin) != NULL) {
        size_t len = strlen(command);
        if(len > 0 && command[len - 1] == '\n') command[len - 1] = '\0';
    }
    else {
        safeClean((void**)command);
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
        MethylishLog(INFO, "runthatCommand(0): Quit methylish shell");
        safeClean((void**)commandFromConsole);
        exit(lastStatus);
    }
    else if(strncmp(commandFromConsole, "cd", 2) == 0) {
        char *path = commandFromConsole + 2;
        while(*path == ' ') path++;
        if(*path == '\0') path = getenv("HOME");
        if(chdir(path) != 0) MethylishLog(ERROR, "runthatCommand(1): Failed to change directory");
        safeClean((void**)path);
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
        safeClean((void**)text);
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
        safeClean((void**)text);
    }
    else if(strcmp(commandFromConsole, "lastReturnCode") == 0 || strcmp(commandFromConsole, "lastReturnStatus") == 0) printf("%d\n", lastStatus);
    else if(strncmp(commandFromConsole, "clear", 5) == 0 || strncmp(commandFromConsole, "cls", 3) == 0) printf("\033c\n");
    else if(strncmp(commandFromConsole, "pwd", 3) == 0 || strncmp(commandFromConsole, "cwd", 3) == 0) {
        char wd[1024];
        printf("%s\n", getcwd(wd, sizeof(wd)));
    }
    else {
        switch(fork()) {
            case -1:
                abortInstance("Failed to fork to run the command");
            case 0:
                execl("/bin/bash", "bash", "-c", commandFromConsole, (char *)NULL);
                abortInstance("Failed to run the command");
            default: {
                int status;
                wait(&status);
                if(WIFEXITED(status)) lastStatus = WEXITSTATUS(status);
                else lastStatus = -1;
                if(lastStatus == 127) MethylishLog(ERROR, "runThatCommand(2): Unknown command %s", commandFromConsole);
            }
        }
    }
    safeClean((void**)commandFromConsole);
}

void safeClean(void* Pointer) {
    if(Pointer) free(Pointer);
    else MethylishLog(ERROR, "safeClean(): Failed to clear the memory of the given variable, reason: \"NULL\" ");
}

void abortInstance(const char* message) {
    MethylishLog(ABORT, message);
    safeClean((void**)HostAndUserName);
    exit(EXIT_FAILURE);
}

void MethylishLog(enum elogLevel Log, const char *message, ...) {
    va_list args;
    va_start(args, message);
    FILE *outputPath;
    if(throwLogsToSTD) {
        if(Log == WARN || Log == ERROR || Log == DEBUG) outputPath = stderr;
        else outputPath = stdout;
        switch(Log) {
            case INFO:
                fprintf(outputPath, "[\e[1;34mINFO\e[0m] ");
            break;
            case WARN:
                fprintf(outputPath, "[\e[1;33mWARNING\e[0m] ");
            break;
            case ERROR:
                fprintf(outputPath, "[\e[1;31mERROR\e[0m] ");
            break;
            case DEBUG:
                fprintf(outputPath, "[\e[1;32mDEBUG\e[0m] ");
            break;
            case ABORT:
                fprintf(outputPath, "[\e[1;35mABORT\e[0m] ");
            break;
            default:
                fprintf(outputPath, "[\e[1;34mUNKNOWN\e[0m] ");
        }
    }
    else {
        outputPath = fopen(logfile, "a");
        if(!outputPath) {
            va_end(args);
            abortInstance("MethylishLog(): Failed to setup MethylishLog function, exiting..");
        }
        switch(Log) {
            case INFO:
                fprintf(outputPath, "INFO: ");
            break;
            case WARN:
                fprintf(outputPath, "WARN: ");
            break;
            case ERROR:
                fprintf(outputPath, "ERROR: ");
            break;
            case DEBUG:
                fprintf(outputPath, "DEBUG: ");
            break;
            case ABORT:
                fprintf(outputPath, "ABORT: ");
            break;
            default:
                fprintf(outputPath, "UNKNOWN: ");
        }
    }
    vfprintf(outputPath, message, args);
    fprintf(outputPath, "\n");
    if(!throwLogsToSTD) fclose(outputPath);
    va_end(args);
}

void handle_sigint(int signum) {
    MethylishLog(DEBUG, "handle_sigint(0): signum: %d", signum);
    if(signum == 2) MethylishLog(INFO, "handle_sigint(1): User force-closed the application.");
    abortInstance("Closing the program...");
}