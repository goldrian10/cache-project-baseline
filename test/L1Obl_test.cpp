/*
 *  Cache simulation project
 *  Class UCR IE-521
 */

#include <gtest/gtest.h>
#include <time.h>
#include <stdlib.h>
#include <debug_utilities.h>
#include <L1cache.h>
#include <L1Obl.h>

using namespace std;

class OBLcache : public ::testing::Test{
  protected:
	int debug_on = 0;
	virtual void SetUp()
	{
	/* Parse for debug env variable */
	get_env_var("TEST_DEBUG", &debug_on);
	};
};

/*
 * TEST1: miss on new block and prefetch 
 * Choose a random associativity
 * Force a miss on block A
 * Check miss_hit_status == MISS_X
 * Check obl_tag for block A is 0
 * Check block A + 1 is updated
 * Check dirty bit of block A + 1 is 0
 * Check block A + 1 obl_block is 1 
 */
TEST_F(OBLcache, miss_prefetch) {
  int status;
  int i;
  int tag;
  enum miss_hit_status expected_miss_hit;
  bool loadstore = bool(rand()%2);
  bool line_found = false;
  bool line_A_obl = false;
  bool line_A_next_obl = false;
  bool dirty = true;
  int associativity = 1 << (rand()%4);
  
  entry cache_block[associativity] = {};
  entry cache_block_obl[associativity] = {};
  operation_result l1_result = {};
  operation_result obl_result = {};
  
  /* Get tag for block A and A + 1 */
  tag = rand()%4096;
  if (debug_on) {
    printf("Entry Info\n tag: %d\n associativity: %d\n",
          tag,
          associativity);
  }
  
  /* Fill cache line */
  for ( i =  0; i < associativity; i++) {
    cache_block[i].valid = true;
    cache_block[i].tag = rand()%4096;
    cache_block[i].dirty = 0;
    cache_block[i].rp_value = i;
    cache_block_obl[i].valid = true;
    cache_block_obl[i].tag = cache_block[i].tag;
    cache_block_obl[i].dirty = 0;
    cache_block_obl[i].rp_value = i;
    while (cache_block[i].tag == tag) {
        cache_block[i].tag = rand()%4096;
	cache_block_obl[i].tag = cache_block[i].tag;
    }
  }
  /* idx field is redundant, but can be used */
  int idx = 0;
  status = lru_obl_replacement_policy(idx,
                                      tag,
                                      associativity,
                                      loadstore,
                                      cache_block,
				      cache_block_obl,
                                      &l1_result,
				      &obl_result);
  EXPECT_EQ(status, OK);
  expected_miss_hit = loadstore ? MISS_STORE: MISS_LOAD;
  EXPECT_EQ(l1_result.miss_hit, expected_miss_hit);
  
  /* Find block A in cache_block */
  for ( i =  0; i < associativity; i++) {
    if(cache_block[i].tag == tag) {
	line_found = true;
	line_A_obl = cache_block[i].obl_tag;
	break;
   }
 }
 EXPECT_TRUE(line_found);
 EXPECT_FALSE(line_A_obl);
 
 line_found = false;
 /* Find block A + 1 in cache_block */
  for ( i =  0; i < associativity; i++) {
    if(cache_block_obl[i].tag == tag) {
	line_found = true;
	line_A_next_obl = cache_block_obl[i].obl_tag;
        dirty = cache_block_obl[i].dirty;
	break;
   }
 }
 EXPECT_TRUE(line_found);
 EXPECT_TRUE(line_A_next_obl);
 EXPECT_FALSE(dirty);

}
 
/*
 * TEST2: hit on block with obl 0
 * Choose a random associativity
 * Fill a cache line cache_block with obl = 0
 * Fill a second cache line (cache_block_obl) with obl = 1
 * Force a hit on block A in cache_blocks
 * Check obl_tag for block A is 0
 * Check cache_block_obl remains the same
 */
