#ifndef PWM_H
#define PWM_H

#define PERIOD_PATH "> /dev/bone/pwm/0/a/period"
#define DUTY_CYCLE_PATH "> /dev/bone/pwm/0/a/duty_cycle"

enum Sound {
    None, Hit, Miss
};

void *PWM_buzzerThread(void * args);

void PWM_playSound(enum Sound newSound);

#endif // !PWM_H
