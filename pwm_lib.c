#include "pwm_lib.h"

static int pwm_write(const char *subpath, const char *value) 
{
    char full_path[256];
    snprintf(full_path, sizeof(full_path), "%s/%s", PWM_PATH, subpath);

    FILE *file = fopen(full_path, "w");
    if (file == NULL) {
        perror("Error al abrir el archivo de PWM");
        return -1;
    }

    fprintf(file, "%s", value);
    fclose(file);
    return 0;
}

int pwm_set_period(unsigned long period_ns) 
{
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%lu", period_ns);
    return pwm_write("period", buffer);
}

int pwm_set_duty_cycle(unsigned long duty_ns) 
{
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%lu", duty_ns);
    return pwm_write("duty_cycle", buffer);
}

int pwm_set_enable(int enable) 
{
    char buffer[8];
    snprintf(buffer, sizeof(buffer), "%d", enable);
    return pwm_write("enable", buffer);
}
