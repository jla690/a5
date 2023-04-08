#include "thread_control.h"

#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "accelerometer.h"
#include "utils.h"
pthread_t accelerometer;
pthread_t joystick;
pthread_t threadControl;
pthread_t networkT;
pthread_t uptimeT;
pthread_t accelerometerSoundT;
pthread_t printTerminalT;

void initThreads()
{
    pthread_create(&threadControl, NULL, (void*)threadControlThread, NULL);
    pthread_join(threadControl, NULL);
}

void* threadControlThread(void* arg)
{
    pthread_create(&accelerometer, NULL, accelerometerThread, NULL);
    return 0;
}

void stopThreads()
{
    stopped = 1;
    pthread_join(accelerometer, NULL);
}