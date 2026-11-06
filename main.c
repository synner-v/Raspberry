#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>

#define PULSO_MIN  500      // us = 0 grados
#define PULSO_MAX  2500     // us = 180 grados

#define PASO_PAR   10
#define PASO       3

#define PINZA_MIN  120
#define PINZA_MAX  140

#define PIN_BASE    22
#define PIN_HOMBRO  23
#define PIN_CODO    24
#define PIN_PINZA   25

void escribirServo(int pin, int angulo);
void moverPar(int delta);
void moverBase(int delta);
void moverPinza(int delta);
void mostrarAngulos(void);

int angBase  = 90;
int angPar   = 90;      // hombro y codo comparten el mismo valor
int angPinza = 130;

int main(void)
{
    char tecla;
    int salir = 0;

    if (gpioInitialise() < 0)
    {
        printf("Error al iniciar pigpio (usa sudo)\n");
        return 1;
    }

    escribirServo(PIN_BASE, angBase);
    escribirServo(PIN_HOMBRO, angPar);
    escribirServo(PIN_CODO, angPar);
    escribirServo(PIN_PINZA, angPinza);

    printf("w/d: par sube  s/a: par baja  i/k: base  j/l: pinza  x: salir\n");
    mostrarAngulos();

    system("stty -icanon -echo");   // lee teclas sin Enter y sin eco

    while (salir == 0)
    {
        if (scanf(" %c", &tecla) == 1)
        {
            switch (tecla)
            {
                case 'w':
                case 'd': moverPar(+PASO_PAR); break;
                case 's':
                case 'a': moverPar(-PASO_PAR); break;

                case 'i': moverBase(+PASO);  break;
                case 'k': moverBase(-PASO);  break;
                case 'j': moverPinza(-PASO); break;
                case 'l': moverPinza(+PASO); break;

                case 'x': salir = 1; break;
                default: break;
            }
            mostrarAngulos();
        }
    }

    system("stty icanon echo");     // regresa la terminal a la normalidad

    gpioServo(PIN_BASE, 0);
    gpioServo(PIN_HOMBRO, 0);
    gpioServo(PIN_CODO, 0);
    gpioServo(PIN_PINZA, 0);
    gpioTerminate();
    return 0;
}

void escribirServo(int pin, int angulo)
{
    int pulso = PULSO_MIN + angulo * (PULSO_MAX - PULSO_MIN) / 180;
    gpioServo(pin, pulso);
}

void moverPar(int delta)
{
    angPar = angPar + delta;
    if (angPar < 0)   angPar = 0;
    if (angPar > 180) angPar = 180;

    escribirServo(PIN_HOMBRO, angPar);
    escribirServo(PIN_CODO, angPar);
}

void moverBase(int delta)
{
    angBase = angBase + delta;
    if (angBase < 0)   angBase = 0;
    if (angBase > 180) angBase = 180;

    escribirServo(PIN_BASE, angBase);
}

void moverPinza(int delta)
{
    angPinza = angPinza + delta;
    if (angPinza < PINZA_MIN) angPinza = PINZA_MIN;
    if (angPinza > PINZA_MAX) angPinza = PINZA_MAX;

    escribirServo(PIN_PINZA, angPinza);
}

void mostrarAngulos(void)
{
    printf("Base:%3d  Hombro:%3d  Codo:%3d  Pinza:%3d\n",
           angBase, angPar, angPar, angPinza);
}