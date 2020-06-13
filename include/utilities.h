/*
 *  Cache simulation project
 *  Class UCR IE-521
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
#include "L1cache.h"

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

#endif
