#include "evdev_handler.h"
#include "config.h"

int retval;
size_t read_bytes;  /* how many bytes were read */
struct input_event ev[64]; /* the events (up to 64 at once) */
struct input_event led_event;



int counter;

int evdevs_count=0;
int file_select(const struct direct   *entry)
{
  
  if ((strncmp(entry->d_name, "event",5))==0)
    return (TRUE);
  else
    return(FALSE);
}


void collect_evdev_devices()
{

  //  char event_dev_path[]="/dev/input/";
 
  struct direct **files;
  evdevs_count = scandir(evdev_path, &files, file_select, NULL);

  printf("%d inputs found in %s\n" , evdevs_count,evdev_path);

  int i=0;
  for (i=0;i<evdevs_count;++i)
    {

      printf("%s",files[i]->d_name);
      sprintf(evdevs[i].fname,"%s%s",evdev_path,files[i]->d_name);
      int act_fd;

      if ((act_fd = open(evdevs[i].fname, O_RDONLY)) < 0) 
	{
	  printf(" cant open %s for reading!",evdevs[i].name);
	  
	}
      else
	{
	  if(ioctl(act_fd , EVIOCGNAME(sizeof(evdevs[i].name)), evdevs[i].name) < 0) {
	    perror("evdev ioctl");
	  }
	  
	  printf("EVDEV reports name: %s\n", evdevs[i].name);
	  close(act_fd);
	}


      /*
      FILE *fp=NULL ;

      sprintf(evdevs[i].fname,"/sys/class/input/%s/name",files[i-1]->d_name);
      fp = fopen ( evdevs[i].fname, "r") ;

      if (fp==NULL)
	{
	  printf("cannot read %s\n",evdevs[i].fname);
	  //  exit(1);
	}
      int ch=-2;
      int str_pos=0;
      while ( (ch!=-1) && (ch!='\n') )
	{

	  ch = fgetc ( fp ) ;
	  if ((ch!=13)&&(ch!=10))evdevs[i].name[str_pos++]=ch;
	  printf("%c",ch);
	}

      fclose(fp);
      evdevs[i].name[str_pos++]=0;
      */

    }
}


void print_event_str(int id)
{
  switch ( id)
    {
    case EV_KEY :
      printf("Keys or Button\n");
      break;
    case EV_ABS :
      printf("Absolute Axis\n");
      break;
    case EV_LED :
      printf("LED(s)\n");
      break;
    case EV_REP :
      printf("Repeat\n");
      break;
    case EV_SYN :
      printf("Sync?\n");
      break;
    case EV_REL :
      printf("Relative Axis\n");
      break;
    case EV_MSC :
      printf("Misc\n");
      break;
    default:
      printf("Unknown event type ( 0x%04hx)\n", id);
    }
  

}

int connect_evdev()
{ 


  struct input_devinfo {
    uint16_t bustype;
    uint16_t vendor;
    uint16_t product;
    uint16_t version;
  };
  struct input_devinfo device_info;

  int i;
  for ( i =0 ; i< input_count;i++)
    {
      inputs[i].evdev_rel_axis = (int *) calloc( 100, sizeof( int ) );
      inputs[i].evdev_button = (char *)calloc( 500, sizeof( char ) );


      printf(" opening evdev %s\n",inputs[i].fname);
      if ((inputs[i].evdev_out_fd = open(inputs[i].fname, O_WRONLY)) < 0) 
	{
	  printf(" cant open %s for writing\n",inputs[i].fname);
	  
	}

      if ((inputs[i].evdev_in_fd = open(inputs[i].fname, O_RDONLY)) < 0) 
	{
	  printf(" cant open %s for reading!",inputs[i].fname);
	  return 0; 
	}
  
      ioctl(inputs[i].evdev_in_fd ,EVIOCGID,&device_info);
    
      printf("vendor 0x%04hx product 0x%04hx version 0x%04hx \n",
	     device_info.vendor, device_info.product,
	     device_info.version);


      uint8_t evtype_bitmask[EV_MAX/8 + 1];
      
      if(ioctl(inputs[i].evdev_in_fd , EVIOCGBIT(0, sizeof(evtype_bitmask)), evtype_bitmask) < 0)
	perror("evdev ioctl");
      
      printf("Supported event types:\n");
      
      int i;
      for (i = 0; i < EV_MAX; i++) {
	if (test_bit(i, evtype_bitmask)) {
	  /* this means that the bit is set in the event types list */
	  printf("  Event type 0x%02x ", i);
	  
	  print_event_str(i);
	}}
    }
  return 1;
}

/*
int init_evdevstatus_led()
{
  led_event.type=EV_LED;
  led_event.code = LED_MISC;
  led_event.value = 1;
  if (evdev_out_fd)
    retval = write(evdev_out_fd, &led_event, sizeof(struct input_event));
  return 1;
}

int blink_evdev_led()
{

  if (evdev_out_fd)
    {
      if (led_event.value)
	led_event.value = 0;
      else 
	led_event.value = 1 ;
      
      retval = write(evdev_out_fd, &led_event, sizeof(struct input_event));
    }
  return 1;
}
*/

int poll_evdev()
{

  printf("polling evdev\n");
  int i;
  for ( i =0 ; i< input_count;i++)
    {
      struct timeval tv = {0,5};
      
      fd_set rfds;
      FD_ZERO(&rfds);
      FD_SET(inputs[i].evdev_in_fd,&rfds);

      
      
      retval = select(inputs[i].evdev_in_fd+1, &rfds, NULL, NULL, &tv);
  
      if (retval==-1)
	printf("error in select!!!!!!!!\n");
      else if (retval)
	{
	  read_bytes = read(inputs[i].evdev_in_fd, ev, sizeof(struct input_event) * 64);
      
	  if (read_bytes < (int) sizeof(struct input_event)) {
	    perror("evtest: short read");
	    exit (1);
	  
}
	  
	  for (counter = 0; counter < (int) (read_bytes / sizeof(struct input_event)); counter++)
	    {
	      //    print_event_str(ev[counter].type); 
	      //      printf(" code:%d val:%d \n",ev[counter].code,ev[counter].value);
	      
	      //	  if (ev[counter].type==EV_REL) evdev_rel_axis[ ev[counter].code]= ev[counter].value;
	      // for logitech problem
	      if (ev[counter].type==EV_REL) inputs[i].evdev_rel_axis[ ev[counter].code]= ev[counter].value;
	      if (ev[counter].type==EV_KEY) inputs[i].evdev_button[ ev[counter].code]= ev[counter].value;
	    }
	  
	  
	  for (counter=0;counter<20;counter++)
	    printf("A%d %d -" , counter, inputs[i].evdev_rel_axis[counter] );
	  printf("\n");
	  
	  for (counter=0;counter<10;counter++)
	    printf("B%d %d -" , counter, inputs[i].evdev_button[counter] );
	  //	     	printf("input read done: nick:%d roll:%d gier:%d gas:%d\n",axis[3] , axis[4] , axis[5] , axis[2]); 
	}
      else
	printf("no data from evdev data from evdev: \n");
    }
  return 1;
}
