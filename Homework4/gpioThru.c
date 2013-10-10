/* Copyright (c) 2011, RidgeRun
 * All rights reserved.
 *
From https://www.ridgerun.com/developer/wiki/index.php/Gpio-int-test.c

 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the RidgeRun.
 * 4. Neither the name of the RidgeRun nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY RIDGERUN ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL RIDGERUN BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>	// Defines signal-handling functions (i.e. trap Ctrl-C)
#include "gpio-utils.h"
#include "i2c-dev.h"

 /****************************************************************
 * Constants
 ****************************************************************/
 
#define POLL_TIMEOUT (1000) //* 1000) /* 3 seconds */
#define MAX_BUF 64

#define GPIO_MOTOR_A 30
#define GPIO_MOTOR_B 60

#define STEPS_PER_ROTATION 20
#define IR_SENSOR_LOWER_THRESH 100
#define IR_SENSOR_UPPER_THRESH 2000

/****************************************************************
 * Global variables
 ****************************************************************/
int keepgoing = 1;	// Set to 0 when ctrl-c is pressed

/****************************************************************
 * signal_handler
 ****************************************************************/
void signal_handler(int sig);
int get_analog(int ain);
// Callback called when SIGINT is sent to the process (Ctrl-C)
void signal_handler(int sig)
{
	printf( "Ctrl-C pressed, cleaning up and exiting..\n" );
	keepgoing = 0;
}


int cur_gpio = 0;
int motor_a = 0;
int motor_b = 0;
int cur_pos = 0;

void rotateClockwise(int steps) {
	while (steps != 0){
		if (steps < 0) {
			if (cur_gpio == 0)
				motor_a = !motor_a;
			else
				motor_b = !motor_b;
			cur_gpio = (cur_gpio + 1) % 2;
			steps ++;
		} else if (steps > 0) {
			cur_gpio = (cur_gpio + 1) % 2;
			if (cur_gpio == 0)
				motor_a = !motor_a;
			else
				motor_b = !motor_b;
			steps--;
		}
		gpio_set_value(GPIO_MOTOR_A, motor_a);
		gpio_set_value(GPIO_MOTOR_B, motor_b);
		usleep(50000);
	}
	
}

typedef enum {SPIN_SEARCH, TRACK } strategy;


/****************************************************************
 * Main
 ****************************************************************/
int main(int argc, char **argv, char **envp)
{
	struct pollfd fdset[2];
	int nfds = 2;
	int timeout, rc;
	int gpio_fd[2];
	char buf[MAX_BUF];
	int gpio_fd_motor_a, gpio_fd_motor_b;
	unsigned int gpio_motor_a = GPIO_MOTOR_A;
	unsigned int gpio_motor_b = GPIO_MOTOR_B;
	int len;
	system("./setup_analog.sh");

/*
	if (argc < 2) {
		printf("Usage: gpio-int <gpio-pin>\n\n");
		printf("Waits for a change in the GPIO pin voltage level or input on stdin\n");
		exit(-1);
	}
*/

	// Set the signal callback for Ctrl-C
	signal(SIGINT, signal_handler);


	gpio_export(gpio_motor_a);
	gpio_set_dir(gpio_motor_a, "out");
	gpio_fd_motor_a = gpio_fd_open(gpio_motor_a, O_WRONLY);

	gpio_export(gpio_motor_b);
	gpio_set_dir(gpio_motor_b, "out");
	gpio_fd_motor_b = gpio_fd_open(gpio_motor_b, O_WRONLY);


	timeout = POLL_TIMEOUT;
	
	//unsigned int gpios[] = {GPIO_BUTTONS,GPIO_SCREEN};
	//unsigned int gpiofds[] = {gpio_fd_buttons, gpio_fd_screen};

	

	int pos = 0; 	
	int value0;
	int value1;
	int lights[STEPS_PER_ROTATION];
	strategy cur_strat = SPIN_SEARCH;
	while (keepgoing) {
		switch (cur_strat){
			case SPIN_SEARCH:
				value0 = get_analog(0);
				value1 = get_analog(1);
				if (value0 > IR_SENSOR_LOWER_THRESH &&
						value0 < IR_SENSOR_UPPER_THRESH &&
						value1 > IR_SENSOR_LOWER_THRESH &&
						value1 < IR_SENSOR_UPPER_THRESH){
					lights[pos] = (value0 + value1) / 2;
					pos++;
					rotateClockwise(1);
					if (pos == STEPS_PER_ROTATION) {
						int i = 0;
						int mini = 0;
						int minv = lights[0];
						for (i = 1; i < STEPS_PER_ROTATION; i++){
							if (lights[i] < minv){
								minv = lights[i];
								mini = i;
							}
						}
						
						rotateClockwise(mini - STEPS_PER_ROTATION);
						cur_strat = TRACK;
					}
				} else 
					
				break;
			case TRACK:
				value0 = get_analog(0);
				value1 = get_analog(1);
				if (value0 < value1)
					rotateClockwise(1);
				else 
					rotateClockwise(-1);
				break;
		
		}
		
		int value0 = get_analog(0);
		int value1 = get_analog(1);
		printf("%i\t%i\tstate:%i\n", value0, value1, (int) cur_strat);
		
	}

	gpio_fd_close(gpio_fd_motor_a);
	gpio_fd_close(gpio_fd_motor_b);
	return 0;
}

int get_analog(int ain){
	char buff[100];
	if (ain > 7 | ain < 0){
		return -1;
	}
	sprintf(buff, "/sys/devices/ocp.2/helper.14/AIN%d", ain);
	FILE * ain_file;
	ain_file = fopen(buff, "r");
	if (ain_file == NULL){
		return -1;
	}
	int ain_value;
	fscanf(ain_file, "%i", &ain_value);
	fclose(ain_file);
	return ain_value;

}
