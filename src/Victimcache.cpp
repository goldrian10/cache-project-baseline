/*
 *  Cache simulation project
 *  Class UCR IE-521
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <math.h>
#include <debug_utilities.h>
#include <L1cache.h>
#include <Victimcache.h>

#define KB 1024
#define ADDRSIZE 32
using namespace std;

int lru_replacement_policy_l1_vc(const l1_vc_entry_info *l1_vc_info,
    	                      	 bool loadstore,
       	                    	 entry* l1_cache_blocks,
       	                  	 entry* vc_cache_blocks,
        	                 operation_result* l1_result,
              	              	 operation_result* vc_result,
                	         bool debug)
{
   return OK;
}