TEST_F(OBLcache, hit_no_prefetch) {
  int status;
  int i;
  int tag;
  enum miss_hit_status expected_miss_hit;
  bool loadstore = bool(rand()%2);
  bool line_found = false;
  bool line_A_obl = false;
  bool dirty = true;
  int associativity = 1 << (rand()%4);
  
  entry cache_block[associativity] = {};
  entry cache_block_obl[associativity] = {};
  entry cache_block_obl_before[associativity] = {};
  
  operation_result l1_result = {};
  operation_result obl_result = {};
  
  /* Get tag for block A and A + 1 */
  tag = rand()%4096;
  
  /* Fill cache line */
  for ( i =  0; i < associativity; i++) {
    cache_block[i].valid = true;
    cache_block[i].tag = tag + i;
    cache_block[i].dirty = 0;
    cache_block[i].rp_value = i;
    cache_block[i].obl_tag = false;
    cache_block_obl[i].valid = true;
    cache_block_obl[i].tag = cache_block[i].tag;
    cache_block_obl[i].dirty = 0;
    cache_block_obl[i].rp_value = i;
    cache_block_obl[i].obl_tag =true;
  }
  /* idx field is redundant, but can be used */
  int idx = 0;
  memcpy(cache_block_obl_before, cache_block_obl, sizeof(struct entry));
  status = lru_obl_replacement_policy(idx,
                                      tag,
                                      associativity,
                                      loadstore,
                                      cache_block,
				      cache_block_obl,
                                      &l1_result,
				      &obl_result);
  EXPECT_EQ(status, OK);
  expected_miss_hit = loadstore ? HIT_STORE: HIT_LOAD;
  EXPECT_EQ(l1_result.miss_hit, expected_miss_hit);
  
  /* Find block A in cache_block */
  for ( i =  0; i < associativity; i++) {
    if(cache_block[i].tag == tag) {
	line_found = true;
	line_A_obl = cache_block[i].obl_tag;
	break;
   }
 }
 EXPECT_TRUE(line_found);
 EXPECT_FALSE(line_A_obl);
 int are_equal = memcmp(cache_block_obl_before, cache_block_obl, sizeof(struct entry)); 
 EXPECT_EQ(0, are_equal);
}

/*
 * TEST3: hit on block with obl 1
 * Choose a random associativity
 * Fill a cache line cache_block with obl = 1
 * Fill a second cache line cache_block_obl
 * Force a hit on block A in cache_blocks
 * Check obl bit for block A get set to 0
 * Check block A + 1 is inserted in cache_block_obl
 * Check block A + 1 obl bit is set to 1
 */
TEST_F(OBLcache, hit_prefetch) {
  int status;
  int i;
  int tag;
  enum miss_hit_status expected_miss_hit;
  bool loadstore = bool(rand()%2);
  bool line_found = false;
  bool line_A_obl = false;
  bool line_A_next_obl = false;
  bool dirty = true;
  int associativity = 1 << (rand()%4);
  
  entry cache_block[associativity] = {};
  entry cache_block_obl[associativity] = {};
  operation_result l1_result = {};
  operation_result obl_result = {};
  
  /* Get tag for block A and A + 1 */
  tag = rand()%4096;
  if (debug_on) {
    printf("Entry Info\n tag: %d\n associativity: %d\n",
          tag,
          associativity);
  }
  
  /* Fill cache line */
  for ( i =  0; i < associativity; i++) {
    cache_block[i].valid = true;
    cache_block[i].tag = tag + i;
    cache_block[i].dirty = 0;
    cache_block[i].rp_value = i;
    cache_block[i].obl_tag =  true;
    cache_block_obl[i].valid = true;
    cache_block_obl[i].tag = cache_block[i].tag;
    cache_block_obl[i].dirty = 0;
    cache_block_obl[i].rp_value = i;
  }
  /* idx field is redundant, but can be used */
  int idx = 0;
  status = lru_obl_replacement_policy(idx,
                                      tag,
                                      associativity,
                                      loadstore,
                                      cache_block,
				      cache_block_obl,
                                      &l1_result,
				      &obl_result);
  EXPECT_EQ(status, OK);
  expected_miss_hit = loadstore ? HIT_STORE: HIT_LOAD;
  EXPECT_EQ(l1_result.miss_hit, expected_miss_hit);
  
  for ( i =  0; i < associativity; i++) {
    if(cache_block[i].tag == tag) {
	line_found = true;
	line_A_obl = cache_block[i].obl_tag;
	break;
   }
 }
 EXPECT_TRUE(line_found);
 EXPECT_FALSE(line_A_obl);
 
 line_found = false;
 /* Find block A + 1 in cache_block */
  for ( i =  0; i < associativity; i++) {
    if(cache_block_obl[i].tag == tag) {
	line_found = true;
	line_A_next_obl = cache_block_obl[i].obl_tag;
        dirty = cache_block_obl[i].dirty;
	break;
   }
 }
 EXPECT_TRUE(line_found);
 EXPECT_TRUE(line_A_next_obl);
 EXPECT_FALSE(dirty);
}
