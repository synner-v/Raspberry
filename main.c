#include <stdio.h>
#include "pwm_lib.h"

int main(void) {
    unsigned long periodo = 10000000; 
    unsigned long duty = 5000000;     

    printf("Configurando PWM...\n");

    if (pwm_set_period(periodo) != 0) {
        return 1;
    }

    if (pwm_set_duty_cycle(duty) != 0) {
        return 1;
    }

    if (pwm_set_enable(1) != 0) {
        return 1;
    }

    printf("PWM configurado exitosamente al 50%% de ciclo de trabajo a 100Hz\n");
    return 0;
}
