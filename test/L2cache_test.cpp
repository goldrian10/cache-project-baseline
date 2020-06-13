/*
 *  Cache simulation project
 *  Class UCR IE-521
 *  Semester: II-2019
 */

#include <gtest/gtest.h>
#include <time.h>
#include <stdlib.h>
#include "debug_utilities.h"
#include "L1cache.h"
#include "L2cache.h"

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
 * TEST3:
 * 1. Se genera una configuración aleatoria de cache
 * 2. Se calculan los sizes
 * 3. Se genera un acceso aleatorio A
 * 5. Se llena L1 al producir accesos distintos que A
 * 6. Se genera un miss en L1 y L2 al accesar A
 * 7. Se accesa A para producir un hit en L1 y L2
 * 8. Se verifica que hay hit en L1 y L2
 */
TEST_F(L2cache,l1_hit_l2_hit){
	DEBUG(debug_on, l1_hit_l2_hit);
	int status = OK;
	operation_result l1_result = {};
	operation_result l2_result = {};

	// Se genera una configuración aleatoria de cache
	parameters params = random_params();
	params.opt = L2;
	// Se calculan los sizes
	sizes sizes = get_sizes(params);

	// Se genera un acceso aleatorio
	line_info access = random_access();
	bool loadstore = rand()%2;

	// Print test params:
	if(debug_on){
		printParams(params);
		print_sizes(sizes, L2);
		printf("** The original memory access for block A: 0x%X **\n", access.address);
	}

	// Se genera un entry_info a partir de la direccion de acceso
	entry_info A = get_entry_info(access.address,sizes);

	// Se crean L1 y L2
	entry l1_cache_blocks[sizes.l1_assoc] = {};
	entry l2_cache_blocks[sizes.l2_assoc] = {};

	// Se llenan L1 y L2
	for (size_t i = 0; i < sizes.l2_assoc; i++){
		line_info  access = random_access();
		entry_info B = get_entry_info(access.address,sizes);
		//se deben forzar tags distintos, mismo indice (misma linea) y que no hayan repetidos
		while(A.l1_tag == B.l1_tag || A.l1_idx != B.l1_idx || is_in_set(l1_cache_blocks,B.l1_assoc,B.l1_tag)){
			access = random_access();
			B = get_entry_info(access.address,sizes);
		}
		status = lru_replacement_policy_l1_l2(&B,loadstore,l1_cache_blocks,l2_cache_blocks,&l1_result,&l2_result,false);
		EXPECT_EQ(status,OK);
	}

	// Print state
	if(debug_on){
		printf("\n** Before insertion of block A **\n");
		printf("Tag of A in L1: 0x%X\n",A.l1_tag);
		printf("Tag of A in L2: 0x%X\n",A.l2_tag);
		print_set(l1_cache_blocks,sizes.l1_assoc,"L1 |");
		print_set(l2_cache_blocks, sizes.l2_assoc,"L2 |");
	}


	// Se inserta A
	status = lru_replacement_policy_l1_l2(&A,loadstore,l1_cache_blocks,l2_cache_blocks,&l1_result,&l2_result,debug_on);
	EXPECT_EQ(status,OK);

	// Print state
	if(debug_on){
		printf("\n** After insertion of block A, before forced hit **\n");
		printf("Tag of A in L1: 0x%X\n",A.l1_tag);
		printf("Tag of A in L2: 0x%X\n",A.l2_tag);
		print_set(l1_cache_blocks,sizes.l1_assoc,"L1 |");
		print_set(l2_cache_blocks, sizes.l2_assoc,"L2 |");
	}

	// Se referencia a A
	status = lru_replacement_policy_l1_l2(&A,loadstore,l1_cache_blocks,l2_cache_blocks,&l1_result,&l2_result,debug_on);
	EXPECT_EQ(status, OK);

	// Print state
	if(debug_on){
		printf("\n** After forced hit **\n");
		printf("Tag of A in L1: 0x%X\n",A.l1_tag);
		printf("Tag of A in L2: 0x%X\n",A.l2_tag);
		print_set(l1_cache_blocks,sizes.l1_assoc,"L1 |");
		print_set(l2_cache_blocks, sizes.l2_assoc,"L2 |");
	}

	int expected = (loadstore) ? HIT_STORE : HIT_LOAD;
	// Comprobación de hit en L1
	EXPECT_EQ(l1_result.miss_hit,expected);
	// Comprobación de hit en L2
	EXPECT_EQ(l2_result.miss_hit,expected);

	//Comprobacion de MRU iguales
	u_int l1_MRU_tag;
	u_int l2_MRU_tag;
	int MRU_value = 0;
	for (int i = 0; i < sizes.l1_assoc; i++) {
		if(l1_cache_blocks[i].rp_value == MRU_value){
			l1_MRU_tag = l1_cache_blocks[i].tag;
		}
	}
	for (int i = 0; i < sizes.l2_assoc; i++) {
		if(l2_cache_blocks[i].rp_value == MRU_value){
			l2_MRU_tag = l2_cache_blocks[i].tag;
		}
	}
	if(debug_on){
		printf("\n** MRU tags **\n");
		printf("L1 MRU tag = 0x%X\n", l1_MRU_tag);
		printf("L2 MRU tag = 0x%X\n", l2_MRU_tag);
	}
	EXPECT_EQ(l1_MRU_tag, A.l1_tag);
	EXPECT_EQ(l2_MRU_tag, A.l2_tag);
}



