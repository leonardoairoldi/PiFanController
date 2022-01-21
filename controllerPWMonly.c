#include <limits.h>
#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define PI_INPUT 0
#define PI_OUTPUT 1

#define FAN_PIN 18

#define DEBUG_MODE 1


//Functions prototypes
float get_temp();
int get_fanspeed();

//Global variable to handle the exit of the program with interrupts
volatile int exit_called = 0;
void signal_handler(int signal) { exit_called = 1; printf("Raised signal: %d\n", signal); }



int main(int argc, char* argv[])
{
    if(gpioInitialise() < 0)
    {
        printf("Error initializing gpio\n");
        return -1;
    }


    // Set up the routine to respond to interrupts
    // Important!
    // Set the interrupt routine after gpioInitialise() beacuse the library defines sigHandlers

    signal(SIGINT, signal_handler);

    gpioSetMode(FAN_PIN, PI_OUTPUT);

    if(argc != 2) {
        printf("Invalid fanspeed. Please use a number between 0 and 255.\n");
        return -1;
    }
    int fanspeed = get_fanspeed(argv);

    while(!exit_called)
    {
        gpioPWM(FAN_PIN, fanspeed);
        sleep(UINT_MAX);
    }

    printf("Program Exited correctly\n");
    gpioWrite(FAN_PIN, 0);
    gpioTerminate();
}

int get_fanspeed(char* argv[])
{
    int fans = atoi(argv[1]);
    if(fans < 0 || fans > 255) {
        printf("Invalid fanspeed. Please use a number between 0 and 255.\n");
        return -1;
    }
    return fans;
}
