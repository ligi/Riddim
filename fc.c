#include "fc.h"



unsigned char TxBuffer[MAX_BUFF_LEN];
unsigned char _TxBuffer[MAX_BUFF_LEN];

unsigned char RxBuffer[MAX_BUFF_LEN];
char PrintableRxBuffer[MAX_BUFF_LEN];



int mk_socket;

int status;

void AddCRC(unsigned int wieviele)
{
  unsigned int tmpCRC = 0,i;
  for(i = 0; i < wieviele;i++)
    {
      tmpCRC += TxBuffer[i];
    }
  tmpCRC %= 4096;
  TxBuffer[i++] = '=' + tmpCRC / 64;
  TxBuffer[i++] = '=' + tmpCRC % 64;
  TxBuffer[i++] = '\r';
}


void SendOutData(unsigned char cmd,unsigned char modul, unsigned char *snd, unsigned char len)
{
  //  return;
  int status =0;
  unsigned int pt = 0;
  unsigned char a,b,c;
  unsigned char ptr = 0;

  TxBuffer[pt++] = '#';               // Startzeichen
  TxBuffer[pt++] = modul;             // Adresse (a=0; b=1,...)
  TxBuffer[pt++] = cmd;		        // Commando

  while(len)
    {
      if(len) { a = snd[ptr++]; len--;} else a = 0;
      if(len) { b = snd[ptr++]; len--;} else b = 0;
      if(len) { c = snd[ptr++]; len--;} else c = 0;
      TxBuffer[pt++] = '=' + (a >> 2);
      TxBuffer[pt++] = '=' + (((a & 0x03) << 4) | ((b & 0xf0) >> 4));
      TxBuffer[pt++] = '=' + (((b & 0x0f) << 2) | ((c & 0xc0) >> 6));
      TxBuffer[pt++] = '=' + ( c & 0x3f);
    }
  
  

  AddCRC(pt);
  printf("Sending to MK %d \n" , pt);

  status = send(mk_socket,"\r" , 1, 0);
  
  
  //  for (c=0;c<pt+2;c++)
  // {
  status = write(mk_socket,&TxBuffer , pt+3);
  

  //   printf("Send to MK %d \n" , TxBuffer[c] );
  // }
  /* int i=0;
  while(TxBuffer[i] !='\r' && i<150)
    {
    //     TxBuffer[i]='#';
    status = send(mk_socket,TxBuffer[i] , 1, 0);
    printf(" +%d%c ",i,TxBuffer[i]);
    i++;
    }

  //  status = send(s,"\r" , 1, 0);
  */
   status = send(mk_socket,"\r" , 1, 0);
   status = send(mk_socket,"\n" , 1, 0);
  printf("\n");
}


int rx_last_length;


void Decode64(unsigned char *ptrOut, unsigned char len, unsigned char ptrIn,unsigned char max) 
{
 unsigned char a,b,c,d;
 unsigned char ptr = 0;
 unsigned char x,y,z;
 while(len)
  {
   a = RxBuffer[ptrIn++] - '=';
   b = RxBuffer[ptrIn++] - '=';
   c = RxBuffer[ptrIn++] - '=';
   d = RxBuffer[ptrIn++] - '=';
   if(ptrIn > max - 2) break;     

   x = (a << 2) | (b >> 4);
   y = ((b & 0x0f) << 4) | (c >> 2);
   z = ((c & 0x03) << 6) | d;

   if(len--) ptrOut[ptr++] = x; else break;
   if(len--) ptrOut[ptr++] = y; else break;
   if(len--) ptrOut[ptr++] = z;	else break;
  }

}

int read_from_mk()
{	
  char in_char='#';
  int count=0;
  int r=0;
  //  int i=0;

  int p=0;
  printf("starting read\n");
  while(in_char!='\r')
    {
      p++;
      //      printf("b read\n");
      count=read(mk_socket,&in_char,1);
     
      //      if ( count ==-1) exit(0);
      printf("a read %d %d %c %d\n",p,count,in_char,in_char);
      if (count!=-1)
	{
	  //  printf("%c\n",in_char);
	  RxBuffer[r]=in_char;
      
	  if (in_char!=0)
	    PrintableRxBuffer[r++]=in_char;
	  else
	    PrintableRxBuffer[r++]='0';
	}

    }
  rx_last_length=r;
  PrintableRxBuffer[r++]='\0'; // terminate 
  printf("done --->%s\n",PrintableRxBuffer);

  if (RxBuffer[2]=='D') 
    {
      debug_sets++;
      Decode64((unsigned char *) &DebugOut,sizeof(DebugOut),3,rx_last_length);
      printf("decoded FC Debug data height:%d\n",DebugOut.Analog[5]);
    }
  return 1;
}


