# PiFanController

## Bash Aliases
To use the program quickly, use an alias.
Add this line in `~/.bashrc`

    alias fan="sudo /home/pi/PiFanController/pwm 255"

Add a temp alias to quickly read the RaspberryPi Core temperature

    alias temp="/usr/bin/vcgencmd measure_temp"

## Compiling
To compile use:

    gcc controller.c -lpigpio -lrt -Wall -pthread

PiGPIO Library is needed
Download it following the instructions at:

https://abyz.me.uk/rpi/pigpio/

Big thanks to the creator of the C library.
   
