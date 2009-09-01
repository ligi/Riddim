
#if !defined(BLUETOOTH_HANDLER_H)
#define BLUETOOTH_HANDLER_H


#define RIDDIM_ADDRESS 12+'a'
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include <unistd.h>//for close() for socket
#include <stdlib.h>


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>




#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h>

#include <unistd.h>//for close() for socket

#include <string.h>
#include "riddim_meta.h"
#include "statistics.h" 
#include "config.h" 
#include "riddim.h"
#include "fc.h"

#define MAX_BT_DEVICES 3

extern int bt_device_count;

extern char names[MAX_BT_DEVICES][248];
extern char addrs[MAX_BT_DEVICES][19];

int scan_bt();
int bt_host_tick(int mk_fd);
int bt_host_init();
int bt_host_send(char RxBuffer[150],int len);


#define MAX_BUFF_LEN 150


extern struct str_DebugOut    DebugOut;
extern struct str_VersionInfo VersionInfo;


#endif
