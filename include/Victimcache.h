/*
 *  Cache simulation project
 *  UCR IE-521
 */

#ifndef VCACHE_H
#define VCACHE_H
#include <utilities.h>
/*
 * STRUCTS
 */

/* L1 and VC info */

struct l1_vc_entry_info {
	int l1_tag;
	int l1_idx;
	int l1_associativity;
    int vc_associativity;
};


/* 
 *  Functions
 */

/* 
 * Search for an address in a cache set and
 * replaces blocks using LRU policy L1  and FIFO in a victim cache
 * 
 * [in] l1_vc_info: struct with victim cache and l1 entry info (this is a ro parameter)
 * [in] loadstore: type of operation true if load false if store
 * [in] debug: if set to one debug information is printed
 *
 * [in/out] l1_cache_blocks: l1 cache line to work on 
 * [in/out] vc_cache_blocks: vc cache line
 * [in/out] l1_result: return the cache operation result in L1 (miss_hit_status)
 * [in/out] vc_result: return the cache operation result in vc (miss_hit_status)
 * [out] result: result of the operation (returns_types)
 */
int lru_replacement_policy_l1_vc(const entry_info *l1_vc_info,
      	                      	 bool loadstore,
								 entry* l1_cache_blocks,
								 entry* vc_cache_blocks,
            	                 operation_result* l1_result,
              	              	 operation_result* vc_result,
								 bool debug=false);

#endif
