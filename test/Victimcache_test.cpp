/*
 *  Cache simulation project
 *  Class UCR IE-521
 *  Semester: II-2019
 */

#include <gtest/gtest.h>
#include <time.h>
#include <stdlib.h>
#include "debug_utilities.h"
#include "utilities.h"
#include "L1cache.h"
#include "Victimcache.h"


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
 * 1. Se genera una configuración aleatoria de cache
 * 2. Se calculan los sizes
 * 3. Se genera un acceso aleatorio A
 * 4. Se inserta A en el sistema de caches
 * 5. Se victimiza A de L1
 * 6. Se genera un miss en L1 y un hit en VC al accesar A
  */
TEST_F(VCcache,l1_miss_vc_hit){
	DEBUG(debug_on, l1_miss_vc_hit);
 	int status = OK;
 	operation_result l1_result = {};
 	operation_result vc_result = {};

 	// Se genera una configuración aleatoria de cache
 	parameters params = random_params();
	params.opt = VC;
 	// Se calculan los sizes
 	sizes sizes = get_sizes(params);

 	// Se genera un acceso aleatorio
 	line_info access = random_access();
	bool loadstore = rand()%2;

	// Print test params:
	if(debug_on){
		printParams(params);
		print_sizes(sizes, VC);
		printf("** The original memory access for block A: 0x%X **\n", access.address);
	}

 	// Se genera un entry_info a partir de access
 	entry_info A = get_entry_info(access.address,sizes);

 	// Se crean L1 y VC
 	entry l1_cache_blocks[sizes.l1_assoc] = {};
 	entry vc_cache_blocks[sizes.vc_assoc] = {};

	// Print state
	if(debug_on){
		printf("\n** Empty L1 and VC **\n");
		print_set(l1_cache_blocks,sizes.l1_assoc,"L1 |");
		print_set(vc_cache_blocks, sizes.vc_assoc,"VC |");
	}

 	// Se inserta A
 	status = lru_replacement_policy_l1_vc(&A,loadstore,l1_cache_blocks,vc_cache_blocks,&l1_result,&vc_result,debug_on);
 	EXPECT_EQ(status,OK);

	// Print state
	if(debug_on){
		printf("\n** Block A has been inserted in L1 **\n");
		printf("Tag of A in L1: 0x%X\n",A.l1_tag);
		print_set(l1_cache_blocks,sizes.l1_assoc,"L1 |");
		print_set(vc_cache_blocks, sizes.vc_assoc,"VC |");
	}

 	// Se llena L1 hasta victimizar A, lo que deberia insertarlo en VC.
 	for (size_t i = 0; i < sizes.l1_assoc; i++){
		line_info  access = random_access();
		entry_info B = get_entry_info(access.address,sizes);
		while(A.l1_tag == B.l1_tag || A.l1_idx != B.l1_idx || is_in_set(l1_cache_blocks,B.l1_assoc,B.l1_tag)){
			access = random_access();
			B = get_entry_info(access.address,sizes);
		}
		status = lru_replacement_policy_l1_vc(&B,loadstore,l1_cache_blocks,vc_cache_blocks,&l1_result,&vc_result,false);
		EXPECT_EQ(status,OK);
 	}

	// Print state
	if(debug_on){
		printf("\n** Block A has been victimized from L1 and inserted in VC **\n");
		printf("Tag of A in L1: 0x%X\n",A.l1_tag);
		print_set(l1_cache_blocks,sizes.l1_assoc,"L1 |");
		print_set(vc_cache_blocks, sizes.vc_assoc,"VC |");
	}

 	// Se referencia a A, lo que deberia provocar miss en L1 pero hit en VC.
 	status = lru_replacement_policy_l1_vc(&A,loadstore,l1_cache_blocks,vc_cache_blocks,&l1_result,&vc_result,debug_on);
 	EXPECT_EQ(status, OK);

	// Comprobación de miss en L1
	int expected_l1 = (loadstore) ? MISS_STORE : MISS_LOAD;
 	// Comprobación de miss en l1
 	EXPECT_EQ(l1_result.miss_hit,expected_l1);

	// Comprobación de hit en VC
	int expected_vc = (loadstore) ? HIT_STORE : HIT_LOAD;
 	EXPECT_EQ(vc_result.miss_hit,expected_vc);
 }

 /*
 * 1. Se genera una configuración aleatoria de cache
 * 2. Se calculan los sizes
 * 3. Se genera un acceso aleatorio A
 * 4. Se inserta A en el sistema de caches
 * 5. Se victimiza A de L1 y VC
 * 6. Se genera un miss en L1 y VC al accesar A
  */
 TEST_F(VCcache,l1_miss_vc_miss){
	 DEBUG(debug_on, l1_miss_vc_miss);
	 int status = OK;
	 operation_result l1_result = {};
	 operation_result vc_result = {};

	 // Se genera una configuración aleatoria de cache
	 parameters params = random_params();
	 params.opt = VC;
	 // Se calculan los sizes
	 sizes sizes = get_sizes(params);

	 // Se genera un acceso aleatorio
	 line_info access = random_access();
	 bool loadstore = rand()%2;

	 // Print test params:
	 if(debug_on){
		 printParams(params);
		 print_sizes(sizes, VC);
		 printf("** The original memory access for block A: 0x%X **\n", access.address);
	 }

 	// Se genera un entry_info a partir de access
 	entry_info A = get_entry_info(access.address,sizes);

 	// Se crean L1 y VC
 	entry l1_cache_blocks[sizes.l1_assoc] = {};
 	entry vc_cache_blocks[sizes.vc_assoc] = {};

	// Print state
	if(debug_on){
		printf("\n** Empty L1 and VC **\n");
		print_set(l1_cache_blocks,sizes.l1_assoc,"L1 |");
		print_set(vc_cache_blocks, sizes.vc_assoc,"VC |");
	}

 	// Se inserta A
 	status = lru_replacement_policy_l1_vc(&A,loadstore,l1_cache_blocks,vc_cache_blocks,&l1_result,&vc_result,debug_on);
 	EXPECT_EQ(status,OK);

	// Print state
	if(debug_on){
		printf("\n** Block A has been inserted in L1 **\n");
		printf("Tag of A in L1: 0x%X\n",A.l1_tag);
		print_set(l1_cache_blocks,sizes.l1_assoc,"L1 |");
		print_set(vc_cache_blocks, sizes.vc_assoc,"VC |");
	}

 	// Se llena L1 hasta victimizar A tanto de L1 como de L2
 	for (size_t i = 0; i < (sizes.l1_assoc + sizes.vc_assoc); i++){
		line_info  access = random_access();
		entry_info B = get_entry_info(access.address,sizes);
		while(A.l1_tag == B.l1_tag || A.l1_idx != B.l1_idx || is_in_set(l1_cache_blocks,B.l1_assoc,B.l1_tag) || is_in_set(vc_cache_blocks,B.vc_assoc,B.l1_tag)){
			access = random_access();
			B = get_entry_info(access.address,sizes);
		}
		status = lru_replacement_policy_l1_vc(&B,loadstore,l1_cache_blocks,vc_cache_blocks,&l1_result,&vc_result,false);
		EXPECT_EQ(status,OK);
 	}

	// Print state
	if(debug_on){
		printf("\n** Block A has been victimized from L1 and VC **\n");
		printf("Tag of A in L1: 0x%X\n",A.l1_tag);
		print_set(l1_cache_blocks,sizes.l1_assoc,"L1 |");
		print_set(vc_cache_blocks, sizes.vc_assoc,"VC |");
	}

 	// Se referencia a A
 	status = lru_replacement_policy_l1_vc(&A,loadstore,l1_cache_blocks,vc_cache_blocks,&l1_result,&vc_result,debug_on);
 	EXPECT_EQ(status, OK);

	// Comprobación de miss en L1
	int expected_l1 = (loadstore) ? MISS_STORE : MISS_LOAD;
 	// Comprobación de miss en l1
 	EXPECT_EQ(l1_result.miss_hit,expected_l1);

	// Comprobación de miss en VC
	int expected_vc = (loadstore) ? MISS_STORE : MISS_LOAD;
 	EXPECT_EQ(vc_result.miss_hit,expected_vc);
 }
