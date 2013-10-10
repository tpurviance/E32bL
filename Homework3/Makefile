TOOLS_CFLAGS	:= -Wstrict-prototypes -Wshadow -Wpointer-arith -Wcast-qual \
		   -Wcast-align -Wwrite-strings -Wnested-externs -Winline \
		   -W -Wundef -Wmissing-prototypes
#
# Programs
#
all:	gpioThru

gpioThru:	gpioThru.o gpio-utils.o
	$(CC) $(LDFLAGS) -o $@ $^
	chmod +x ./setup.sh
	./setup.sh


#
# Objects
#

%.o: %.c
	$(CC) $(CFLAGS) $(TOOLS_CFLAGS) -c $< -o $@

clean:
	rm gpioThru *.o
