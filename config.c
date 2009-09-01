#include "config.h"


 
char *input_evdev_name;
char *input_joydev_name;

char *bluetooth_mac;
char *mk_tty;


char *evdev_path;

int  mk_socket_port=0;
int  loop_delay=0;

double nick_mul=0.3f;
double roll_mul=0.3f;
double gier_mul=0.3f;
double gas_mul=0.3f;


double nick_add=0.3f;
double roll_add=0.3f;
double gier_add=0.3f;
double gas_add=0.3f;

int rel_axis_nick;
int rel_axis_roll;
int rel_axis_gier;
int rel_axis_gas;

cfg_bool_t exit_after_init = cfg_false;


int input_count=0;
int configured_input_count=0;
cfg_t *cfg;

void parse_config(char* fname)
{

  // section for inputs
  static cfg_opt_t input_opts[] = {
    CFG_STR("name", 0, CFGF_NONE),    

    CFG_FLOAT("nick_mul", 1.0,CFGF_NONE),
    CFG_FLOAT("roll_mul", 1.0,CFGF_NONE),
    CFG_FLOAT("gier_mul", 1.0,CFGF_NONE),
    CFG_FLOAT("gas_mul", 1.0,CFGF_NONE),
    CFG_FLOAT("alt_mul", 1.0,CFGF_NONE),

    CFG_FLOAT("nick_add", 0,CFGF_NONE),
    CFG_FLOAT("roll_add", 0,CFGF_NONE),
    CFG_FLOAT("gier_add", 0,CFGF_NONE),
    CFG_FLOAT("gas_add", 0,CFGF_NONE),
    
    CFG_INT("rel_axis_nick",  -1,CFGF_NONE),
    CFG_INT("rel_axis_roll",  -1,CFGF_NONE),
    CFG_INT("rel_axis_gier",  -1,CFGF_NONE),
    CFG_INT("rel_axis_gas", -1,CFGF_NONE),
    CFG_INT("rel_axis_alt", -1,CFGF_NONE),

    CFG_INT("nick_up_btn",  -1,CFGF_NONE),
    CFG_INT("nick_down_btn",  -1,CFGF_NONE),
    CFG_INT("roll_left_btn",  -1,CFGF_NONE),
    CFG_INT("roll_right_btn", -1,CFGF_NONE),


    CFG_INT("engine_switch_btn", -1,CFGF_NONE),

    CFG_END()
  };

  cfg_opt_t opts[] = {
    CFG_SEC("input", input_opts, CFGF_MULTI | CFGF_TITLE),
    CFG_SIMPLE_STR("bluetooth_mac", &bluetooth_mac),    

    CFG_SIMPLE_STR("mk_tty", &mk_tty),    
    CFG_SIMPLE_STR("evdev_path", &evdev_path),    

    CFG_SIMPLE_BOOL("exit_after_init", &exit_after_init),
    CFG_SIMPLE_STR("input_evdev", &input_evdev_name),
    CFG_SIMPLE_STR("input_joydev", &input_joydev_name),
    CFG_SIMPLE_INT("loop_delay", &loop_delay),
    CFG_SIMPLE_INT("mk_socket_port", &mk_socket_port),
    
    CFG_SIMPLE_FLOAT("nick_mul", &nick_mul),
    CFG_SIMPLE_FLOAT("roll_mul", &roll_mul),
    CFG_SIMPLE_FLOAT("gier_mul", &gier_mul),
    CFG_SIMPLE_FLOAT("gas_mul", &gas_mul),

    CFG_SIMPLE_FLOAT("nick_add", &nick_add),
    CFG_SIMPLE_FLOAT("roll_add", &roll_add),
    CFG_SIMPLE_FLOAT("gier_add", &gier_add),
    CFG_SIMPLE_FLOAT("gas_add", &gas_add),
    
    CFG_SIMPLE_INT("rel_axis_nick", &rel_axis_nick),
    CFG_SIMPLE_INT("rel_axis_roll", &rel_axis_roll),
    CFG_SIMPLE_INT("rel_axis_gier", &rel_axis_gier),
    CFG_SIMPLE_INT("rel_axis_gas", &rel_axis_gas),
    
    CFG_END()
  };
  

  printf("Parsing config file %s\n",fname);
  
  cfg = cfg_init(opts, 0);

  cfg_parse(cfg,fname);

}

void parse_config_input_sections()
{
  printf("parsing inputs \n");
  configured_input_count=cfg_size(cfg,"input");
 
  printf("%d inputs configured\n", input_count);
  int i;
  int act_input=0;
  for (i=0;i<configured_input_count;i++)
    {
      cfg_t *input_sect=cfg_getnsec(cfg,"input",i);


      printf("processing input: %s\n",cfg_getstr( input_sect, "name"));
      int evdev_i=0;
      
      for (evdev_i=0;evdev_i<evdevs_count;evdev_i++)
	if (!strcmp(cfg_getstr( input_sect, "name"),evdevs[evdev_i].name))
	  {
	    sprintf( inputs[act_input].fname,"%s",evdevs[evdev_i].fname);
       	  
	    // todo check lengt
	    sprintf(inputs[act_input].name,"processing input: %s\n",cfg_getstr( input_sect, "name"));
	    
	    inputs[act_input].nick_mul=cfg_getfloat(input_sect,"nick_mul");
	    inputs[act_input].roll_mul=cfg_getfloat(input_sect,"roll_mul");
	    inputs[act_input].gier_mul=cfg_getfloat(input_sect,"gier_mul");
	    inputs[act_input].gas_mul=cfg_getfloat(input_sect,"gas_mul");
	    inputs[act_input].alt_mul=cfg_getfloat(input_sect,"alt_mul");
	    
	    inputs[act_input].nick_add=cfg_getfloat(input_sect,"nick_add");
	    inputs[act_input].roll_add=cfg_getfloat(input_sect,"roll_add");
	    inputs[act_input].gier_add=cfg_getfloat(input_sect,"gier_add");
	    inputs[act_input].gas_add=cfg_getfloat(input_sect,"gas_add");


	    inputs[act_input].nick_up_btn=cfg_getint(input_sect,"nick_up_btn");
	    printf("nick_up_btn %ld\n" ,cfg_getint(input_sect,"nick_up_btn"));

	    inputs[act_input].nick_down_btn=cfg_getint(input_sect,"nick_down_btn");

	    inputs[act_input].roll_left_btn=cfg_getint(input_sect,"roll_left_btn");	    
	    inputs[act_input].roll_right_btn=cfg_getint(input_sect,"roll_right_btn");


	    inputs[act_input].rel_axis_nick=cfg_getint(input_sect,"rel_axis_nick");
	    inputs[act_input].rel_axis_roll=cfg_getint(input_sect,"rel_axis_roll");

	    inputs[act_input].rel_axis_gier=cfg_getint(input_sect,"rel_axis_gier");
	    inputs[act_input].rel_axis_gas=cfg_getint(input_sect,"rel_axis_gas");

	    inputs[act_input].rel_axis_alt=cfg_getint(input_sect,"rel_axis_alt");

	    inputs[act_input].engine_switch_btn=cfg_getint(input_sect,"engine_switch_btn");



	    act_input++;
	  }


    }
  
  input_count=act_input;
  printf("%d inputs matching\n", input_count);

}


