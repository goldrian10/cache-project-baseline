/*
 *  Cache simulation project
 *  Class UCR IE-521
 */

#include <gtest/gtest.h>
#include <time.h>
#include <stdlib.h>
#include <debug_utilities.h>
#include <L1cache.h>
#include <L2cache.h>

using namespace std;

class L2cache : public ::testing::Test{
	protected:
		int debug_on;
		virtual void SetUp()
		{
  		/* Parse for debug env variable */
  		get_env_var("TEST_DEBUG", &debug_on);
		};
};

/*
 * TEST1: 
 * <STEPS>
 */
TEST_F(L2cache,l1_miss_l2_hit){
  int status = OK;
  struct l1_l2_entry_info l1_l2_info;
  struct entry l1_cache_blocks;
  struct entry l2_cache_blocks;
  struct operation_result l1_result = {};
  struct operation_result l2_result = {};
  DEBUG(debug_on);
  status = lru_replacement_policy_l1_l2(&l1_l2_info,
					true,
					&l1_cache_blocks,
					&l2_cache_blocks,
					&l1_result,
					&l2_result);
  EXPECT_EQ(status, OK);
}
