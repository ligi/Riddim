/**************************************************
 *
 *
 * Riddim
 * Remote Interactive Digital Drone Interface Mashup
 * 
 * 2008 Marcus -LiGi- Bueschleb
 * 
 *
**************************************************/

#if !defined(RIDDIM_H)
#define RIDDIM_H




// for config file parsing
#include <confuse.h>
#include <string.h>
#include "riddim_meta.h"

// for configuration file parsing
#include "config.h" 
// for measuring
#include "statistics.h"
// for understanding the FC
#include "fc.h"
#include "bluetooth_handler.h"

#include "evdev_handler.h"

#include "joy_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>


#include "lib/x52/x52.h"

#include <sys/socket.h>


#include <sys/time.h>



#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <stdio.h>




#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>//for close() for socket


extern int act_nick;
extern int act_roll;
extern int act_gier;
extern int act_gas;
extern int act_mode;

extern int act_alt;
extern long act_long_alt;

#define FALSE 0
#define TRUE 1

#define STATEID_SCANNING 0
#define STATEID_CONNECTING 1


#define BUTTON_SELECT 26
#define BUTTON_DOWN 28
#define BUTTON_UP 27


// #define AXIS_ROLL 0
// #define AXIS_NICK 1
// #define AXIS_GIER 5
// #define AXIS_GAS  2



// for x52
/*
#define AXIS_ROLL 4
#define AXIS_NICK 3
#define AXIS_GIER 5
#define AXIS_GAS  2

#define INVERT_ROLL -1
#define INVERT_NICK -1
#define INVERT_GIER 1
#define INVERT_GAS  -1
*/

#define AXIS_ROLL 0
#define AXIS_NICK 1
#define AXIS_GIER 3
#define AXIS_GAS  2

#define INVERT_ROLL 1
#define INVERT_NICK -1
#define INVERT_GIER 1
#define INVERT_GAS  -1


#define INPUT_NONE 0
#define INPUT_JOYDEV 1
#define INPUT_EVDEV 2


#endif