/*
 * TEST4:
 * 1. Se genera una configuración aleatoria de cache
 * 2. Se calculan los sizes
 * 3. Se genera un acceso aleatorio A
 * 4. Se inserta A en el sistema de caches
 * 5. Se victimiza A de L1 al producir accesos distintos que A.
 * 6. Se genera un miss en L1 y hit en L2 al accesar A.
 */
TEST_F(L2cache,l1_miss_l2_hit){
	DEBUG(debug_on, l1_miss_l2_hit);
	int status = OK;
	operation_result l1_result = {};
	operation_result l2_result = {};

	// Se genera una configuración aleatoria de cache
	parameters params = random_params();
	params.opt = L2;

	// Se calculan los sizes
	sizes sizes = get_sizes(params);

	// Se genera un acceso aleatorio
	line_info access = random_access();
	bool loadstore = rand()%2;

	// Print test params:
	if(debug_on){
		printParams(params);
		print_sizes(sizes, L2);
		printf("** The original memory access for block A: 0x%X **\n", access.address);
	}

	// Se genera un entry_info a partir de la direccion de acceso
	entry_info A = get_entry_info(access.address,sizes);

	// Se crean L1 y L2
	entry l1_cache_blocks[sizes.l1_assoc] = {};
	entry l2_cache_blocks[sizes.l2_assoc] = {};

	// Print state
	if(debug_on){
		printf("\n** Empty L1 and L2 **\n");
		print_set(l1_cache_blocks,sizes.l1_assoc,"L1 |");
		print_set(l2_cache_blocks, sizes.l2_assoc,"L2 |");
	}

	// Se inserta A en L1 y L2
	status = lru_replacement_policy_l1_l2(&A,loadstore,l1_cache_blocks,l2_cache_blocks,&l1_result,&l2_result,false);
	EXPECT_EQ(status,OK);

	// Print state
	if(debug_on){
		printf("\n** Block A has been inserted **\n");
		printf("Tag of A in L1: 0x%X\n",A.l1_tag);
		printf("Tag of A in L2: 0x%X\n",A.l2_tag);
		print_set(l1_cache_blocks,sizes.l1_assoc,"L1 |");
		print_set(l2_cache_blocks, sizes.l2_assoc,"L2 |");
	}

	// Se hacen accsesos hasta victimizar A de L1 (pero A sigue estando en L2)
	for (size_t i = 0; i < sizes.l1_assoc; i++){
		line_info  access = random_access();
		entry_info B = get_entry_info(access.address,sizes);
		while(A.l1_tag == B.l1_tag || A.l1_idx != B.l1_idx || is_in_set(l1_cache_blocks,B.l1_assoc,B.l1_tag)){
			access = random_access();
			B = get_entry_info(access.address,sizes);
		}
		status = lru_replacement_policy_l1_l2(&B,loadstore,l1_cache_blocks,l2_cache_blocks,&l1_result,&l2_result,false);
		EXPECT_EQ(status,OK);
	}

	// Print state
	if(debug_on){
		printf("\n** Block A has been victimized from L1 but not from L2 **\n");
		printf("Tag of A in L1: 0x%X\n",A.l1_tag);
		printf("Tag of A in L2: 0x%X\n",A.l2_tag);
		print_set(l1_cache_blocks,sizes.l1_assoc,"L1 |");
		print_set(l2_cache_blocks, sizes.l2_assoc,"L2 |");
	}

	// Se referencia a A: esto fuerza miss en L1 y Hit en L2.
	status = lru_replacement_policy_l1_l2(&A,loadstore,l1_cache_blocks,l2_cache_blocks,&l1_result,&l2_result,debug_on);
	EXPECT_EQ(status, OK);

	// Comprobación de miss en L1
	int expected_l1 = (loadstore) ? MISS_STORE : MISS_LOAD;
	EXPECT_EQ(l1_result.miss_hit,expected_l1);
	// Comprobación de hit en L2
	int expected_l2 = (loadstore) ? HIT_STORE : HIT_LOAD;
	EXPECT_EQ(l2_result.miss_hit,expected_l2);
}


