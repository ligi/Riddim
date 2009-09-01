#include "statistics.h"

struct timeval loop_start_time;
struct timeval loop_end_time;

struct timeval time_struct1;
struct timeval time_struct2;


int debug_sets=0;
int confirm_sets=0;
int version_sets=0;

unsigned long last_trip_time=0;
unsigned long trip_count=0;
