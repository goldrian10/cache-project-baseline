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

int lru_obl_replacement_policy (int idx,
                                int tag,
                                int associativity,
                                bool loadstore,
                                entry* cache_block,
				entry* cache_block_obl,
                                operation_result* operation_result_cache_block,
				operation_result* operation_result_cache_obl,
                                bool debug=false)
{
   return OK;
}