/*
 * TEST5:
 * 1. Se genera una configuración aleatoria de cache
 * 2. Se calculan los sizes
 * 3. Se genera un acceso aleatorio A
 * 4. Se inserta A en el sistema de caches
 * 5. Se victimiza A de L1 y L2 al producir accesos distintos que A.
 * 6. Se genera un miss en L1 y en L2 al accesar A.
 */
TEST_F(L2cache,l1_miss_l2_miss){
	DEBUG(debug_on, l1_miss_l2_miss);
	int status = OK;
	operation_result l1_result = {};
	operation_result l2_result = {};

	// Se genera una configuración aleatoria de cache
	parameters params = random_params();
	params.opt = L2;

	// Se calculan los sizes
	sizes sizes = get_sizes(params);

	// Se genera un acceso aleatorio
	line_info access = random_access();
	bool loadstore = rand()%2;

	// Print test params:
	if(debug_on){
		printParams(params);
		print_sizes(sizes, L2);
		printf("** The original memory access for block A: 0x%X **\n", access.address);
	}

	// Se genera un entry_info a partir de la direccion de acceso
	entry_info A = get_entry_info(access.address,sizes);

	// Se crean L1 y L2
	entry l1_cache_blocks[sizes.l1_assoc] = {};
	entry l2_cache_blocks[sizes.l2_assoc] = {};

	// Print state
	if(debug_on){
		printf("\n** Empty L1 and L2 **\n");
		print_set(l1_cache_blocks,sizes.l1_assoc,"L1 |");
		print_set(l2_cache_blocks, sizes.l2_assoc,"L2 |");
	}

	// Se inserta A en L1 y L2
	status = lru_replacement_policy_l1_l2(&A,loadstore,l1_cache_blocks,l2_cache_blocks,&l1_result,&l2_result,false);
	EXPECT_EQ(status,OK);

	// Print state
	if(debug_on){
		printf("\n** Block A has been inserted **\n");
		printf("Tag of A in L1: 0x%X\n",A.l1_tag);
		printf("Tag of A in L2: 0x%X\n",A.l2_tag);
		print_set(l1_cache_blocks,sizes.l1_assoc,"L1 |");
		print_set(l2_cache_blocks, sizes.l2_assoc,"L2 |");
	}

	// Se hacen accsesos hasta victimizar A de L1 y de L2
	for (size_t i = 0; i < sizes.l2_assoc; i++){
		line_info  access = random_access();
		entry_info B = get_entry_info(access.address,sizes);
		while(A.l1_tag == B.l1_tag || A.l1_idx != B.l1_idx || is_in_set(l1_cache_blocks,B.l1_assoc,B.l1_tag)){
			access = random_access();
			B = get_entry_info(access.address,sizes);
		}
		status = lru_replacement_policy_l1_l2(&B,loadstore,l1_cache_blocks,l2_cache_blocks,&l1_result,&l2_result,false);
		EXPECT_EQ(status,OK);
	}

	// Print state
	if(debug_on){
		printf("\n** Block A has been victimized from L1 and L2 **\n");
		printf("Tag of A in L1: 0x%X\n",A.l1_tag);
		printf("Tag of A in L2: 0x%X\n",A.l2_tag);
		print_set(l1_cache_blocks,sizes.l1_assoc,"L1 |");
		print_set(l2_cache_blocks, sizes.l2_assoc,"L2 |");
	}

	// Se referencia a A: esto fuerza miss en L1 y miss en L2.
	status = lru_replacement_policy_l1_l2(&A,loadstore,l1_cache_blocks,l2_cache_blocks,&l1_result,&l2_result,debug_on);
	EXPECT_EQ(status, OK);

	// Comprobación de miss en L1
	int expected_l1 = (loadstore) ? MISS_STORE : MISS_LOAD;
	EXPECT_EQ(l1_result.miss_hit,expected_l1);
	// Comprobación de hit en L2
	int expected_l2 = (loadstore) ? MISS_STORE : MISS_LOAD;
	EXPECT_EQ(l2_result.miss_hit,expected_l2);
}

