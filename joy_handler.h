#if !defined(JOYDEV_HANDLER_H)
#define JOYDEV_HANDLER_H

#include <linux/joystick.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "config.h"

extern int *axis;
extern char *button;



int joy_input_fd;
int num_of_axis,num_of_buttons;

char *button_trigger;
char name_of_joystick[80];


int connect_joy();

#endif
