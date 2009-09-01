#include "bluetooth_handler.h" 

int bt_device_count=0;

char names[MAX_BT_DEVICES][248];
char addrs[MAX_BT_DEVICES][19];

char BtHostRxBuffer[150];
char BtHostRxBufferDecoded[150];

int BtRxCount;
int scan_bt()
{
  inquiry_info *ii = NULL;

  int dev_id, sock, len, flags;
  int i;
  char addr[19] = { 0 };
  char name[248] = { 0 };
  
  dev_id = hci_get_route(NULL);
  sock = hci_open_dev( dev_id );
  if (dev_id < 0 || sock < 0) {
    perror("opening socket");
    return 0; 
  }

  len  = 8;
  
  flags = IREQ_CACHE_FLUSH;
  ii = (inquiry_info*)malloc(MAX_BT_DEVICES * sizeof(inquiry_info));
  
  bt_device_count = hci_inquiry(dev_id, len, MAX_BT_DEVICES, NULL, &ii, flags);
  if(  bt_device_count < 0 ) perror("hci_inquiry");
  
  for (i = 0; i <  bt_device_count; i++) {
    ba2str(&(ii+i)->bdaddr, addr);
    sprintf(addrs[i],"%s",addr);

    memset(name, 0, sizeof(name));
   
    if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), 
			     name, 0) < 0)
      sprintf(names[i],"[unknown]");
    else
      sprintf(names[i],"%s",name);

  }

  
  free( ii );
  close( sock );
  return 1;
}

//struct timeval timeout;
fd_set readfds, writefds;
int s;
struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
char buf[1024] = { 0 };
int  client, bytes_read, status;
unsigned int opt = sizeof(rem_addr);

int maxfd, sock_flags;


unsigned char BT_TxBuffer[MAX_BUFF_LEN];

extern struct str_DebugOut    DebugOut;
struct str_VersionInfo VersionInfo;

int bt_host_init()
{
  
  // allocate socket
  s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
  
  // bind socket to port 1 of the first available bluetooth adapter
  loc_addr.rc_family = AF_BLUETOOTH;
  loc_addr.rc_bdaddr = *BDADDR_ANY;
  loc_addr.rc_channel = 1;
  bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));
  
  // put server socket into listening mode
  listen(s, 1);
  
  // put server socket into nonblocking mode
  sock_flags = fcntl( s, F_GETFL, 0 );
  fcntl( s, F_SETFL, sock_flags | O_NONBLOCK );

  return 1;
}


int bt_host_connected=0;

int bt_host_send(char RxBuffer[150],int len)
{
  if (bt_host_connected)
    {
      send(client,"\r",1,0);
      send(client,RxBuffer,len,0);
      send(client,"\r",1,0);
    }
  return 1;
}


struct {
  unsigned char MenuePunkt;
  unsigned char MaxMenue;
  char DisplayBuff[80];
} menu;


void copy_lcd_str(int x,int y,char str[])
{
  int pos=0;
  while (str[pos]!=0)
    {

      menu.DisplayBuff[y*20+x+pos]=str[pos];
      pos++;
    }

}



