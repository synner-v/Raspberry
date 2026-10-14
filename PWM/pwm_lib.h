#ifndef PWM_LIB_H
#define PWM_LIB_H

#include <stdio.h>
#include <stdlib.h>

#define PWM_PATH "/sys/class/pwm/pwmchip0/pwm2"

int pwm_set_period(unsigned long period_ns);
int pwm_set_duty_cycle(unsigned long duty_ns);
int pwm_set_enable(int enable);

#endif 
