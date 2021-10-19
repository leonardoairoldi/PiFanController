//#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#define PI_INPUT 0
#define PI_OUTPUT 1

#define TEMP_CMD "cat /sys/class/thermal/thermal_zone0/temp"

#define FAN_PIN 1
#define RANGE_MAX 255
#define CONFIG_FILE "fan.config"
#define MAX_LENGHT 64

#define DEBUG_MODE 1

int gpioInitialise() { return 0; };
void gpioTerminate() {};
void gpioSetMode(int a, int b) {};
void gpioWrite(int a, int b) {};
void gpioPMW(int a, int b) {};
void gpioSetPMWrange(int a, int b) {};

//Functions prototypes
float get_temp();
int load_preset();
int get_fanspeed(float temp);

//Global variables
int a, b, c; //Curve coefficients

//Global variable to handle the exit of the program with interrupts
volatile int exit_called = 0;
void signal_handler(int signal) { exit_called = 1; printf("Raised signal: %d\n", signal); }

int main()
{
    //Set up the routine to respond to interrupts
    signal(SIGINT, signal_handler);

    if(load_preset() < 0)
        return -2;
    if(gpioInitialise() < 0)
    {
        printf("Error initializing gpio\n");
        return -1;
    }

    gpioSetMode(FAN_PIN, PI_OUTPUT);

    int fanspeed;
    float temp;
    while(!exit_called)
    {
        temp = get_temp();
        fanspeed = get_fanspeed(temp);
        gpioPMW(FAN_PIN, fanspeed);

        sleep(5);
    }

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
	
    if(DEBUG_MODE) printf("%f\n", x/1000);

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

}