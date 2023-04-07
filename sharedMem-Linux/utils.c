#include "utils.h"

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

int stopped = 0;
FILE *readFile = NULL;
FILE *writeFile = NULL;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void readFromFile(char filename[], char buffer[])
{
    readFile = fopen(filename, "r");
    if (readFile == NULL) {
        printf("ERROR OPENING %s.", filename);
        exit(-1);
    }
    fgets(buffer, 1000, readFile);
    fclose(readFile);
    readFile = NULL;
}

void runCommand(char *command)
{
    FILE *pipe = popen(command, "r");
    char commandBuffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(commandBuffer, sizeof(commandBuffer), pipe) == NULL) break;
    }
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0) {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}

void sleepForMs(long long delayInMs)
{
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SEC = 1000 * NS_PER_MS;

    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SEC;
    int nanoseconds = delayNs % NS_PER_SEC;

    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *)NULL);
}

void writeToFile(char filename[], char value[])
{
    writeFile = fopen(filename, "w");
    if (writeFile == NULL) {
        printf("ERROR OPENING %s.", filename);
    }
    int char_written = fputs(value, writeFile);
    if (char_written < 0) {
        printf("ERROR WRITING TO %s.", filename);
    }
    fclose(writeFile);
    writeFile = NULL;
}