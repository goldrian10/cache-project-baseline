/*
 * 	Cache simulation project
 * 	Class UCR IE-521
 * 	Semester: I-2019
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <math.h>
#include "debug_utilities.h"


/* Get enviroment var */
void get_env_var( const char* var_name,
                 int *var_value )
{
    char* pPath = getenv(var_name);
    if (pPath==NULL) {
      *var_value = 0;
	} else {
	     *var_value= atoi(pPath);
	}
}


bool is_in_set(entry* set, int assoc, int tag){
  for (size_t i = 0; i < assoc; i++){
    if(set[i].tag == tag){
      return true;
    }
  }
  return false;
}

parameters random_params(){
  parameters params = {};
  params.a   = (u_int) (1 << (     rand()%4 ));
  params.l   = (u_int) (1 << ( 3 + rand()%4 ));
  params.t   = (u_int) (1 << ( 8 + rand()%4 ));
  params.opt = (u_int) (rand()%2);
  return params;
}

line_info random_access(int loadstore){
  line_info info = {};
  info.address   = (long) rand()%4294967295;
  info.loadstore = (loadstore == RAND)? rand()%2:loadstore;
  info.ic        = rand()%10;
  return info;
}

bool is_in_set(entry* set, int assoc, int tag){
  for (size_t i = 0; i < assoc; i++){
    if(set[i].tag == tag){
      return true;
    }
  }
  return false;
}

parameters random_params(){
  parameters params = {};
  params.a   = (u_int) (1 << (     rand()%4 ));
  params.l   = (u_int) (1 << ( 3 + rand()%4 ));
  params.t   = (u_int) (1 << ( 8 + rand()%4 ));
  params.opt = (u_int) (rand()%2);
  return params;
}

line_info random_access(int loadstore){
  line_info info = {};
  info.address   = (long) rand()%4294967295;
  info.loadstore = (loadstore == RAND)? rand()%2:loadstore;
  info.ic        = rand()%10;
  return info;
}

/* Print way info */
void print_way_info(int idx,
                    int associativity,
                    entry* cache_blocks)
{
  int j = 0;
  for (j = 0 ; j< associativity; j++) {
	printf(CYN "INFO: " RESET "Way #%d: tag: %d--- valid: %d rp_value: %d --- dirty: %d\n", j, cache_blocks[j].tag, cache_blocks[j].valid,cache_blocks[j].rp_value, cache_blocks[j].dirty);
  }
}
