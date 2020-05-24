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
#include <debug_utilities.h>

#define CYN "\x1B[36m"
#define RESET "\x1B[0m"

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
