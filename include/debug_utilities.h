/*
 * 	Cache simulation project
 * 	Class UCR IE-521
 * 	Semester: I-2019
*/
#ifndef DEBUG_UTILITIES
#define DEBUG_UTILITIES
#include "L1cache.h"

/* MACROS */
#define DEBUG(x) if (debug_on) printf("%s\n",#x)
#define YEL   "\x1B[33m"

/* FUNCTIONS */

/* Get enviroment var */
void get_env_var( const char* var_name,
                 int *var_value );

/* Print way info */
void print_way_info(int idx,
                    int associativity,
                    entry* cache_blocks); 
                    

bool is_in_set(entry* set, int assoc, int tag);

parameters random_params();

line_info random_access(int loadstore=-1);

entry_info generate_entry_info(sizes sizes);

#endif
