/*
 *  Cache simulation project
 *  Class UCR IE-521
*/

#ifndef L1CACHE_H
#define L1CACHE_H

#include <netinet/in.h> 
/* 
 * ENUMERATIONS 
 */

/* Return Values */
enum returns_types {
 OK,
 PARAM,
 ERROR
};

/* Represent the cache replacement policy */
enum replacement_policy{
 LRU,
 NRU,
 RRIP,
 RANDOM 
};

enum miss_hit_status {
 MISS_LOAD,
 MISS_STORE,
 HIT_LOAD,
 HIT_STORE
};

/*
 * STRUCTS
 */

/* Cache entry metadata */

struct entry {
 /* Indicates if the line is valid */
 bool valid;

 /* Indicates if the entry was written */
 bool dirty;

 /* Tag value */
 int tag;

 /* Replacement policy value */
 uint8_t rp_value;

 /* Prefetch tag, only used in OBL */
 bool obl_tag;
};

/* Cache replacement policy results */
struct operation_result {
 /* Result of the operation */
 enum miss_hit_status miss_hit;

 /* Set to one if the evicted line was dirty */
 bool dirty_eviction;

 /* Block address of the evited line */
 int  evicted_address;
};

/* Cache params */
struct cache_params {
  /* Total size of the cache in Kbytes */
  int size;

  /* Number of ways of the cache */  
  int asociativity;

  /* Size of each cache line in bytes */
  int block_size;
};

/* Address field size */
struct cache_field_size {
  /* Number of bits used for the tag */
  int tag;

  /* Number of bits used for the idx */
  int idx;

  /* Number of bits used for the offset */
  int offset;
};

/* 
 *  Functions
 */

/*
 * Get tag, index and offset length
 * 
 * [in] cache_parms :      Cache size, asociativity and block size
 *
 * [out] cache_field_size: Struct containing tag, idx and offset size
 */
int field_size_get(struct cache_params cache_params,
                   struct cache_field_size *field_size);

/* 
 * Get tag and index from address
 * 
 * [in] address:    memory address
 * [in] field_size: Struct containing tag, idx and offset size in bits
 *
 * [out] idx: cache line idx
 * [out] tag: cache line tag
 */

void address_tag_idx_get(long address,
                         struct cache_field_size field_size,
                         int *idx,
                         int *tag);


/* 
 * Search for an address in a cache set and
 * replaces blocks using SRRIP(hp) policy
 * 
 * [in] idx: index field of the block
 * [in] tag: tag field of the block
 * [in] associativity: number of ways of the entry
 * [in] loadstore: type of operation false if load true if store
 * [in] debug: if set to one debug information is printed
 *
 * [in/out] cache_block: return the cache operation return (miss_hit_status)
 * [out] result: result of the operation (returns_types)
 */
int srrip_replacement_policy (int idx,
                              int tag,
                              int associativity,
                              bool loadstore,
                              entry* cache_blocks,
                              operation_result* operation_result,
                              bool debug=false);

/* 
 * Search for an address in a cache set and
 * replaces blocks using LRU policy
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
int lru_replacement_policy (int idx,
                           int tag,
                           int associativity,
                           bool loadstore,
                           entry* cache_blocks,
                           operation_result* operation_result,
                           bool debug=false);
/* 
 * Search for an address in a cache set and
 * replaces blocks using NRU policy
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

int nru_replacement_policy(int idx,
                           int tag,
                           int associativity,
                           bool loadstore,
                           entry* cache_blocks,
                           operation_result* operation_result,
                           bool debug=false);

#endif
/*
 * Invalidates a line in the cache
 *
 * [in] tag: tag field of the block
 * [in] associativity: number of ways of the entry
 * [in] debug: if set to one debug information is printed
 *
 * [in/out] cache_block: cache entry to edit
 *
 * return error if tag is not found in cache blocks
 */
int l1_line_invalid_set(int tag,
                        int associativity,
                        entry* cache_blocks,
                        bool debug);
