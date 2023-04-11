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
#include "display.h"
#include "pwm.h"
pthread_t accelerometer;
pthread_t threadControl;
pthread_t LED;
pthread_t Joystick;
pthread_t display;
pthread_t displayUpdate;
pthread_t Buzzer;

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
    pthread_create(&display, NULL, displayThreadFunction, NULL);
    pthread_create(&displayUpdate, NULL, updateDigitsThread, NULL);
    pthread_create(&Buzzer, NULL, PWM_buzzerThread, NULL);
    return 0;
}

void stopThreads()
{
    stopped = 1;
    pthread_join(accelerometer, NULL);
    pthread_join(LED, NULL);
    pthread_join(Joystick, NULL);
    pthread_join(display, NULL);
    pthread_join(displayUpdate, NULL);
    pthread_join(Buzzer, NULL);
}