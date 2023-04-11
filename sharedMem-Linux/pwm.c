#include "pwm.h"
#include "utils.h"
#include "stdbool.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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


static void playFrequency(float frequency) {
    int periodValue = (1 / frequency) * 1000000000;
    char periodValueStr[32];
    char dutyValueStr[32];

    //learn how to find digits of an int from https://stackoverflow.com/a/8257728
    int periodLength = (int)((ceil(log10(periodValue))+1)*sizeof(char));
    snprintf(periodValueStr, periodLength, "%d", periodValue);
    printf("%d = %s\n", periodValue, periodValueStr);

    int dutyLength = (int)((ceil(log10(periodValue/2))+1)*sizeof(char));
    snprintf(dutyValueStr, dutyLength, "%d", (int)periodValue/2);
    printf("%d = %s\n", periodValue/2, dutyValueStr);

    setPWMStatus(false);
    setPWMSound(periodValueStr, dutyValueStr);
    setPWMStatus(true);
}

static void playHit() {
    //play note
    //sleep
    //if sound playing == false call setPWMState(false) and return
    //else play next note
    //repeat until done
    sound = None;
    playFrequency(261.6);
    sleepForMs(100);
    setPWMStatus(false);
    stopped = 1;
}

static void playMiss() {
    printf("misss");
}

void * PWM_buzzerThread(void * args) {

    while(!stopped) {
        if(sound == None) {
            sleepForMs(1);
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