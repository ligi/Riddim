/**************************************************
 *
 *
 * Riddim
 * Remote Interactive Digital Drone Interface Mashup
 * 
 * (cc) 2007-2009 Marcus -LiGi- Bueschleb
 * 
 *
 **************************************************/

#include "riddim.h"

int state=STATEID_SCANNING;
struct js_event x52_event_struct;

int engines_on=0;
int old_engines_on=0;

int *axis;
char *button;

int act_nick=0;
int act_roll=0;
int act_gier=0;
int act_gas=0;
int act_mode=0;

int act_alt=0;
long act_long_alt=0;

struct x52 *x52_output;

int selected_bt_device=0;

int count=0;
int connected=0;
int input=INPUT_NONE;

void write_display(int line,char* text)
{
  if (x52_output) x52_settext(x52_output, line , text, strlen(text));
}

void clear_display()
{
  write_display(0,"");
  write_display(1,"");
  write_display(2,"");
}


void output_device_list()
{
  int i;
  char disp_txt[20];
  for(i=0;i<bt_device_count;i++)
    {
      if (i<3)
        {
          
          if (selected_bt_device==i)
            sprintf(disp_txt,"#%s",names[i]);
          else
            sprintf(disp_txt," %s",names[i]);
          write_display(i,disp_txt);
        }
    }
}


void print_device_list()
{
  int i;
  for(i=0;i<bt_device_count;i++)
    printf("device%i->%s\n",i,names[i]);
}




