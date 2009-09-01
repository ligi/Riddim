# Makefile for Riddim
#
# 
CC=gcc
CFLAGS+=-Wall -g 
LDFLAGS+=-g -lusb -lbluetooth -lconfuse

riddim:  config.o statistics.o fc.o joy_handler.o evdev_handler.o bluetooth_handler.o riddim.o lib/x52/x52.o


clean:
	rm -f *.o riddim *~ lib/*/*.o
