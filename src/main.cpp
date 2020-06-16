#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <math.h>
#include <L1cache.h>
#include <debug_utilities.h>
#include <array>
#include <cstdlib>
#include <time.h>
#include <sstream>
#include <bitset>
#include <cstdio>
using namespace std;

/* Helper funtions */
void print_usage ()
{
  printf ("Print print_usage\n");
  exit (0);
}

void resultados(cache_params params, int miss_load, int miss_store, int hit_load, int hit_store, int dirties){
   cout << "_______________________________________________________" << " \n " << "Cache Parameters \n" << "_______________________________________________________ \n";
   cout << " Cache Size (KB):    " << params.size << "\n";
   cout << " Cache Associativity:    " << params.asociativity << "\n";
   cout << " Cache Block Size (bytes):    " << params.block_size << "\n";
   cout << "_______________________________________________________" << " \n " << "Simulation results \n" << "_______________________________________________________ \n";
   cout << " CPU time (cycles):    \n";
   cout << " AMAT (cycles):    \n";
   cout << " Overall miss rate:    " << (miss_load + miss_store)*100/(miss_load+hit_load + miss_store + hit_store) << "% \n";
   cout << " Read miss rate:       " << miss_load*100/(miss_load+hit_load) << "%\n";
   cout << " Dirty evictions:      " << dirties << "\n";
   cout << " Load misses:          " << miss_load <<"\n";
   cout << " Store misses:         " << miss_store << "\n";
   cout << " Total misses:         " << miss_load+miss_store << "\n";
   cout << " Load hits:            " << hit_load << "\n";
   cout << " Store hits:           " << hit_store << "\n";
   cout << " Total hits:           " << hit_load + hit_store << "\n";
   cout << "_______________________________________________________" << " \n ";
}


int main(int argc, char * argv []) {
 
  int size=0, block_size=0,asociativity=0,tag=0,idx=0,ls;
  int miss_load=0,  miss_store=0,  hit_load=0,  hit_store=0,  dirty =0;
  uint8_t rp_value=0;
  string pc,trace1, offset,trace;
  
  
  struct cache_params cache_params;
  struct cache_field_size field_size;
  struct operation_result result;
  
  enum replacement_policy RP;

  sscanf(argv[2], "%d",&size);
  sscanf(argv[4], "%d",&block_size);
  sscanf(argv[6],"%d",&asociativity);
  sscanf(argv[8],"%c",&rp_value);
  
  if(rp_value=='0') RP=LRU;
  if(rp_value=='1') RP=NRU;
  if(rp_value=='2') RP=RRIP;
  if(rp_value=='3') RP=RANDOM;
  
  cache_params.size=size;
  cache_params.block_size=block_size;
  cache_params.asociativity=asociativity;
  
  field_size_get(cache_params, &field_size);	
  int sets=pow(2,field_size.idx);
  struct entry cache_blocks[sets][cache_params.asociativity];

    for(int i = 0; i<(int)pow(2,field_size.idx); i++){
		for(int j =0; j < cache_params.asociativity; j++){
			if(RP==LRU){
				cache_blocks[i][j].rp_value = 0;
			}
			if(RP==NRU){
				cache_blocks[i][j].rp_value=1;
				}
			cache_blocks[i][j].valid = 0;
			cache_blocks[i][j].tag = 0;
			cache_blocks[i][j].dirty = 0;
		 }
	  }
  while(getline(cin,trace1)){
	  
	pc=trace1.substr(4, 8);
	stringstream ss;
	ss << hex << pc;
	unsigned n;
	ss >> n;
	bitset<32> b(n);
	long bulong=b.to_ulong();
    //cout <<"\npc number:"<< pc<< endl;
	//cout<<"pc bin:"<<b<<endl;
	address_tag_idx_get(bulong,field_size, &idx,&tag);
	string loadstore = trace1.substr(2,1);
	ls=stoi(loadstore);
    //printf("tag len:%d\nindex len:%d\noffset len:%d\n",field_size.tag,field_size.idx,field_size.offset);
    //cout <<"\ntag:"<<tag<<endl;
    //cout <<"offset:"<<offset<<endl;
    //cout <<"idx:"<<idx<<endl;
    
   
    switch(RP){
		case LRU:
			lru_replacement_policy (idx, tag, cache_params.asociativity, ls, cache_blocks[idx], &result, false);
		break;
		case NRU:
			nru_replacement_policy (idx, tag, cache_params.asociativity, ls, cache_blocks[idx], &result, false);
		break;
		case RRIP:
			srrip_replacement_policy (idx, tag, cache_params.asociativity, ls, cache_blocks[idx], &result, false);
		break;
		default:
		cout<<"ERROR RP"<<endl;
	 }
	 
		if(result.miss_hit == MISS_LOAD) miss_load++;
		if(result.miss_hit == MISS_STORE) miss_store++;
		if(result.miss_hit == HIT_LOAD) hit_load++;
		if(result.miss_hit == HIT_STORE) hit_store++;
		if(result.dirty_eviction == 1) dirty++; 
    
	}
	resultados(cache_params, miss_load, miss_store, hit_load, hit_store, dirty);
	
	


	
return 0;
}
