#include "thread_control.h"

#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "accelerometer.h"
#include "utils.h"
#include "sharedMem-Linux.h"
pthread_t accelerometer;
pthread_t threadControl;
pthread_t LED;
pthread_t Joystick;

void initThreads()
{
    pthread_create(&threadControl, NULL, (void*)threadControlThread, NULL);
    pthread_join(threadControl, NULL);
}

void* threadControlThread(void* arg)
{
    pthread_create(&accelerometer, NULL, accelerometerThread, NULL);
    pthread_create(&LED, NULL, LEDThread, NULL);
    pthread_create(&Joystick, NULL, JoystickThread, NULL);
    return 0;
}

void stopThreads()
{
    stopped = 1;
    pthread_join(accelerometer, NULL);
    pthread_join(LED, NULL);
    pthread_join(Joystick, NULL);
}