int connect_mk_bluetooth(char dest[18])
{
  struct sockaddr_rc addr ;

  // allocate a socket
  mk_socket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
  
  // set the connection parameters (who to connect to)
  addr.rc_family = AF_BLUETOOTH;
  addr.rc_channel = 1;
  str2ba( dest, &addr.rc_bdaddr );

  // connect to server
  status = connect(mk_socket, (struct sockaddr *)&addr, sizeof(addr));

  printf("connection status %d\n",status);
  return status;
}


int connect_mk_tty(char* tty_filename)
{
 
  mk_socket = open(tty_filename,O_RDWR | O_NOCTTY);



  struct termios termattr;
  //   speed_t baudRate;

      /* Make a copy of the termios structure. */
   tcgetattr(mk_socket, &termattr);
    termattr.c_cc[VMIN]=0;
    termattr.c_cc[VTIME]=1;

    termattr.c_cflag|=CREAD|CLOCAL;
    termattr.c_lflag&=(~(ICANON|ECHO|ECHOE|ECHOK|ECHONL|ISIG));
    termattr.c_iflag&=(~(INPCK|IGNPAR|PARMRK|ISTRIP|ICRNL|IXANY));
    termattr.c_oflag&=(~OPOST);
    termattr.c_cc[VMIN]=0;
    termattr.c_cc[VINTR] = _POSIX_VDISABLE;
    termattr.c_cc[VQUIT] = _POSIX_VDISABLE;
    termattr.c_cc[VSTART] = _POSIX_VDISABLE;
    termattr.c_cc[VSTOP] = _POSIX_VDISABLE;
    termattr.c_cc[VSUSP] = _POSIX_VDISABLE;
    

    //data 8
    
    termattr.c_cflag&=(~CSIZE);
    
    // no par
    termattr.c_cflag&=(~PARENB);
    
    termattr.c_cflag |=CS8 | CREAD | CLOCAL ;
    termattr.c_iflag |=  IGNPAR;
    // stop1
    termattr.c_cflag&=(~CSTOPB);
    
    // Flow off
    termattr.c_cflag&=(~CRTSCTS);
    termattr.c_iflag&=(~(IXON|IXOFF|IXANY | ICRNL));
   
   
        
    cfsetospeed(&termattr,B57600);
    cfsetispeed(&termattr,B57600);

    printf("set attr: %d\n" , tcsetattr(mk_socket, TCSANOW, &termattr));
    

   /*
   usleep(1000000);  
  char in_char='#';
  int count=0;
  int r=0;
  int i=0;

  int p=0;
  printf("starting read %d\n",mk_socket);
  while  (1)
    {
      p=0;
      r=0;
  char in_char='#';
  while((in_char!='\r'))//&&(r<MAX_BUFF_LEN))
	{
	  //      p++;
	  //      printf("b read\n");
	  count=read(mk_socket,&in_char,1);
	  // tcflush( mk_socket, TCOFLUSH );
	  
	  //	  printf("\np !read %d %d %c \n",p,count,in_char);

	  //count=read(mk_socket,&in_char,1);
	  // tcflush( mk_socket, TCOFLUSH );
	  
	  //	  printf("\np !read %d %d %d %c \n",r,p,count,in_char);
	  printf("%d %c \n",r, in_char);
	  
		  if (count==1)
		  {
		  //  printf("%c\n",in_char);
		    RxBuffer[r]=in_char;
		  
		  if (in_char!=0)
		  PrintableRxBuffer[r]=in_char;
		  else
		  PrintableRxBuffer[r]='0';
		  r++;
		  }
	  //else 
			 //printf("%d/%d",errno,EBADF);
	  
	}
  PrintableRxBuffer[r]=0;
      printf("buff> %d\n %s\n", r, PrintableRxBuffer);
    }
*/
  if (mk_socket<0)
    return 0;
  else
    return  1;

}


int connect_mk_localhost_socket(int port)
{
  struct sockaddr_in sa;
 
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = htonl(0x0);
  sa.sin_port = htons(port);
 
  mk_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  return  connect(mk_socket,(struct sockaddr*) &sa, sizeof(struct sockaddr_in));

}

