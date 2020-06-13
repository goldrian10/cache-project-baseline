/*
 *  Cache simulation project
 *  Class UCR IE-521
*/

#ifndef L1OBL_H
#define L1OBL_H

#include <netinet/in.h> 
#include "utilities.h"

/* 
 *  Functions
 */

/* 
 * Search for an address in a cache set and
 * replaces blocks using LRU policy and prefetch data using OBL tagged prefech algorithm 
 * 
 * [in] idx: index field of the block
 * [in] tag: tag field of the block
 * [in] associativity: number of ways of the entry
 * [in] loadstore: type of operation false if true if store
 * [in] debug: if set to one debug information is printed
 *
 * [in/out] cache_blocks: l1 cache line to work on 
 * [in/out] cache_blocks_obl: l1 cache line where obl will be located
 * [in/out] cache_blocks_result: return the cache operation result in L1 (miss_hit_status)
 * [in/out] cache_blocks_result_obl: return the cache operation result in L1 for obl block (miss_hit_status)
 * [out] result: result of the operation (returns_types)
 */
int lru_obl_replacement_policy (int idx,
                                int tag,
                                int associativity,
                                bool loadstore,
                                entry* cache_block,
				                entry* cache_block_obl,
                                operation_result* operation_result_cache_block,
				                operation_result* operation_result_cache_obl,
                                bool debug=false);
#endif
