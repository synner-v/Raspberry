#include <stdio.h>
#include <unistd.h>
#include "pwm.h"

int ActualPeriod = 0;
int ActualDuty = 0;

int escribir(char *ruta, int valor)
{
    FILE *file = fopen(ruta, "w");

    if (file == NULL)
    {
        perror(ruta);
        return -1;
    }
    fprintf(file, "%d", valor);
    fclose(file);
    return 0;
}

int pwm_init(int period_ns, int duty_ns)
{
    escribir(PWM_CHIP_PATH "/export", 0);
    usleep(100000);

    if (escribir(PWM_PATH "/period", period_ns) < 0)
    {
        return -1;
    }
    ActualPeriod = period_ns;

    if (pwm_set_duty(duty_ns) < 0)
    {
        return -1;
    }

    return pwm_enable(1);
}

int pwm_set_duty(int duty_ns)
{
    if (duty_ns > ActualPeriod)
    {
        duty_ns = ActualPeriod;
    }

    if (escribir(PWM_PATH "/duty_cycle", duty_ns) < 0)
    {
        return -1;
    }
    ActualDuty = duty_ns;
    return 0;
}

int pwm_set_duty_percent(int percent)
{
    if (percent < 0)
    {
        percent = 0;
    }
    if (percent > 100)
    {
        percent = 100;
    }

    return pwm_set_duty(ActualPeriod / 100 * percent);
}

int pwm_get_duty_percent(void)
{
    if (ActualPeriod == 0)
    {
        return 0;
    }

    return ActualDuty / (ActualPeriod / 100);
}

int pwm_enable(int enable)
{
    return escribir(PWM_PATH "/enable", enable);
}

void pwm_close(void)
{
    pwm_enable(0);
    escribir(PWM_CHIP_PATH "/unexport", 0);
}