/*
 * 	Cache simulation project
 * 	Class UCR IE-521
 * 	Semester: I-2019
*/
#ifndef DEBUG_UTILITIES
#define DEBUG_UTILITIES
#include "L1cache.h"

/* MACROS */
#define DEBUG(x) if (debug_on) printf("%s\n",#x)
#define YEL   "\x1B[33m"


#define RAND -1
typedef struct{
  u_int original_address;
  // L1
	u_int l1_tag;
	u_int l1_idx;
	u_int l1_offset;
	u_int l1_assoc;
  // L2
	u_int l2_tag;
	u_int l2_idx;
	u_int l2_offset;
	u_int l2_assoc;
  // VC
  u_int vc_assoc;
}entry_info;


typedef struct{
  u_int l1_tag_bits;    //
  u_int l1_offset_bits; //
  u_int l1_index_bits;  //
  u_int l2_tag_bits;    //
  u_int l2_offset_bits; //
  u_int l2_index_bits;  //
  u_int vc_assoc;       //
  u_int l1_assoc;
  u_int l2_assoc;
}sizes;


typedef struct{
  u_int t;   //size of the cache in kilobytes.
  u_int l;   //size of the cache block in bytes.
  u_int a;   //associativity.
  u_int opt; //optimization: victim cache or maltilevel cache.
} parameters;

typedef struct {
  u_int  address;    //
  u_int loadstore;  //
  u_int ic;         //
}line_info;


/* FUNCTIONS */

/* Get enviroment var */
void get_env_var( const char* var_name,
                 int *var_value );

/* Print way info */
void print_way_info(int idx,
                    int associativity,
                    entry* cache_blocks); 
                    

bool is_in_set(entry* set, int assoc, int tag);

parameters random_params();

line_info random_access(int loadstore=-1);

entry_info generate_entry_info(sizes sizes);

#endif

