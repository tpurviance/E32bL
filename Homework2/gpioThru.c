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
#include "i2cbusses.h"



 /****************************************************************
 * Constants
 ****************************************************************/
 
#define POLL_TIMEOUT (30) //* 1000) /* 3 seconds */
#define MAX_BUF 64

#define GPIO_UP 60
#define GPIO_DOWN 48
#define GPIO_LEFT 3
#define GPIO_RIGHT 49

#define BICOLOR		// undef if using a single color display

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


/****************************************************************
 * LED Matrix code
 ****************************************************************/
// The upper btye is RED, the lower is GREEN.
// The single color display responds only to the lower byte
static __u16 smile_bmp[]=
	{0x3c, 0x42, 0xa9, 0x85, 0x85, 0xa9, 0x42, 0x3c};
static __u16 frown_bmp[]=
	{0x3c00, 0x4200, 0xa900, 0x8500, 0x8500, 0xa900, 0x4200, 0x3c00};
static __u16 neutral_bmp[]=
	{0x3c3c, 0x4242, 0xa9a9, 0x8989, 0x8989, 0xa9a9, 0x4242, 0x3c3c};

static void help(void) __attribute__ ((noreturn));

static void help(void) {
	fprintf(stderr, "Usage: matrixLEDi2c (hardwired to bus 3, address 0x70)\n");
	exit(1);
}

static int check_funcs(int file) {
	unsigned long funcs;

	/* check adapter functionality */
	if (ioctl(file, I2C_FUNCS, &funcs) < 0) {
		fprintf(stderr, "Error: Could not get the adapter "
			"functionality matrix: %s\n", strerror(errno));
		return -1;
	}

	if (!(funcs & I2C_FUNC_SMBUS_WRITE_BYTE)) {
		fprintf(stderr, MISSING_FUNC_FMT, "SMBus send byte");
		return -1;
	}
	return 0;
}

// Writes block of data to the display
static int write_block(int file, __u16 *data) {
	int res;
#ifdef BICOLOR
	res = i2c_smbus_write_i2c_block_data(file, 0x00, 16, 
		(__u8 *)data);
	return res;
#else
/*
 * For some reason the single color display is rotated one column, 
 * so pre-unrotate the data.
 */
	int i;
	__u16 block[I2C_SMBUS_BLOCK_MAX];
//	printf("rotating\n");
	for(i=0; i<8; i++) {
		block[i] = (data[i]&0xfe) >> 1 | 
			   (data[i]&0x01) << 7;
	}
	res = i2c_smbus_write_i2c_block_data(file, 0x00, 16, 
		(__u8 *)block);
	return res;
#endif
}

int grabTemp(int addr, int daddr, int isTest){
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
	if (isTest)
		printf("reached %f degrees farenheit (0x%2.2X) on pin 0x%2.2X\n", res * 1.8 + 32.0, res, addr);
	return res;
}

/****************************************************************
 * Etchasketch code
 ****************************************************************/

#define ROWS 8
#define COLS 8

int* matrix;
int pos_x = 0;
int pos_y = 0;

typedef enum {UP, DOWN, LEFT, RIGHT} Direction;

void mat_alloc(){
	matrix = (int*) calloc(ROWS*COLS, sizeof(int));
}

void mat_free(){
	free(matrix);
}

void mat_do_move(int newx, int newy) {
	printf("moving to %i, %i\n",newx,newy);
	matrix[newx + newy * COLS] = 1;
	pos_x = newx;
	pos_y = newy;	
}

void mat_try_move(Direction dir){
	printf("trying to move\n");
	int dx = 0;
	int dy = 0;
	switch(dir){
		case UP:
			dy = -1;
			break;
		case DOWN:
			dy = 1;
			break;
		case LEFT:
			dx = -1;
			break;
		case RIGHT:
			dx = 1;				
			break;
	}
	int newx = pos_x + dx;
	int newy = pos_y + dy;
	if (newx >= 0 && newx <	COLS &&
		newy >= 0 && newy < ROWS) {
		mat_do_move(newx, newy);
	}
}

void mat_reset() {
	int x, y;
	for (x = 0; x < COLS; x++)
		for (y = 0; y < ROWS; y++)
			matrix[x + y * COLS] = 0;
}

void mat_print(){
	int i,x,y;
	for (x = -1; x <= COLS; x++){
		for (y = -1; y <= ROWS; y++){
			if (x < 0 || x == COLS)
				printf("-");
			else if(y < 0 || y == ROWS)
				printf("|");
			else
				printf((matrix[x+y*COLS] == 1 ? "*" : " "));
		}
		printf((y == ROWS ? "" : "\n"));
	}
}

