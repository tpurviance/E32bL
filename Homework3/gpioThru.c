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

#define GPIO_BUTTONS 47
#define GPIO_SCREEN 44

/****************************************************************
 * Global variables
 ****************************************************************/
int keepgoing = 1;	// Set to 0 when ctrl-c is pressed

/****************************************************************
 * signal_handler
 ****************************************************************/
void signal_handler(int sig);
// Callback called when SIGINT is sent to the process (Ctrl-C)
void signal_handler(int sig)
{
	printf( "Ctrl-C pressed, cleaning up and exiting..\n" );
	keepgoing = 0;
}

typedef enum {UP, DOWN, LEFT, RIGHT} Direction;

void grabTemp(int addr, int daddr, int isTest){
	char *end;
	int res, i2cbus, address, size, file;
	int daddress;
	char filename[20];

	i2cbus   = 1;
	address  = addr;
	daddress = daddr;
	size = I2C_SMBUS_BYTE;

	sprintf(filename, "/dev/i2c-%d", i2cbus);
	file = open(filename, O_RDWR);
	if (file<0) {
		if (errno == ENOENT) {
			fprintf(stderr, "Error: Could not open file "
				"/dev/i2c-%d: %s\n", i2cbus, strerror(ENOENT));
		} else {
			fprintf(stderr, "Error: Could not open file "
				"`%s': %s\n", filename, strerror(errno));
			if (errno == EACCES)
				fprintf(stderr, "Run as root?\n");
		}
		exit(1);
	}

	if (ioctl(file, I2C_SLAVE, address) < 0) {
		fprintf(stderr,
			"Error: Could not set address to 0x%02x: %s\n",
			address, strerror(errno));
		return -errno;
	}

/*
	res = i2c_smbus_write_byte(file, daddress);
	if (res < 0) {
		fprintf(stderr, "Warning - write failed, filename=%s, daddress=%d\n",
			filename, daddress);
	}
*/

	res = i2c_smbus_read_byte_data(file, daddress);
	close(file);

	if (res < 0) {
		fprintf(stderr, "Error: Read failed, res=%d\n", res);
		//exit(2);
	}
	if (!isTest)
		printf("reached %f degrees farenheit (0x%2.2X)\n", res * 1.8 + 32.0, res);
	else
		printf("reached %f degrees farenheit (0x%2.2X) on pin 0x%2.2X\n", res * 1.8 + 32.0, res, addr);
}



void mat_try_move(Direction dir){
	printf("something happened!\n");
	switch(dir){
		case UP:
			grabTemp(0x49,0,0);
			break;
		case DOWN:
			grabTemp(0x4a,0,0);
			break;
		case LEFT:
			break;
		case RIGHT:			
			break;
	}
}

/****************************************************************
 * Main
 ****************************************************************/
int main(int argc, char **argv, char **envp)
{
	struct pollfd fdset[2];
	int nfds = 2;
	int timeout, rc;
	int gpio_fd_buttons, gpio_fd_screen;
	char buf[MAX_BUF];
	unsigned int gpio_buttons = GPIO_BUTTONS;
	unsigned int gpio_screen = GPIO_SCREEN;
	int len;

/*
	if (argc < 2) {
		printf("Usage: gpio-int <gpio-pin>\n\n");
		printf("Waits for a change in the GPIO pin voltage level or input on stdin\n");
		exit(-1);
	}
*/

	// Set the signal callback for Ctrl-C
	signal(SIGINT, signal_handler);

	system("./setup.sh");


	gpio_export(gpio_buttons);
	gpio_set_dir(gpio_buttons, "in");
	gpio_set_edge(gpio_buttons, "both");  // Can be rising, falling or both
	gpio_fd_buttons = gpio_fd_open(gpio_buttons, O_RDONLY);

	gpio_export(gpio_screen);
	gpio_set_dir(gpio_screen, "in");
	gpio_set_edge(gpio_screen, "both");  // Can be rising, falling or both
	gpio_fd_screen = gpio_fd_open(gpio_screen, O_RDONLY);


	timeout = POLL_TIMEOUT;
	
	unsigned int gpios[] = {GPIO_BUTTONS,GPIO_SCREEN};
	unsigned int gpiofds[] = {gpio_fd_buttons, gpio_fd_screen};

	int cur_gpio = 0;
 
	while (keepgoing) {
		memset((void*)fdset, 0, sizeof(fdset));

		fdset[0].fd = STDIN_FILENO;
		fdset[0].events = POLLIN;
		fdset[1].events = POLLPRI;


		fdset[1].fd = gpiofds[cur_gpio];
		rc = poll(fdset, nfds, timeout);      

		if (rc < 0) {
			printf("\npoll() failed!\n");
			return -1;
		}
            
		if (fdset[1].revents & POLLPRI) {
			lseek(fdset[1].fd, 0, SEEK_SET);  // Read from the start of the file
			len = read(fdset[1].fd, buf, MAX_BUF);
			printf("\npoll() GPIO %d interrupt occurred, value=%c, len=%d\n",
				 gpios[cur_gpio], buf[0], len);
			switch (cur_gpio) {
				case 0:
					mat_try_move(UP);
					break;
				case 1:
					mat_try_move(DOWN);
					break;

			}
		}
		

		grabTemp(0x49+cur_gpio,0,1);
		cur_gpio = (cur_gpio + 1) % 2;
		fflush(stdout);
	}

	gpio_fd_close(gpio_fd_buttons);
	gpio_fd_close(gpio_fd_screen);
	return 0;
}

