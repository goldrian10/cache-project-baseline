/*
 *  Cache simulation project
 *  Class UCR IE-521
 *  Semester: II-2019
 */

#include <gtest/gtest.h>
#include <time.h>
#include <stdlib.h>
#include <debug_utilities.h>
#include <L1cache.h>
#include <Victimcache.h>

using namespace std;

class VCcache : public ::testing::Test{
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
TEST_F(VCcache,l1_miss_vc_hit){
	int status = OK;
	struct l1_vc_entry_info l1_vc_info;
	struct entry l1_cache_blocks;
	struct entry vc_cache_blocks;
        struct operation_result l1_result = {};
        struct operation_result vc_result = {};
	DEBUG(l1_miss_vc_hit_test);
        status = lru_replacement_policy_l1_vc(&l1_vc_info,
      	                        	      true,
        	                      	      &l1_cache_blocks,
          	                    	      &vc_cache_blocks,
            	                  	      &l1_result,
              	                	      &vc_result);
  EXPECT_EQ(status, OK);
}
