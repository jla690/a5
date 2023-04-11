#include "pwm.h"
#include "utils.h"
#include "stdbool.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static enum Sound sound = None;
static bool soundPlaying = false;

static void setPWMStatus(bool on) {
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

    setPWMStatus(false);
    setPWMSound(periodValueStr, dutyValueStr);
    setPWMStatus(true);
}

static void playHit() {
    sound = None;
    for(int i = 1; i < 10; i++) {
        playFrequency(i * 100, 0.5);
        sleepForMs(80);
        if(!soundPlaying) {
            setPWMStatus(false);
            return;
        }
    }
    sleepForMs(500);
    setPWMStatus(false);
}

static void playMiss() {
    sound = None;
    for(int i = 8; i > 0; i--) {
        playFrequency(i * 500, 0.5);
        sleepForMs(40);
        if(!soundPlaying) {
            setPWMStatus(false);
            return;
        }
    }
    sleepForMs(500);
    setPWMStatus(false);
}

void * PWM_buzzerThread(void * args) {

    while(!stopped) {
        if(sound == None) {
            sleepForMs(100);
        }
        if(sound == Hit) {
            soundPlaying = true;
            playHit();
        }
        if(sound == Miss) {
            soundPlaying = true;
            playMiss();
        }
    }

    return 0;
}

void PWM_playSound(int newSound) {
    sound = newSound;
    soundPlaying = false;
}