/*
 *  Cache simulation project
 *  Class UCR IE-521
 *  Semester: II-2019
 *
 * Autores:
 *    Julian Morua Vindas B54872
 *    José López Picado
*/

#ifndef UTILITIES
#define UTILITIES


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define BUFFER_SIZE 32
#define VC_SIZE 16
#define ADDRESS_SIZE 32
#define FACT_ASOC_L2 2
#define FACT_C_SIZE_L2 4
#define RAND -1
typedef unsigned int u_int;

enum optimization_types{
  L2,
  VC
};

enum load_store{
  LOAD,
  STORE
};
/* Return Values */
enum returns_types {
  OK,
  PARAM,
  ERROR,
  INVALIDATED,
  NON_INVALIDATED
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

/* Cache tag array fields */
typedef struct{
  bool valid ;
  bool dirty;
  int tag ;
  int index;
  int rp_value ;
  bool obl_tag;
}entry;

void print_address(u_int addr, const char* msj="");

void print_entry(entry block,const char* msj="");
/**
@brief function to print the metadata from a given cache entry.
@param block is the cache entry that needs to be printed.
@param msj is a message to be printed along with the entry for debug purposes.
*/


void print_set(entry* set, int assoc, const char* msj="");
/**
@brief function to print the metada from a given cache set; it prints the metadata from every entry in the set.
@param set is the cache set that needs to be printed.
@param msj is a message to be printed along with the entry for debug purposes.
*/


/* Cache replacement policy results */
typedef struct{
  enum miss_hit_status miss_hit;
  bool dirty_eviction;
  int  evicted_address;
  bool valid_victimization;
}operation_result;

void print_result(operation_result results,const char* msj="");
/**
@brief function to print the operation_result from a memory reference.
@param results is the operation_result struct whose information needs to be printed.
@param msj is a message to be printed along with the entry for debug purposes.
*/

typedef struct{
  u_int t;   //size of the cache in kilobytes.
  u_int l;   //size of the cache block in bytes.
  u_int a;   //associativity.
  u_int opt; //optimization: victim cache or maltilevel cache.
} parameters;

parameters getParams(int argc, char* argv[]);
/*
* @brief Stores the execution parameters in an organized manner. It also converts them to the required data type.
* @param argc is the amount of parameters recieved at the start of the execution.
* @param argv is a string array containing all the parameters.
* @return A parameter struct containing all of the simulation parameters. If a parameter is not specified at execution time it is assigned a 0 by default.
*/

void printParams(parameters params);
/*
* @brief Prints in the terminal all the execution parameters in an organized table.
* @param params is a parameter struct with the simulation parameters.
*/

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

sizes get_sizes(parameters params);
/**
@brief Obtains the sizes for all of the fields in which a memory address is decomposed to characterize a cache block.
@param params is a parameter struct with the simulation parameters.
@return A sizes struct that contains all of the bit field sizes.
*/

void print_sizes(sizes sizes, int opt);
/**
@brief a function to print all of the bit field sizes from a given sizes struct
@param sizes is the sizes struct with al the information that needs to be printed.
@param opt is the optimization being used to filter out the unwanted data.
*/

typedef struct {
  u_int  address;    //
  u_int loadstore;  //
  u_int ic;         //
}line_info;

line_info get_line_info(char* line);
/**
@brief This function reads a line from a trace file and stores its information in the provided line_info structure.
@param line is the line extracted from the trace file
@return a line_info structure that contans the loadstore bit, the address and the instruction count.
*/

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

entry_info get_entry_info(long address, sizes sizes);
/**
@brief this functions decomposes a given address into the bit fields required to characterize a cache block.
@param address is the memory reference
@return an entry_info struct
*/

void print_entry_info(entry_info info, int opt);
/**
@brief prints the information from an entry_info struct
@param info is the entry_info struct
@param opt is the optimization being used to filter out the unwanted data.
*/

typedef struct statistics{
  u_int references;
  u_int L1_misses;
  u_int L1_hits;
  u_int L2_misses;
  u_int L2_hits;
  u_int global_misses;
  u_int VC_misses;
  u_int VC_hits;
  u_int dirty_evictions;
} statistics;

void printStats(statistics stats, double exec_time, int opt);

void updateStats(statistics* stats, operation_result* l1_results, operation_result* vc_l2_results, u_int opt);;

#endif
