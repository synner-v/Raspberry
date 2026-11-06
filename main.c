#include <stdio.h>
#include <pigpio.h>

#define PULSO_MIN  500      // us = 0 grados
#define PULSO_MAX  2500     // us = 180 grados

#define PASO_PAR   10       // grados por tecla (hombro y codo)
#define PASO       3        // grados por tecla (base y pinza)

#define PINZA_MIN  120      // rango permitido de la pinza
#define PINZA_MAX  140

// Pines de cada servo
#define PIN_BASE    22
#define PIN_HOMBRO  23
#define PIN_CODO    24
#define PIN_PINZA   25

/* Prototipos */
void escribir_servo(int pin, int angulo);
void mover_par(int delta);
void mover_base(int delta);
void mover_pinza(int delta);
void mostrar_angulos(void);

// Angulos actuales
int ang_base  = 90;
int ang_par   = 90;     // hombro y codo comparten el mismo valor
int ang_pinza = 130;

int main(void)
{
    char c;
    int salir = 0;

    if (gpioInitialise() < 0)
    {
        printf("Error al iniciar pigpio (usa sudo)\n");
        return 1;
    }

    // Posicion inicial
    escribir_servo(PIN_BASE, ang_base);
    escribir_servo(PIN_HOMBRO, ang_par);
    escribir_servo(PIN_CODO, ang_par);
    escribir_servo(PIN_PINZA, ang_pinza);

    printf("w/d: par sube  s/a: par baja  i/k: base  j/l: pinza  x: salir\n");
    mostrar_angulos();

    while (salir == 0)
    {
        // Lee el primer caracter y descarta el resto de la linea
        if (scanf(" %c%*[^\n]", &c) == 1)
        {
            switch (c)
            {
                // Servos dependientes (hombro y codo juntos)
                case 'w':
                case 'd': mover_par(+PASO_PAR); break;
                case 's':
                case 'a': mover_par(-PASO_PAR); break;

                // Servos independientes
                case 'i': mover_base(+PASO);  break;
                case 'k': mover_base(-PASO);  break;
                case 'j': mover_pinza(-PASO); break;
                case 'l': mover_pinza(+PASO); break;

                case 'x': salir = 1; break;
                default: break;
            }
            mostrar_angulos();
        }
    }

    // Apaga los servos y libera pigpio
    gpioServo(PIN_BASE, 0);
    gpioServo(PIN_HOMBRO, 0);
    gpioServo(PIN_CODO, 0);
    gpioServo(PIN_PINZA, 0);
    gpioTerminate();
    return 0;
}

// Convierte el angulo a pulso y lo manda al servo
void escribir_servo(int pin, int angulo)
{
    int pulso = PULSO_MIN + angulo * (PULSO_MAX - PULSO_MIN) / 180;
    gpioServo(pin, pulso);
}

// Hombro y codo se mueven juntos
void mover_par(int delta)
{
    ang_par = ang_par + delta;
    if (ang_par < 0)   ang_par = 0;
    if (ang_par > 180) ang_par = 180;

    escribir_servo(PIN_HOMBRO, ang_par);
    escribir_servo(PIN_CODO, ang_par);
}

void mover_base(int delta)
{
    ang_base = ang_base + delta;
    if (ang_base < 0)   ang_base = 0;
    if (ang_base > 180) ang_base = 180;

    escribir_servo(PIN_BASE, ang_base);
}

// La pinza solo trabaja entre 120 y 140 grados
void mover_pinza(int delta)
{
    ang_pinza = ang_pinza + delta;
    if (ang_pinza < PINZA_MIN) ang_pinza = PINZA_MIN;
    if (ang_pinza > PINZA_MAX) ang_pinza = PINZA_MAX;

    escribir_servo(PIN_PINZA, ang_pinza);
}

void mostrar_angulos(void)
{
    printf("Base:%3d  Hombro:%3d  Codo:%3d  Pinza:%3d\n",
           ang_base, ang_par, ang_par, ang_pinza);
}