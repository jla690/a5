// header for util functions
#ifndef UTILS_H
#define UTILS_H

#include <pthread.h>

extern int stopped;
extern pthread_mutex_t lock;

#define DEBOUNCE_DELAY 100
#define MAX_LEN 1024

// Sleep for a given number of milliseconds
void readFromFile(char filename[], char buffer[]);

// run a command without output
void runCommand(char* command);

// Sleep for a given number of milliseconds
void sleepForMs(long long delayInMs);

// Write a string to a file
void writeToFile(char filename[], char value[]);

#endif
