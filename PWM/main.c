#include <stdio.h>
#include <unistd.h>
#include <gpiod.h>
#include "pwm.h"

#define BUTTON_SUBIR 22
#define BUTTON_BAJAR 27

int main(void)
{
    struct gpiod_chip *chip;
    struct gpiod_line_settings *button_settings;
    struct gpiod_line_config *line_config;
    struct gpiod_line_request *request;

    unsigned int button_subir = BUTTON_SUBIR;
    unsigned int button_bajar = BUTTON_BAJAR;

    int duty = 50; 

    pwm_init(10000000, 0);
    pwm_set_duty_percent(duty);


    chip = gpiod_chip_open("/dev/gpiochip0");

    if (chip == NULL)
    {
        perror("Error al abrir GPIO chip");
        return 1;
    }


    button_settings = gpiod_line_settings_new();

    if (button_settings == NULL)
    {
        perror("Error al crear BUTTON settings");
        gpiod_chip_close(chip);
        return 1;
    }

    gpiod_line_settings_set_direction(button_settings,GPIOD_LINE_DIRECTION_INPUT);
    gpiod_line_settings_set_bias(button_settings,GPIOD_LINE_BIAS_PULL_UP);


    line_config = gpiod_line_config_new();

    if (line_config == NULL)
    {
        perror("Error");
        gpiod_line_settings_free(button_settings);
        gpiod_chip_close(chip);
        return 1;
    }
    if (gpiod_line_config_add_line_settings(line_config,&button_subir,1,button_settings) < 0)
    {
        perror("Error al configurar BUTTON SUBIR");
        gpiod_line_settings_free(button_settings);
        gpiod_chip_close(chip);
        return 1;
    }

    if (gpiod_line_config_add_line_settings(line_config,&button_bajar,1,button_settings) < 0)
    {
        perror("Error al configurar BUTTON BAJAR");
        gpiod_line_settings_free(button_settings);
        gpiod_chip_close(chip);
        return 1;
    }

    request = gpiod_chip_request_lines(chip,NULL,line_config);

    if (request == NULL)
    {
        perror("Error al solicitar GPIO");
        return 1;
    }

    printf("Sistema inicializado. Velocidad: %d%%\n", duty);

    while (1)
    {
        if (gpiod_line_request_get_value(request,button_subir) == GPIOD_LINE_VALUE_INACTIVE)
        {
            if (duty < 100)
            {
                duty = duty + 10;   //Sube 10%
            }

            pwm_set_duty_percent(duty);

            printf("Velocidad: %d%%\n", duty);

            while (gpiod_line_request_get_value(request,button_subir) == GPIOD_LINE_VALUE_INACTIVE)
            {
                usleep(10000);
            }
        }

        if (gpiod_line_request_get_value(request,button_bajar) == GPIOD_LINE_VALUE_INACTIVE)
        {
            if (duty > 0)
            {
                duty = duty - 10;   //Baja 10%
            }

            pwm_set_duty_percent(duty);

            printf("Velocidad: %d%%\n", duty);

            while (gpiod_line_request_get_value(request,button_bajar) == GPIOD_LINE_VALUE_INACTIVE)
            {
                usleep(10000);
            }
        }

        usleep(10000);
    }

    return 0;
}