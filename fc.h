#ifndef FC_H
#define FC_H

#include <stdio.h>
#include <termios.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <inttypes.h>
#include "bluetooth_handler.h"
#include "statistics.h"

struct ExternControl_s
{
  unsigned char Digital[2];   // (noch unbenutzt)
  unsigned char RemoteTasten; //(gab es schon für das virtuelle Display)
  signed char   Nick;
  signed char   Roll;
  signed char   Gier;
  unsigned char Gas;          //(es wird das Stick-Gas auf diesen Wert begrenzt; --> StickGas ist das Maximum)
  signed char   Higt;        //(Hoehenregler)
  unsigned char free;         // (unbenutzt)
  unsigned char Frame;        // (Bestätigung)
  unsigned char Config;
};

struct ExternEvent_s
{
  unsigned char key;
  unsigned char value[4];

};

struct str_DebugOut
{
 unsigned char Digital[2];
  //# signed int

uint16_t Analog[32];    // Debugwert//
};


struct str_VersionInfo
{
  unsigned char SWMajor;
  unsigned char SWMinor;
  unsigned char ProtoMajor;
  unsigned char ProtoMinor;
  unsigned char SWPatch;
  unsigned char Reserved[5];
};


struct str_DebugOut    DebugOut;


struct ExternControl_s  ExternControl;
struct ExternEvent_s  ExternEvent;

extern int mk_socket;

//int connect_mk_bluetooth(char dest[18]);
int connect_mk_localhost_socket(int port);
int connect_mk_bluetooth(char dest[18]);
int connect_mk_tty(char* tty_filename);

void SendOutData(unsigned char cmd,unsigned char modul, unsigned char *snd, unsigned char len);
int read_from_mk();


#define MAX_BUFF_LEN 150

extern unsigned char RxBuffer[MAX_BUFF_LEN ];
extern char PrintableRxBuffer[MAX_BUFF_LEN ];
extern int rx_last_length;


#endif
