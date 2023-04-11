#ifndef PWM_H
#define PWM_H

#include <stdbool.h>

#define PERIOD_PATH "/dev/bone/pwm/0/a/period"
#define DUTY_CYCLE_PATH "/dev/bone/pwm/0/a/duty_cycle"
#define PWM_ENABLE "/dev/bone/pwm/0/a/enable"

enum Sound {
    None, Hit, Miss
};

void *PWM_buzzerThread(void * args);

void PWM_playSound(int sound);

bool PWM_getSound();

#endif // !PWM_H
