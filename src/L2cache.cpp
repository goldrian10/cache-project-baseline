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
#include <L2cache.h>

#define KB 1024
#define ADDRSIZE 32
using namespace std;

int lru_replacement_policy_l1_l2(const l1_l2_entry_info *l1_l2_info,
				 bool loadstore,
				 entry* l1_cache_blocks,
				 entry* l2_cache_blocks,
				 operation_result* l1_result,
				 operation_result* l2_result,
				 bool debug) 
{
	return OK;
}