void Menu(void)
{

  //  menu.MenuePunkt=0;
  menu.MaxMenue=3;
  int x;
  int y;
  
  char tmp_str[20];
  
  for (x=0;x<20;x++)
    for (y=0;y<4;y++)
      menu.DisplayBuff[y*20+x]=' ';
      
  switch(menu.MenuePunkt)
   {
    case 0:
      //       LCD_printfxy(0,0,"+ Riddim +");
		 /*    LCD_printfxy(0,1,"Version: %d %d",0,8);
           LCD_printfxy(0,2,"");
           LCD_printfxy(0,3,"(cc) 2009 ligi");
      */
      // clear_lcd



      sprintf(tmp_str,"+ Riddim +");
      copy_lcd_str(0,0,tmp_str);

      sprintf(tmp_str,"Version: V%d.%d/%d",RIDDIM_VERSION_MAJOR,RIDDIM_VERSION_MINOR,RIDDIM_VERSION_PATCH);
      copy_lcd_str(0,1,tmp_str);


      sprintf(tmp_str,"(cc) 2009 LiGi");
      copy_lcd_str(0,3,tmp_str);

      //  sprintf(&menu.DisplayBuff,"+ Riddim ++");

	   //	   sprintf(&menu.DisplayBuff+20,"Version: %d %d",0,8);

           break;
   
   case 1:
     sprintf(tmp_str,"Loop:");
     copy_lcd_str(0,0,tmp_str);
     sprintf(tmp_str," Count %ld",trip_count);
     copy_lcd_str(0,1,tmp_str);
     sprintf(tmp_str," Time %ld",last_trip_time);
     copy_lcd_str(0,2,tmp_str);
     break;
   

   case 2:
     sprintf(tmp_str,"Inputs:");
     copy_lcd_str(0,0,tmp_str);
     sprintf(tmp_str," Complete %d",evdevs_count);
     copy_lcd_str(0,1,tmp_str);
     sprintf(tmp_str," Connected %d",input_count);
     copy_lcd_str(0,2,tmp_str);
     sprintf(tmp_str," Configured %d",configured_input_count);
     copy_lcd_str(0,3,tmp_str);
     break;

   case 3:
     sprintf(tmp_str,"Nick: %d",act_nick);
     copy_lcd_str(0,0,tmp_str);
     sprintf(tmp_str,"Roll %d",act_roll);
     copy_lcd_str(0,1,tmp_str);
     sprintf(tmp_str,"Gier %d",act_gier);
     copy_lcd_str(0,2,tmp_str);
     sprintf(tmp_str,"Gas %d",act_gas);
     copy_lcd_str(0,3,tmp_str);
     break;

   default:
     sprintf(tmp_str,"illegal page %d",menu.MenuePunkt);
     copy_lcd_str(0,0,tmp_str);
     break;
  }
}



void BT_Decode64(void)  // die daten werden im rx buffer dekodiert, das geht nur, weil aus 4 byte immer 3 gemacht werden.
{
 unsigned char a,b,c,d;
 unsigned char x,y,z;
 unsigned char ptrIn = 3; // start at begin of data block
 unsigned char ptrOut = 0;
 unsigned char len = BtRxCount - 5; // von der Gesamtbytezahl eines Frames gehen 3 Bytes des Headers  ('#',Addr, Cmd) und 3 Bytes des Footers (CRC1, CRC2, '\r') ab.

 while(len)
  {
   a = BtHostRxBuffer[ptrIn++] - '=';
   b = BtHostRxBuffer[ptrIn++] - '=';
   c = BtHostRxBuffer[ptrIn++] - '=';
   d = BtHostRxBuffer[ptrIn++] - '=';

   x = (a << 2) | (b >> 4);
   y = ((b & 0x0f) << 4) | (c >> 2);
   z = ((c & 0x03) << 6) | d;
   printf("\naaaaa %d / %d\naaaaa s %s => x %d ; y %d ; z %d \n",ptrOut, BtRxCount ,BtHostRxBuffer,x,y,z);


   if(len--) BtHostRxBufferDecoded[ptrOut++] = x; else break;
   if(len--) BtHostRxBufferDecoded[ptrOut++] = y; else break;
   if(len--) BtHostRxBufferDecoded[ptrOut++] = z;	else break;
  }
 //RxDataLen = ptrOut ;  // wie viele Bytes wurden dekodiert?

}

void BT_AddCRC(unsigned int wieviele)
{
  unsigned int tmpCRC = 0,i;
  for(i = 0; i < wieviele;i++)
    {
      tmpCRC += BT_TxBuffer[i];
    }
  tmpCRC %= 4096;
  BT_TxBuffer[i++] = '=' + tmpCRC / 64;
  BT_TxBuffer[i++] = '=' + tmpCRC % 64;
  BT_TxBuffer[i++] = '\r';
}

void BT_SendOutData(unsigned char cmd,unsigned char modul, unsigned char *snd, unsigned char len)
{
  //  return;
  int status =0;
  unsigned int pt = 0;
  unsigned char a,b,c;
  unsigned char ptr = 0;

  printf("packing base len %d -cmd %c\n",len,cmd);

  BT_TxBuffer[pt++] = '#';               // Startzeichen
  BT_TxBuffer[pt++] = modul;             // Adresse (a=0; b=1,...)
  BT_TxBuffer[pt++] = cmd;		        // Commando

  while(len)
    {
      if(len) { a = snd[ptr++]; len--;} else a = 0;
      if(len) { b = snd[ptr++]; len--;} else b = 0;
      if(len) { c = snd[ptr++]; len--;} else c = 0;
      BT_TxBuffer[pt++] = '=' + (a >> 2);
      BT_TxBuffer[pt++] = '=' + (((a & 0x03) << 4) | ((b & 0xf0) >> 4));
      BT_TxBuffer[pt++] = '=' + (((b & 0x0f) << 2) | ((c & 0xc0) >> 6));
      BT_TxBuffer[pt++] = '=' + ( c & 0x3f);
    }
  
  

  BT_AddCRC(pt);
  printf("Sending to BT_Client %d \n" , pt);

  status = send(client,"\r" , 1, 0);
  
  
  //  for (c=0;c<pt+2;c++)
  // {
  status = write(client,&BT_TxBuffer , pt+3);
  

   status = send(client,"\r" , 1, 0);
  printf("\n");
}


