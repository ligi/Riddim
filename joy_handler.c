#include "joy_handler.h"


int connect_joy()
{
  
  axis = (int *) calloc( 100, sizeof( int ) );
  button = (char *)calloc( 100, sizeof( char ) );
  button_trigger = (char *) calloc( 100, sizeof( char ) );

  
  //  axis = (int *) calloc( num_of_axis, sizeof( int ) );
  // button = (char *)calloc( num_of_buttons, sizeof( char ) );
  // button_trigger = (char *) calloc( num_of_buttons, sizeof( char ) );

  
  if( ( joy_input_fd = open( input_joydev_name, O_RDONLY ) ) < 0 )
    {
      printf( "can't open %s !\nHint: try modprobe joydev\n",input_joydev_name  );
      return 0;
    }

  ioctl( joy_input_fd, JSIOCGAXES, &num_of_axis );
  ioctl( joy_input_fd, JSIOCGBUTTONS, &num_of_buttons );
  ioctl( joy_input_fd, JSIOCGNAME(80), &name_of_joystick );

  printf("Joystick detected: %s\n\t%d axis\n\t%d buttons\n\n"
	 , name_of_joystick
	 , num_of_axis
	 , num_of_buttons );
  
  fcntl( joy_input_fd, F_SETFL, O_NONBLOCK );   /* use non-blocking mode */

  return 1;
}

/*
int connect_joy()
{
  
  axis = (int *) calloc( 100, sizeof( int ) );
  button = (char *)calloc( 100, sizeof( char ) );
  button_trigger = (char *) calloc( 100, sizeof( char ) );

  
  //  axis = (int *) calloc( num_of_axis, sizeof( int ) );
  // button = (char *)calloc( num_of_buttons, sizeof( char ) );
  // button_trigger = (char *) calloc( num_of_buttons, sizeof( char ) );

  
  if( ( x52_input_fd = open( JOY_DEV, O_RDONLY ) ) < 0 )
    {
      printf( "Couldn't open joystick device %s\n", JOY_DEV );
      printf( "try modprobe joydev\n"  );
      return 0;
    }

  ioctl( x52_input_fd, JSIOCGAXES, &num_of_axis );
  ioctl( x52_input_fd, JSIOCGBUTTONS, &num_of_buttons );
  ioctl( x52_input_fd, JSIOCGNAME(80), &name_of_joystick );

  printf("Joystick detected: %s\n\t%d axis\n\t%d buttons\n\n"
	 , name_of_joystick
	 , num_of_axis
	 , num_of_buttons );
  
  fcntl( x52_input_fd, F_SETFL, O_NONBLOCK );  

  return 1;
}
*/
