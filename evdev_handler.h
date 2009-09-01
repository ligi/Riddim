#if !defined(EVDEV_HANDLER_H)
#define EVDEV_HANDLER_H

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>//for close() for socket
#include <string.h>
#include <linux/joystick.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>

#define MAX_EVDEVS 20

#define test_bit(bit, array)    (array[bit/8] & (1<<(bit%8)))

int connect_evdev();
int blink_evdev_led();
int init_evdevstatus_led();
int poll_evdev();
void collect_evdev_devices();

extern int  *evdev_rel_axis;
extern char *evdev_button;



typedef struct
{
  char name[255];
  char fname[255];
}  evdevs_t ;

evdevs_t evdevs[MAX_EVDEVS];
extern int evdevs_count;


#define FALSE 0
#define TRUE 1

#endif
