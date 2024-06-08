#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#define PI_INPUT 0
#define PI_OUTPUT 1

#define TEMP_CMD "cat /sys/class/thermal/thermal_zone0/temp"

#define FAN_PIN 18
#define RANGE_MAX 255
#define CONFIG_FILE "fan.config"
#define MAX_LENGHT 64

#define DEBUG_MODE 1


//Functions prototypes
float get_temp();
int load_preset();
int get_fanspeed(float temp);

//Global variables
int a, b, c; //Curve coefficients

//Global variable to handle the exit of the program with interrupts
volatile int exit_called = 0;
void signal_handler(int signal) { exit_called = 1; printf("Raised signal: %d\n", signal); }

int main(int argc, char* argv)
{
    //signal(SIGKILL, signal_handler);

    if(load_preset() < 0)
        return -2;
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

    int fanspeed;
    float temp;
    while(!exit_called)
    {
        temp = get_temp();
        fanspeed = get_fanspeed(temp);
        gpioPWM(FAN_PIN, fanspeed);

        sleep(5);
    }

    printf("Program Exited correctly\n");
    gpioWrite(FAN_PIN, 0);
    gpioTerminate();
}

float get_temp()
{
    FILE* fp;
	fp = popen(TEMP_CMD, "r");

	int x;
	fscanf(fp, "%d", &x);

    pclose(fp);
	
    if(DEBUG_MODE) printf("%f\n", (float)x/1000); // Debug

    return (float)x / 1000;
}

int load_preset()
{
    FILE* fp = fopen(CONFIG_FILE, "r");
    if(!fp)
    {
        printf("Error opening '%s' file", CONFIG_FILE);
        return -2;
    }
    char preset[MAX_LENGHT];
    fscanf(fp, "%s", preset);
    if(strcmp("preset:", preset) != 0)
    {
        printf("Bad config file\n");
        return -3;
    }
    fscanf(fp, "%s", preset); //Preset name loaded
    
    //Scrolling through presets
    char read_str[MAX_LENGHT];
    int nofpreset;
    int preset_found = 0;

    fscanf(fp, "%s %d", read_str, &nofpreset);
    for(int i = 0; i < nofpreset && !preset_found; i++)
    {
        fscanf(fp, "%s %d %d %d", read_str, &a, &b, &c);
        if(strcmp(read_str, preset) == 0)
            preset_found = 1;
    }
    if(!preset_found)
    {
        printf("Bad config file - preset not found\n");
        return -4;
    }

    return 0;
}

int get_fanspeed(float temp)
{
    if(temp < a) return 0;
    //else if (temp < b) return 100;
    //else if (temp < c) return 180;
    return 255;
}