// Test6: Invalidación de L1 por una victimización en L2
 TEST_F(L2cache,invalid_L1){
 	int status;
 	operation_result l1_result = {};
 	operation_result l2_result = {};

 	// Se genera una configuración aleatoria de cache
 	parameters params = random_params();

 	// Se calculan los sizes
 	sizes sizes = get_sizes(params);

 	// Se genera un acceso aleatorio
 	line_info access = random_access();
	bool loadstore = rand()%2;

	// Print test params:
	if(debug_on){
		printParams(params);
		print_sizes(sizes, L2);
		printf("** The original memory access for block A: 0x%X **\n", access.address);
	}

 	// Se genera un entry_info a partir de access
 	entry_info A = get_entry_info(access.address,sizes);
	 
 	// Se crean L1 y L2
 	entry l1_cache_blocks[sizes.l1_assoc] = {};
 	entry l2_cache_blocks[sizes.l2_assoc] = {};

	// Se inserta A en L1 y L2
	status = lru_replacement_policy_l1_l2(&A,STORE,l1_cache_blocks,l2_cache_blocks,&l1_result,&l2_result,false);
	EXPECT_EQ(status,OK);

	// Print state
	if(debug_on){
		printf("\n** Block A has been inserted **\n");
		printf("Tag of A in L1: 0x%X\n",A.l1_tag);
		printf("Tag of A in L2: 0x%X\n",A.l2_tag);
		print_set(l1_cache_blocks,sizes.l1_assoc,"L1 |");
		print_set(l2_cache_blocks, sizes.l2_assoc,"L2 |");
	}

	// Se hacen accsesos hasta victimizar A de L1 y de L2
	for (size_t i = 0; i < sizes.l2_assoc; i++){
		line_info  access = random_access();
		entry_info B = get_entry_info(access.address,sizes);
		while(A.l1_tag == B.l1_tag || A.l1_idx != B.l1_idx || is_in_set(l1_cache_blocks,B.l1_assoc,B.l1_tag)){
			access = random_access();
			B = get_entry_info(access.address,sizes);
		}
		status = lru_replacement_policy_l1_l2(&B,loadstore,l1_cache_blocks,l2_cache_blocks,&l1_result,&l2_result,false);
		EXPECT_EQ(status,OK);
	}

	// Print state
	if(debug_on){
		printf("\n** Block A has been victimized from L1 and L2 **\n");
		printf("Tag of A in L1: 0x%X\n",A.l1_tag);
		printf("Tag of A in L2: 0x%X\n",A.l2_tag);
		print_set(l1_cache_blocks,sizes.l1_assoc,"L1 |");
		print_set(l2_cache_blocks, sizes.l2_assoc,"L2 |");
	}

	// Se añade A de manera forzada
	l1_cache_blocks[0].tag = A.l1_tag;

	// Print state
	if(debug_on){
		printf("\n** Block A has been forced into L1 **\n");
		printf("Tag of A in L1: 0x%X\n",A.l1_tag);
		printf("Tag of A in L2: 0x%X\n",A.l2_tag);
		print_set(l1_cache_blocks,sizes.l1_assoc,"L1 |");
		print_set(l2_cache_blocks, sizes.l2_assoc,"L2 |");
		printf("\n");
	}

	status = l1_line_invalid_set(A.l1_tag, A.l1_assoc, l1_cache_blocks,debug_on);
	EXPECT_EQ(status, INVALIDATED);
 }