static __u16 screen[8]= {0,0,0,0,0,0,0,0};

void mat_disp_gen(){
	// The upper btye is RED, the lower is GREEN.
	// The single color display responds only to the lower byte
	int x, y, temp;

	temp = grabTemp(0x49, 0, 0);
	for (y = 0; y < ROWS; y++)
		screen[y] = 0;
		
	for (x = 0; x < COLS; x++){
		for (y = 0; y < ROWS; y++){
			screen[y] = screen[y] * 2 + matrix[x+y*COLS] * 
					((temp > 0x1d ? 0 : 1) + 256*(temp > 0x1b ? 1 : 0));
		}
	}
}

static double theta = 0.0;

void mat_disp_gen_sinos(){
	// The upper btye is RED, the lower is GREEN.
	// The single color display responds only to the lower byte
	int x, y;
	for (y = 0; y < ROWS; y++)
		screen[y] = 0;
		
	for (x = 0; x < COLS; x++){
		for (y = 0; y < ROWS; y++){
			screen[y] = screen[y] * 2 + matrix[x+y*COLS];
		}
	}
}

void mat_led_disp(){

	int res, i2cbus, address, file;
	char filename[20];
	int force = 0;

	i2cbus = lookup_i2c_bus("1");
	if (i2cbus < 0)
		help();

	address = parse_i2c_address("0x70");
	if (address < 0)
		help();

	file = open_i2c_dev(i2cbus, filename, sizeof(filename), 0);
//	printf("file = %d\n", file);
	if (file < 0
	 || check_funcs(file)
	 || set_slave_addr(file, address, force))
		exit(1);

	// Check the return value on these if there is trouble
	i2c_smbus_write_byte(file, 0x21); // Start oscillator (p10)
	i2c_smbus_write_byte(file, 0x81); // Disp on, blink off (p11)
	i2c_smbus_write_byte(file, 0xe7); // Full brightness (page 15)

//	Display a series of pictures
	mat_disp_gen();
	write_block(file, screen);


	close(file);
}


/****************************************************************
 * Main
 ****************************************************************/
int main(int argc, char **argv, char **envp)
{
	struct pollfd fdset[2];
	int nfds = 2;
	int timeout, rc;
	int gpio_fd_up, gpio_fd_down, gpio_fd_left, gpio_fd_right;
	char buf[MAX_BUF];
	unsigned int gpio_up = GPIO_UP;
	unsigned int gpio_down = GPIO_DOWN;
	unsigned int gpio_left = GPIO_LEFT;
	unsigned int gpio_right = GPIO_RIGHT;
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

	mat_alloc();


	gpio_export(gpio_up);
	gpio_set_dir(gpio_up, "in");
	gpio_set_edge(gpio_up, "rising");  // Can be rising, falling or both
	gpio_fd_up = gpio_fd_open(gpio_up, O_RDONLY);

	gpio_export(gpio_down);
	gpio_set_dir(gpio_down, "in");
	gpio_set_edge(gpio_down, "rising");  // Can be rising, falling or both
	gpio_fd_down = gpio_fd_open(gpio_down, O_RDONLY);

	gpio_export(gpio_left);
	gpio_set_dir(gpio_left, "in");
	gpio_set_edge(gpio_left, "falling");  // Can be rising, falling or both
	gpio_fd_left = gpio_fd_open(gpio_left, O_RDONLY);

	gpio_export(gpio_right);
	gpio_set_dir(gpio_right, "in");
	gpio_set_edge(gpio_right, "rising");  // Can be rising, falling or both
	gpio_fd_right = gpio_fd_open(gpio_right, O_RDONLY);

	timeout = POLL_TIMEOUT;
	
	unsigned int gpios[] = {GPIO_UP,GPIO_DOWN,GPIO_LEFT,GPIO_RIGHT};
	unsigned int gpiofds[] = {gpio_fd_up, gpio_fd_down, gpio_fd_left, gpio_fd_right};

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
				case 2:
					mat_try_move(LEFT);
					break;
				case 3:
					mat_try_move(RIGHT);
					break;
			}
			//mat_print();
		}

		if (fdset[0].revents & POLLIN) {
			(void)read(fdset[0].fd, buf, 1);
			printf("\npoll() stdin read 0x%2.2X\n", (unsigned int) buf[0]);
		}

		mat_led_disp();

		cur_gpio = (cur_gpio + 1) % 4;
		fflush(stdout);
	}

	mat_free();
	gpio_fd_close(gpio_fd_up);
	gpio_fd_close(gpio_fd_down);
	gpio_fd_close(gpio_fd_left);
	gpio_fd_close(gpio_fd_right);
	return 0;
}

