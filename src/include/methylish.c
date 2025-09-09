#include <methylish.h>

char *getCommandFromUserConsole(void) {
    char *command = malloc(256);
    if(command == NULL) abortInstance("getCommandFromUserConsole()", "Failed to allocate memory for user input.");
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
        MethylishLog(INFO, "runthatCommand(0)", "Quit methylish shell");
        safeClean((void**)commandFromConsole);
        exit(lastStatus);
    }
    else if(strncmp(commandFromConsole, "cd", 2) == 0) {
        char *path = commandFromConsole + 2;
        while(*path == ' ') path++;
        if(*path == '\0') path = getenv("HOME");
        if(chdir(path) != 0) MethylishLog(ERROR, "runthatCommand(1)", "Failed to change directory");
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
    else if(strncmp(commandFromConsole, "childExec", 9) == 0) {
        char *cmdLine = commandFromConsole + 10;
        char *args[64];
        int argc = 0;
        char *token = strtok(cmdLine, " ");
        while(token != NULL && argc < 63) {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }
        args[argc] = NULL;
        char *scriptFile = args[0];
        pid_t pid = fork();
        if(pid < 0) MethylishLog(ERROR, "runthatCommand(2)", "Failed to fork child process.");
        else if(pid == 0) {
            execv(scriptFile, args);
            MethylishLog(ERROR, "runthatCommand(3)", "Failed to execute %s", scriptFile);
            exit(EXIT_FAILURE);
        }
        else {
            int status;
            waitpid(pid, &status, 0);
            if(WIFEXITED(status)) {
                lastStatus = WEXITSTATUS(status);
                MethylishLog(DEBUG, "runthatCommand(4)", "Script exited with status %d", lastStatus);
            }
        }
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
                abortInstance("runThatCommand(2)", "Failed to fork to run the command");
            case 0:
                execl("/bin/bash", "bash", "-c", commandFromConsole, (char *)NULL);
                abortInstance("runThatCommand(3)", "Failed to run the command");
            default: {
                int status;
                wait(&status);
                if(WIFEXITED(status)) lastStatus = WEXITSTATUS(status);
                else lastStatus = -1;
                if(lastStatus == 127) MethylishLog(ERROR, "runThatCommand(4)", "Unknown command %s", commandFromConsole);
            }
        }
    }
    safeClean((void**)commandFromConsole);
}

void safeClean(void* Pointer) {
    if(Pointer) free(Pointer);
    else MethylishLog(ERROR, "safeClean()", "Failed to clear the memory of the given variable, reason: \"NULL\" ");
}

void abortInstance(const char *service, const char *format, ...) {
    va_list args;
    va_start(args, format);
    MethylishLog(ABORT, service, format);
    va_end(args);
    safeClean((void**)HostAndUserName);
    exit(EXIT_FAILURE);
}

void MethylishLog(enum elogLevel loglevel, const char *service, const char *message, ...) {
    va_list args;
    va_start(args, message);
    FILE *out = NULL;
    bool toFile = false;
    if(throwLogsToSTD) {
        out = stdout;
        if(loglevel == ERROR || loglevel == WARN || loglevel == DEBUG || loglevel == ABORT) out = stderr;
    }
    else {
        out = fopen(logfile, "a");
        if(!out) exit(EXIT_FAILURE);
        toFile = true;
    }
    switch(loglevel) {
        case INFO:
            if(!toFile) fprintf(out, "\033[2;30;47mINFO: ");
            else fprintf(out, "INFO: %s: ", service);
        break;
        case WARN:
            if(!toFile) fprintf(out, "\033[1;33mWARNING: ");
            else fprintf(out, "WARNING: %s: ", service);
        break;
        case DEBUG:
            if(!toFile) fprintf(out, "\033[0;36mDEBUG: ");
            else fprintf(out, "DEBUG: %s: ", service);
        break;
        case ERROR:
            if(!toFile) fprintf(out, "\033[0;31mERROR: ");
            else fprintf(out, "ERROR: %s: ", service);
        break;
        case ABORT:
            if(!toFile) fprintf(out, "\033[0;31mABORT: ");
            else fprintf(out, "ABORT: %s: ", service);
        break;
    }
    vfprintf(out, message, args);
    if(!toFile) fprintf(out, "\033[0m\n");
    else fprintf(out, "\n");
    if(!throwLogsToSTD && out) fclose(out);
    va_end(args);
}

void handle_sigint(int signum) {
    MethylishLog(DEBUG, "handle_sigint(0)", "signum: %d", signum);
    if(signum == 2) MethylishLog(INFO, "handle_sigint(1)", "User force-closed the application.");
    abortInstance("handle_sigint(2)", "Closing the program...");
}