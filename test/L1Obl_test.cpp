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
 * TEST1: 
 * <STEPS>
 */
TEST_F(OBLcache,prefetch) {
  int status = OK;
  entry cache_block;
  entry cache_block_obl;
  operation_result l1_result = {};
  operation_result obl_result = {};
  int idx, tag, way = 0;
  DEBUG(debug_on,OBL test);
  status = lru_obl_replacement_policy(idx,
                                      tag,
                                      way,
                                      false,
                                      &cache_block,
				      &cache_block_obl,
                                      &l1_result,
				      &obl_result);
  EXPECT_EQ(status, OK);
}