void bt_process_buffer()
{
  printf("processing buffer\n");
  switch(BtHostRxBuffer[2])
    {

    case 'v':
      printf("got version request\n");
      VersionInfo.SWMinor=RIDDIM_VERSION_MINOR;
      VersionInfo.SWMajor=RIDDIM_VERSION_MAJOR;
      VersionInfo.SWPatch=RIDDIM_VERSION_PATCH;

      BT_SendOutData('V', RIDDIM_ADDRESS, (unsigned char *) &VersionInfo, sizeof(VersionInfo));
     
      break;

    case 'l':
      printf("got display request\n");
      Menu();
      menu.MenuePunkt=BtHostRxBufferDecoded[0];
      printf("sending %s \n",menu.DisplayBuff);
      BT_SendOutData('L', RIDDIM_ADDRESS, (unsigned char *) &menu, sizeof(menu));
	break;

    }

}








      int r=0;

struct timeval last_debug_time;
struct timeval act_time;

int bt_host_tick(int mk_fd)
{

  // try to accept connection if none yet
  if (!bt_host_connected)
  { 
    //    timeout.tv_sec = 0;
    // timeout.tv_usec = 100;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_SET(s, &readfds);
    maxfd = s;

    printf("waiting for bt connection\n");
    //    status = select(maxfd + 1, &readfds, &writefds, 0, &timeout);
    status = select(maxfd + 1, &readfds, &writefds, 0,& (struct timeval) { 0, 100 });
    if( status > 0 && FD_ISSET( s, &readfds ) ) {
      // incoming connection
      client = accept( s, (struct sockaddr*)&rem_addr, &opt );
      if( client >= 0 )
	{
	  bt_host_connected=1;

	  // close the server socket, leaving only the client socket open
	  //	  close(s);
  
	  
	  ba2str( &rem_addr.rc_bdaddr, buf );
	  fprintf(stderr, "accepted connection from %s!!!!!!!\n", buf);
	  memset(buf, 0, sizeof(buf));
	  
	  // put client socket into nonblocking mode
	  sock_flags = fcntl( client, F_GETFL, 0 );
	  fcntl( client, F_SETFL, sock_flags | O_NONBLOCK );
	}
	
    }

  }
  else
    {
      gettimeofday(&act_time,NULL);
      if (act_time.tv_sec>last_debug_time.tv_sec)
	{

	  DebugOut.Analog[9]=23; // voltage
	  DebugOut.Analog[5]=23; // alt

	  BT_SendOutData('D', RIDDIM_ADDRESS, (unsigned char *) &DebugOut, sizeof(DebugOut));


	  gettimeofday(&last_debug_time,NULL);
	}
      printf("reading from bt_host");
      char in_char='#';
      int count=0;

      struct timeval timeout;
      timeout.tv_sec = 0;
      timeout.tv_usec = 100;
      
      FD_ZERO(&readfds);
      FD_ZERO(&writefds);
      FD_SET(client, &readfds);
      maxfd = client;
      
      printf("waiting for connection\n");
      status = select(maxfd + 1, &readfds, 0, 0, &timeout);
      if( status >0 ) 
	{
	  count=read(client,&in_char,1);
	  //send(mk_fd,"\r",1,0);
	  if (count==-1)   
	    {
	      bt_host_connected=0;
			  //return 0;    
	    }	
	  else
	  if(in_char!='\r')
	    {
	   
		printf("count: %d in  %d chr %c r:%d\n",count,in_char,in_char,r);
		//send(mk_fd,&in_char,1,0);
		BtHostRxBuffer[r++]=in_char;

	    }
	  else 
	    {
	      BtRxCount=r;
	      r=0;
	     
	      BT_Decode64();
	      bt_process_buffer();
	    }
	  //	  send(mk_fd,"\r",1,0);
	}
	  else
	return 0;
    }
  
  return 1;    
}
