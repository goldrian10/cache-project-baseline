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

#define KB 1024
#define ADDRSIZE 32
using namespace std;

int field_size_get(struct cache_params cache_params,
                   struct cache_field_size *field_size)
{
 
  return OK;
}

void address_tag_idx_get(long address,
                         struct cache_field_size field_size,
                         int *idx,
                         int *tag)
{


}

int srrip_replacement_policy (int idx,
                             int tag,
                             int associativity,
                             bool loadstore,
                             entry* cache_blocks,
                             operation_result* result,
                             bool debug)
{
   return ERROR;
}


int lru_replacement_policy (int idx,
                             int tag,
                             int associativity,
                             bool loadstore,
                             entry* cache_blocks,
                             operation_result* result,
                             bool debug)
{
   return ERROR;
}
int nru_replacement_policy(int idx,
                           int tag,
                           int associativity,
                           bool loadstore,
                           entry* cache_blocks,
                           operation_result* operation_result,
                           bool debug)
{
   return ERROR;
}

int l1_line_invalid_set(int tag,
                        int associativity,
                        entry* cache_blocks,
                        bool debug)
{
   for (size_t i = 0; i < associativity; i++)
   {
      if(cache_blocks[i].tag == tag){
         if (debug) printf("Tag of invalidated entry : 0x%X\n", cache_blocks->tag);
         cache_blocks[i].valid = 0;
         return INVALIDATED;
      }
   }
   return NON_INVALIDATED;
}