int main(int argc, char**argv)
{

  printf("Starting Riddim %d.%d \n",RIDDIM_VERSION_MAJOR,RIDDIM_VERSION_MINOR );
  printf("\tRemote Interactive Digital Drone Interface Mashup\n");
  printf("\nusage:\n");
  printf("\t riddim [config_file]\n\n");

  // check if config file as argument
  if (argv[1])
    parse_config(argv[1]);
  else
    parse_config("/etc/riddim.conf");

  bt_host_init();
  collect_evdev_devices();
  parse_config_input_sections();
  //  exit(0);

  printf("input %s:\n",input_evdev_name);  
  /*
  if (bluetooth_mac)
    {

      printf("Connecting via Bluetooth to %s\n",bluetooth_mac);
      if (connect_mk_bluetooth(bluetooth_mac));;
      connected=TRUE;
    }
*/

  if (mk_tty)
    {
      printf("connecting to mk via tty: %s\n",mk_tty);
      if (!connect_mk_tty(mk_tty))
	printf("cant connect !!");
      else
	{
	  printf("connected !-)");
	  connected=TRUE;
	}
    }

  if (mk_socket_port)
    {
      printf("connecting to mk via local port: %i\n",mk_socket_port);
      
      if (connect_mk_localhost_socket(mk_socket_port)==-1)
	printf("cant connect !!");
      else
	{
	  printf("connected !-)");
	  connected=TRUE;
	}
    }

  // todo reenable  bluetooth connection

  connect_evdev();

  /*
  if ((input_evdev_name))
    {
      printf("\nInitializing evdev input (%s) ..\n",input_evdev_name);
      
      if (connect_evdev(input_evdev_name)) 
	{
	  printf(".. done");//
	  input=INPUT_EVDEV;
	}
      else
	printf(".. ERROR ");//
    }
  */
  if (input_joydev_name)
    {
      printf("\nInitializing joystick input from %s ..\n",input_joydev_name);
      if (connect_joy()) 
	{
	  printf(".. done");//
	  input=INPUT_JOYDEV;
	}
      else
	printf(".. ERROR ");//
    }


  printf("\nInitializing X-52 output ..");

  x52_output = x52_init();
    
  clear_display();

  write_display(0, "RIDDIM active");

  if (x52_output) x52_setbri(x52_output, 1,128);  
  if (x52_output) 
    printf(" done \n");  
  else
    printf(" not found \n");      

  /*
    if (!connected)
    {
    printf("Scanning for Bluetooth Devices ..\n");
    write_display(1,"Bluetooth Scan");
    scan_bt();
    printf(" done \n");  
    printf(" %d Devices found \n",bt_device_count);  
    print_device_list() ;
    }
  */

  //  int v_old;
  int polls=0;


  if (exit_after_init) 
    exit(0);
  printf("starting loop ..\n");



  int complete_misses=0;
  int complete_matches=0;


  int confirm_misses;



  //  init_evdevstatus_led();

  while( TRUE )    
    {

      gettimeofday(&loop_start_time,NULL);


      //      blink_evdev_led();
      bt_host_tick(mk_socket);
      usleep(loop_delay);

      poll_evdev();

      switch (input)
	{

	  
	case INPUT_NONE:
	  printf("processing input none\n");
	  break;

	case INPUT_EVDEV:
	  printf("processing input evdev\n");

	  
	  break;

	case INPUT_JOYDEV:
	  printf("processing input joydev\n");	 
	  // poll values from input device
	 
	  for (polls=0;polls<100;polls++) // FIXME - better Polling
	    {
	      read(joy_input_fd, &x52_event_struct, sizeof(struct js_event));
	      
	      
	      /* see what to do with the event */
	      switch (x52_event_struct.type & ~JS_EVENT_INIT)
		{
		case JS_EVENT_AXIS:
		  axis   [ x52_event_struct.number ] = x52_event_struct.value;
		  break;
		case JS_EVENT_BUTTON:
		  button [ x52_event_struct.number ] = x52_event_struct.value;
		  break;
		}
	    }
	  int x;
	  for( x=0 ; x<num_of_buttons ; ++x )
	    if( button[x]==0) 
	      button_trigger[x]=0;
	    else
	      {
		if (button_trigger[x]<100)button_trigger[x]++;
	      }
	  break;
	} // switch (input)

      printf("input done\n");	 
		
      switch(state)
	{
	case STATEID_SCANNING:
	  
	  state=STATEID_CONNECTING; 
	  /*
	  ExternControl.Digital[0]=0;
	  ExternControl.Digital[1]=0;
	  ExternControl.RemoteTasten=0;
	  ExternControl.Nick=(axis[1]>>8)*(-1)+127;;

	  printf("nick%d\n",ExternControl.Nick);	  
	  ExternControl.Roll=(axis[0]>>8)*(-1)+127;;
	  ExternControl.Gier=(axis[5]>>8)*(-1)+127;;
	  ExternControl.Gas=(axis[2]>>8)*(-1)+127;
	  ExternControl.Higt=0;
	  ExternControl.free=0;
	  ExternControl.Frame='t';
	  ExternControl.Config=1;

	  printf("sending data\n");

	  
	  if (connected)SendOutData('b', 0, (unsigned char *)&ExternControl, sizeof(ExternControl));
	  gettimeofday(&time_struct1,NULL);

	  if (button_trigger[BUTTON_SELECT]==1)
	    {
	      state=STATEID_CONNECTING;
	      clear_display();
	      write_display(0,"connecting to");
	      write_display(1,names[selected_bt_device]);
	      //connect_mk(addrs[selected_bt_device]);
	      write_display(0,"connected to");
	    }
		    
	  if ((button_trigger[BUTTON_UP]+button_trigger[BUTTON_DOWN])==1)
	    {
	      printf("-> sel_dev %d - %d\n",selected_bt_device,button_trigger[19]);
	      if (button_trigger[BUTTON_DOWN]==1)
		if (selected_bt_device>0) selected_bt_device--;
	      if (button_trigger[BUTTON_UP]==1)
		if (selected_bt_device<bt_device_count-1) selected_bt_device++;
	      

	    }
	  */
	  break;
	
	case STATEID_CONNECTING:
	  



	  
	  confirm_misses=0;

	  RxBuffer[2]=0;
	  if (connected) 
	    {
	      read_from_mk();
	      if (RxBuffer[2]=='B')
		complete_misses++;
	      /*while (RxBuffer[2]!='B')
	    {

	      RxBuffer[1]=0;
	      read_from_mk();
	      //	      bt_host_send(RxBuffer,rx_last_length);
	      printf("sending to host: %s",PrintableRxBuffer);


	      //	  ftime(&time_struct);

	      printf("waiting for confirm frame ( confirmed:%d misses:%d %c)\n",complete_matches,complete_misses,RxBuffer[2]);
	      //	      RxBuffer[2]=0;

	      //	      r=0;

	      // new
	      
	      //if (button_trigger[12]>1)
	      //	{
	      //	  SendOutData('s', 0, (unsigned char *)&ExternControl, sizeof(ExternControl));
	      //	  button_trigger[12]=0;
	      //	}
	      
	      ExternControl.Frame='t';
	      if (++confirm_misses>4)
		{
		  complete_misses++;
		  printf("sending again\n");
		  SendOutData('b', 0, (unsigned char *)&ExternControl, sizeof(ExternControl));
		}
	      */
	    }
	  else
	    printf("not connected to mk\n");
	  
	  //	  gettimeofday(&time_struct2,NULL);

	  //	  printf("last trip: %d\n",(int)(time_struct2.tv_usec-time_struct1.tv_usec));
	  //	  act_mode=button[24] | (button[25]<<1);



	  // Step converting axis data to nick/roll/gier/gas/..

	  //	  act_nick=(evdev_rel_axis[rel_axis_nick]-128)*nick_mul;




	  /* Mix input values */

	  act_gas=0;
	  act_nick=0;
	  act_roll=0;
	  act_gier=0;

	  int act_input=0;
	  for (act_input=0;act_input<input_count;act_input++)
	    {

	    printf("process b %d\n",inputs[act_input].nick_up_btn);
	      //process buttons
	    if (inputs[act_input].nick_up_btn!=-1)
	      {
		if (inputs[act_input].evdev_button[inputs[act_input].nick_up_btn]!=0)
		  act_nick=100;
		
	      }


	    if (inputs[act_input].nick_down_btn!=-1)
	      {
		if (inputs[act_input].evdev_button[inputs[act_input].nick_down_btn]!=0)
		  act_nick=-100;
		
	      }


	    if (inputs[act_input].roll_left_btn!=-1)
	      {
		if (inputs[act_input].evdev_button[inputs[act_input].roll_left_btn]!=0)
		  act_roll=100;
		
	      }

	    if (inputs[act_input].roll_right_btn!=-1)
	      {
		if (inputs[act_input].evdev_button[inputs[act_input].roll_right_btn]!=0)
		  act_roll=-100;
		

	      }

	    // process axis
	    
	    if (inputs[act_input].rel_axis_nick!=-1)
	      act_nick=inputs[act_input].evdev_rel_axis[inputs[act_input].rel_axis_nick]*inputs[act_input].nick_mul;

	    if (inputs[act_input].rel_axis_roll!=-1)
	      act_roll=inputs[act_input].evdev_rel_axis[inputs[act_input].rel_axis_roll]*inputs[act_input].roll_mul;


	    if (inputs[act_input].rel_axis_gier!=-1)
	      act_gier=inputs[act_input].evdev_rel_axis[inputs[act_input].rel_axis_gier]*inputs[act_input].gier_mul;


	    if (inputs[act_input].rel_axis_gas!=-1)
	      act_gas=inputs[act_input].evdev_rel_axis[inputs[act_input].rel_axis_gas]*inputs[act_input].gas_mul;



	    if (inputs[act_input].rel_axis_alt!=-1)
	     {

	       if (inputs[act_input].evdev_rel_axis[inputs[act_input].rel_axis_alt]>300)
		  act_long_alt-=last_trip_time/100;

	       if (inputs[act_input].evdev_rel_axis[inputs[act_input].rel_axis_alt]<-300)
		 act_long_alt+=last_trip_time/100;
		   
		act_alt=act_long_alt/1000;

		if (act_alt>120)act_alt=120;
		else if (act_alt<-120)act_alt=-120;
	     }
	    // process_events
	    
	    if (inputs[act_input].engine_switch_btn!=-1)
	      {
		
		if ((inputs[act_input].evdev_button[inputs[act_input].engine_switch_btn]==0)&&(DebugOut.Analog[16]==1))
		  {
		    ExternEvent.key=2;
		    SendOutData('e', 0, (unsigned char *)&ExternEvent, sizeof(ExternEvent));
		  }
		if ((inputs[act_input].evdev_button[inputs[act_input].engine_switch_btn]!=0)&&(DebugOut.Analog[16]==0))
		  {
		    ExternEvent.key=1;
		    SendOutData('e', 0, (unsigned char *)&ExternEvent, sizeof(ExternEvent));
		  }



	      }

	    }

	  switch(input)
	    {
	    case INPUT_EVDEV:
	      /*
	      act_nick=(evdev_rel_axis[rel_axis_nick]-nick_add)*nick_mul;
	      act_roll=(evdev_rel_axis[rel_axis_roll]-nick_add)*roll_mul;
	      act_gier=(evdev_rel_axis[rel_axis_gier]-nick_add)*gier_mul;
	      act_gas=(evdev_rel_axis[rel_axis_gas]-nick_add)*gas_mul;
	      */

	      break;

	    case INPUT_JOYDEV:
	      act_nick=(axis[rel_axis_nick])*nick_mul;
	      act_roll=(axis[rel_axis_roll])*roll_mul;
	      act_gier=(axis[rel_axis_gier])*gier_mul;
	      act_gas=(axis[rel_axis_gas]*-1+33000)*gas_mul;
     
	      break;
	    }

	  // act values clipping to usefull vals
	  //	  act_gas=0;

	  //	  act_gas=255;


	  /*
	    switch (act_mode)
	    {
	    case 0:


	    act_nick=(axis[AXIS_NICK])*(INVERT_NICK);
	    act_roll=(axis[AXIS_ROLL])*(INVERT_ROLL);
	    act_gier=(axis[AXIS_GIER])*(INVERT_GIER);
	    act_gas=((axis[AXIS_GAS])-128)*(-1);
	      
	    // clip gas      
	    if (act_gas<0) act_gas=0;

	    if (act_gas>250) act_gas=250;	    

	    ////////	      act_gas=0;
	    break;

	    case 1:
	    act_nick=(axis[AXIS_NICK]>>8)*(INVERT_NICK)/2;
	    act_roll=(axis[AXIS_ROLL]>>8)*(INVERT_ROLL)/2;
	    act_gier=(axis[AXIS_GIER]>>8)*(INVERT_GIER)/2;
	    act_gas=(axis[AXIS_GAS]>>8)*(INVERT_GAS);

	    break;
	      
	    case 2:
	    act_nick=(axis[AXIS_NICK]>>8)*(INVERT_NICK)/3;
	    act_roll=(axis[AXIS_ROLL]>>8)*(INVERT_ROLL)/3;
	    act_gier=(axis[AXIS_GIER]>>8)*(INVERT_GIER)/3;
	    act_gas=(axis[AXIS_GAS]>>8)*(INVERT_GAS);


	    break;

	    }
	  */  
	  ExternControl.Digital[0]=0;
	  ExternControl.Digital[1]=0;
	  ExternControl.RemoteTasten=0;
	  ExternControl.Higt=act_alt;
	  ExternControl.free=0;
	  ExternControl.Frame='t';
	  ExternControl.Config=1;


	  ExternControl.Nick=act_nick;  //(axis[1]>>8)*(-1)/2;
	  ExternControl.Roll=act_roll*(-1); //(axis[0]>>8)*(-1)/2;
	  ExternControl.Gier=act_gier; // ************
	  ExternControl.Gas=act_gas; // ************
	  ExternControl.Gas=255; // ************
	  


	  printf("act_mode %d , act_nick %d , act_roll %d , act_gier %d , act_gas %d , act_alt %d",act_mode , act_nick  , act_roll  , act_gier , act_gas,act_alt);

	  if (connected)
	    {
	      complete_matches++;
	      printf("sending data\n");
	      
	      SendOutData('b', 0, (unsigned char *)&ExternControl, sizeof(ExternControl));
	      //	      gettimeofday(&time_struct1,NULL);
	      printf("sent data\n");
	    }
	  
	  // printf("sleeping\n");
	  // 	  for (polls=0;polls<100;polls++) // FIXME - better Polling
	  // printf("end_sleep\n");
	      
	  //	  int v=axis[6]/655+50;
	  //	  if (v!=v_old)if (x52_output) x52_setbri(x52_output, 0,v );  
	  //	  v_old=v;
	  
	  //	  printf("v: %d \n",v);
	      
	  
	  /*
	    for (i=0;i<num_of_axis;i++)
	    printf("A%d: %d  ", i,axis[i]>>8 );
	  
	    for( x=0 ; x<num_of_buttons ; ++x )
	    
	    printf("B%d: %d  ", x, button[x] );		    
	  */

	  break;
	}
      
      printf("\n");
      fflush(stdout);
      printf("loop fin ( confirmed:%d misses:%d | debug_sets:%d )\n",complete_matches,complete_misses,debug_sets);
      printf("------------------------------------------------------------------------\n");
	
      gettimeofday(&loop_end_time,NULL);
      last_trip_time=(unsigned long)(loop_end_time.tv_usec-loop_start_time.tv_usec)+(unsigned long)(loop_end_time.tv_sec-loop_start_time.tv_sec)*1000000;
      printf("last trip: %ld\n",last_trip_time);

      trip_count++;
	
    }


  /******************** Cleanup **********************/
  close(joy_input_fd); 
  close(mk_socket);

  if (x52_output) x52_close(x52_output);
  return 0;
}
