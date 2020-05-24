/*
 *  Cache simulation project
 *  Class UCR IE-521
*/

#ifndef L1OBL_H
#define L1OBL_H

#include <netinet/in.h> 

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
 * [in/out] cache_block: return the cache operation return (miss_hit_status)
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
