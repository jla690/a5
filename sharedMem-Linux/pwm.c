#include "pwm.h"
#include "utils.h"
#include "stdbool.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static enum Sound sound = None;
static bool soundPlaying = false;
pthread_mutex_t PWM_lock;

void PWM_setStatus(bool on) {
    if(on) {
        writeToFile(PWM_ENABLE, "1");
    } else {
        writeToFile(PWM_ENABLE, "0");
    }
}

static void setPWMSound(char * period, char * duty) {
    writeToFile(PERIOD_PATH, period);
    writeToFile(DUTY_CYCLE_PATH, duty);
}


static void playFrequency(float frequency, float ratio) {
    int periodValue = (1 / frequency) * 1000000000;
    char periodValueStr[32];
    char dutyValueStr[32];

    //learn how to find digits of an int from https://stackoverflow.com/a/8257728
    int periodLength = (int)((ceil(log10(periodValue))+1)*sizeof(char));
    snprintf(periodValueStr, periodLength, "%d", periodValue);

    int dutyLength = (int)((ceil(log10(periodValue/2))+1)*sizeof(char));
    snprintf(dutyValueStr, dutyLength, "%d", (int)(periodValue * ratio));

    PWM_setStatus(false);
    setPWMSound(periodValueStr, dutyValueStr);
    PWM_setStatus(true);
}

static void playHit() {
    sound = None;
    soundPlaying = true;
    pthread_mutex_unlock(&PWM_lock);
    for(int i = 1; i < 10; i++) {
        playFrequency(i * 100, 0.5);
        sleepForMs(80);
        if(!soundPlaying) {
            PWM_setStatus(false);
            return;
        }
    }
    sleepForMs(500);
    PWM_setStatus(false);
    soundPlaying = false;
}

static void playMiss() {
    sound = None;
    soundPlaying = true;
    pthread_mutex_unlock(&PWM_lock);
    for(int i = 8; i > 0; i--) {
        playFrequency(i * 500, 0.5);
        sleepForMs(40);
        if(!soundPlaying) {
            PWM_setStatus(false);
            return;
        }
    }
    sleepForMs(500);
    PWM_setStatus(false);
    soundPlaying = false;
}

bool PWM_getSound() {
    return soundPlaying;
}

void * PWM_buzzerThread(void * args) {

    pthread_mutex_init(&PWM_lock, NULL);

    while(!stopped) {
        pthread_mutex_lock(&PWM_lock);
        if(sound == None) {
            pthread_mutex_unlock(&PWM_lock);
            sleepForMs(100);
        }
        if(sound == Hit) {
            playHit();
        }
        if(sound == Miss) {
            playMiss();
        }
    }
    pthread_mutex_destroy(&PWM_lock);
    return 0;
}

void PWM_playSound(int newSound) {
    pthread_mutex_lock(&PWM_lock);
    sound = newSound;
    soundPlaying = false;
    pthread_mutex_unlock(&PWM_lock);
}