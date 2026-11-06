#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
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

// Posiciones de la demo (ajustalas a tu brazo)
#define PAR_RETRAIDO   40
#define PAR_EXTENDIDO  140
#define PINZA_CERRADA  120
#define PINZA_ABIERTA  140
#define BASE_IZQ       30
#define BASE_CENTRO    90
#define BASE_DER       150

void escribirServo(int pin, int angulo);
void moverPar(int delta);
void moverBase(int delta);
void moverPinza(int delta);
void mostrarAngulos(void);
void mostrarMenu(void);
void modoLibre(void);
void modoDemo(void);
void detener(int senal);
int  pausa(double segundos);

int salirDemo = 0;
int angBase  = 90;
int angPar   = 90;      // hombro y codo comparten el mismo valor
int angPinza = 130;

int main(void)
{
    char opcion;

    if (gpioInitialise() < 0)
    {
        printf("Error al iniciar pigpio (usa sudo)\n");
        return 1;
    }
    gpioSetSignalFunc(SIGINT, detener);     // Ctrl+C detiene todo

    escribirServo(PIN_BASE, angBase);
    escribirServo(PIN_HOMBRO, angPar);
    escribirServo(PIN_CODO, angPar);
    escribirServo(PIN_PINZA, angPinza);

    system("stty -icanon -echo");   // lee teclas sin Enter y sin eco

    while (1)
    {
        mostrarMenu();
        if (scanf(" %c", &opcion) == 1)
        {
            switch (opcion)
            {
                case '1': modoDemo();  break;
                case '2': modoLibre(); break;
                default: break;
            }
        }
    }
    return 0;
}

void mostrarMenu(void)
{
    printf("\n******************************************\n");
    printf("* Selecciona un modo: (1) Demo  (2) Libre *\n");
    printf("*         Ctrl+C para terminar           *\n");
    printf("******************************************\n");
}

void modoLibre(void)
{
    char tecla;
    int salir = 0;

    printf("w/d: par sube  s/a: par baja  i/k: base  j/l: pinza  x: menu\n");
    mostrarAngulos();

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
}

// Saluda en izquierda, centro y derecha
void modoDemo(void)
{
    int posiciones[3] = { BASE_IZQ, BASE_CENTRO, BASE_DER };
    int i, j;

    salirDemo = 0;
    system("stty min 0 time 0");    // scanf ya no se queda esperando tecla
    printf("Demo en curso...  x: menu\n");

    while (salirDemo == 0)
    {
        for (i = 0; i < 3; i++)
        {
            // mover(destino - actual) lleva el servo directo al destino
            moverPar(PAR_RETRAIDO - angPar);
            moverPinza(PINZA_CERRADA - angPinza);
            if (pausa(1)) break;

            moverBase(posiciones[i] - angBase);
            if (pausa(1)) break;

            moverPar(PAR_EXTENDIDO - angPar);
            if (pausa(1)) break;

            for (j = 0; j < 2; j++)
            {
                moverPinza(PINZA_ABIERTA - angPinza);
                if (pausa(0.5)) break;
                moverPinza(PINZA_CERRADA - angPinza);
                if (pausa(0.5)) break;
            }
            if (salirDemo) break;
        }
    }

    system("stty min 1 time 0");    // scanf vuelve a esperar tecla
    clearerr(stdin);
}

// Espera y revisa si se presiono x
int pausa(double segundos)
{
    char tecla;

    time_sleep(segundos);
    clearerr(stdin);
    if (scanf("%c", &tecla) == 1 && tecla == 'x')
        salirDemo = 1;
    return salirDemo;
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

// Se ejecuta con Ctrl+C: apaga servos y regresa la terminal
void detener(int senal)
{
    system("stty icanon echo");
    gpioServo(PIN_BASE, 0);
    gpioServo(PIN_HOMBRO, 0);
    gpioServo(PIN_CODO, 0);
    gpioServo(PIN_PINZA, 0);
    gpioTerminate();
    printf("\nPrograma terminado\n");
    exit(0);
}