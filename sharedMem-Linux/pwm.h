// PWM class header
#ifndef PWM_H
#define PWM_H

#include <stdbool.h>

// paths for pwm
#define PERIOD_PATH "/dev/bone/pwm/0/a/period"
#define DUTY_CYCLE_PATH "/dev/bone/pwm/0/a/duty_cycle"
#define PWM_ENABLE "/dev/bone/pwm/0/a/enable"

// enum for sounds
enum Sound {
    None, Hit, Miss
};

// thread for pwm
void *PWM_buzzerThread(void * args);

// plays sound
void PWM_playSound(int sound);

// gets sound
bool PWM_getSound();

// sets status
void PWM_setStatus(bool on);

#endif // !PWM_H
