#ifndef PWM_H
#define PWM_H

#define PWM_CHIP_PATH "/sys/class/pwm/pwmchip0"
#define PWM_PATH      "/sys/class/pwm/pwmchip0/pwm0"

extern int pwm_init(int period_ns, int duty_ns);
extern int pwm_set_duty(int duty_ns);
extern int pwm_set_duty_percent(int percent);
extern int pwm_get_duty_percent(void);
extern int pwm_enable(int enable);
extern void pwm_close(void);

#endif