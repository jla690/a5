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

    } else {
        
    }
}


static void playFrequency(float frequency) {
    char period[64] = "echo ";
    char duty[64] = "echo ";
    char dutyCycle;
    int periodValue = (1 / frequency) * 1000000000;
    char periodValueStr[32];
    char dutyValueStr[32];

    //learn how to find digits of an int from https://stackoverflow.com/a/8257728
    int periodLength = (int)((ceil(log10(periodValue))+1)*sizeof(char));
    snprintf(periodValueStr, periodLength, "%d", periodValue);
    printf("%d = %s\n", periodValue, periodValueStr);
    strncat(period, periodValueStr, periodLength);
    strcat(period, PERIOD_PATH);

    int dutyLength = (int)((ceil(log10(periodValue/2))+1)*sizeof(char));
    snprintf(dutyValueStr, dutyLength, "%d", periodValue/2);
    printf("%d = %s\n", periodValue/2, dutyValueStr);
    strncat(duty, dutyValueStr, dutyLength);
    strcat(duty, DUTY_CYCLE_PATH);

    runCommand("echo 0 > /dev/bone/pwm/0/a/enable");
    runCommand(period);
    runCommand(duty);
    runCommand("echo 1 > /dev/bone/pwm/0/a/enable");
}

static void playHit() {

}

static void playMiss() {

}

void * PWM_buzzerThread(void * args) {

    while(!stopped) {
        if(sound == None) {
            sleepForMs(1);
        }
        if(sound == Hit) {
            playHit();
        }
        if(sound == Miss) {
            playMiss();
        }
    }

    return 0;
}

void PWM_playSound(enum Sound newSound) {
    sound = newSound;
    soundPlaying = false;
}