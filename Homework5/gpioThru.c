// From : http://stackoverflow.com/questions/13124271/driving-beaglebone-gpio-through-dev-mem
//
// Read one gpio pin and write it out to another using mmap.
// Be sure to set -O3 when compiling.
// Modified by Mark A. Yoder  26-Sept-2013
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <signal.h>    // Defines signal-handling functions (i.e. trap Ctrl-C)
#include "beaglebone_gpio.h"

/****************************************************************
 * Global variables
 ****************************************************************/
volatile int keepgoing = 1;    // Set to 0 when ctrl-c is pressed

/****************************************************************
 * signal_handler
 ****************************************************************/
void signal_handler(int sig);
// Callback called when SIGINT is sent to the process (Ctrl-C)
void signal_handler(int sig)
{
    printf( "\nCtrl-C pressed, cleaning up and exiting...\n" );
	keepgoing = 0;
}

int main(int argc, char *argv[]) {
    volatile void *gpio0_addr;
    volatile void *gpio1_addr;

    volatile unsigned int *gpio0_oe_addr;
    volatile unsigned int *gpio0_datain;
    volatile unsigned int *gpio0_setdataout_addr;
    volatile unsigned int *gpio0_cleardataout_addr;

    volatile unsigned int *gpio1_oe_addr;
    volatile unsigned int *gpio1_datain;
    volatile unsigned int *gpio1_setdataout_addr;
    volatile unsigned int *gpio1_cleardataout_addr;


    // Set the signal callback for Ctrl-C
    signal(SIGINT, signal_handler);

    int fd = open("/dev/mem", O_RDWR);

    gpio0_addr = mmap(0, GPIO0_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 
                        GPIO0_START_ADDR);
    gpio1_addr = mmap(0, GPIO1_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 
                        GPIO1_START_ADDR);


    if(gpio0_addr == MAP_FAILED || gpio1_addr == MAP_FAILED) {
        printf("Unable to map GPIO\n");
        exit(1);
    }

    gpio0_oe_addr           = gpio0_addr + GPIO_OE;
    gpio0_datain            = gpio0_addr + GPIO_DATAIN;
    gpio0_setdataout_addr   = gpio0_addr + GPIO_SETDATAOUT;
    gpio0_cleardataout_addr = gpio0_addr + GPIO_CLEARDATAOUT;

    gpio1_oe_addr           = gpio1_addr + GPIO_OE;
    gpio1_datain            = gpio1_addr + GPIO_DATAIN;
    gpio1_setdataout_addr   = gpio1_addr + GPIO_SETDATAOUT;
    gpio1_cleardataout_addr = gpio1_addr + GPIO_CLEARDATAOUT;


    
    while(keepgoing == 1) {
    	if(*gpio0_datain & GPIO_07) {
            *gpio0_setdataout_addr= GPIO_03;
    	} else {
            *gpio0_cleardataout_addr = GPIO_03;
    	}

	if(*gpio1_datain & GPIO_60) {
            *gpio1_cleardataout_addr = GPIO_48;
    	} else {
            *gpio1_setdataout_addr = GPIO_48;
    	}
    }

    munmap((void *)gpio0_addr, GPIO0_SIZE);
    munmap((void *)gpio1_addr, GPIO1_SIZE);
    close(fd);
    return 0;
}
