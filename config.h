#if !defined(CONFIG_H)
#define CONFIG_H


#define MAX_INPUTS 5

#include <confuse.h>
#include "evdev_handler.h"

// values from config 
extern char *input_evdev_name;
extern char *input_joydev_name;
extern char *bluetooth_mac;
extern char *evdev_path;
extern int  mk_socket_port;
extern char *mk_tty;
extern int  loop_delay;

extern double nick_mul;
extern double roll_mul;
extern double gier_mul;
extern double gas_mul;

extern double nick_add;
extern double roll_add;
extern double gier_add;
extern double gas_add;

extern int rel_axis_nick;
extern int rel_axis_roll;
extern int rel_axis_gier;
extern int rel_axis_gas;


extern cfg_bool_t exit_after_init ;


extern int input_count;
extern int configured_input_count;
struct  {


  int  *evdev_rel_axis;
  char *evdev_button;

  double nick_mul;
  double roll_mul;
  double gier_mul;
  double gas_mul;
  double alt_mul;

  double nick_add;
  double roll_add;
  double gier_add;
  double gas_add;
  double alt_add;

  int rel_axis_nick;
  int rel_axis_roll;
  int rel_axis_gier;
  int rel_axis_gas;
  int rel_axis_alt;

  int nick_up_btn;
  int nick_down_btn;

  int roll_left_btn;
  int roll_right_btn;

  int engine_switch_btn;

  char name[255];
  char fname[255];


  int evdev_id;

  int evdev_out_fd;
  int evdev_in_fd;

} inputs[MAX_INPUTS];


void parse_config(char* fname);
void parse_config_input_sections();

#endif
