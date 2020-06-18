/*
 *  Cache simulation project
 *  Class UCR IE-521
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <math.h>
#include <debug_utilities.h>
#include <L1cache.h>
#include <L2cache.h>
#include <utilities.h>
#include <sstream>
#include <bitset>
#include <cstdio>


#include <array>
#include <cstdlib>
#include <time.h>

#define KB 1024
#define ADDRSIZE 32
using namespace std;


	/*L2 cache multi nivel
	 * L2 debe ser 4 veces el tamaño de L1
	 * asociatividad el doble de la de L1
	 * politica de remplazo L1 y L2 -> LRU
	 * inclusivos
	 * entre L1 y L2 write-trough 
	 * entre L2 y memoria writeback
	 */
	

int l1_l2_entry_info_get (const struct cache_params l1_params, 
						const struct cache_params l2_params, 
						long address, entry_info* l1_l2_info, 
						bool debug){
						/*const struct cache_params l1_params,
						const struct cache_params l2_params,
						long address,
						entry_info* l1_l2_info,
						bool debug=false);*/

	
	
	
	int sizeTag, sizeIdx, sizeOff,tag,idx;
	
	sizeOff=log2(l1_params.block_size);
	sizeIdx=log2((l1_params.size*KB)/l1_params.block_size/l1_params.asociativity);
	sizeTag=ADDRSIZE-sizeOff-sizeIdx;
	
	bitset<32> b(address);
	string pcstr = b.to_string();
	string tagstr,idxstr;
    tagstr=pcstr.substr(4, sizeTag);
    idxstr=pcstr.substr((sizeTag), sizeIdx);
    tag=static_cast<int>(bitset<32>(tagstr).to_ulong());
    idx=static_cast<int>(bitset<32>(idxstr).to_ulong());
    l1_l2_info -> l1_assoc = l1_params.asociativity;
    l1_l2_info -> l1_tag = tag;
	l1_l2_info -> l1_idx = idx;
	
	sizeOff=log2(l2_params.block_size);
	sizeIdx=log2((l2_params.size*KB)/l2_params.block_size/l2_params.asociativity);
	sizeTag=ADDRSIZE-sizeOff-sizeIdx;
	
	bitset<32> p(address);
	pcstr = p.to_string();
	tagstr,idxstr;
    tagstr=pcstr.substr(4, sizeTag);
    idxstr=pcstr.substr((sizeTag), sizeIdx);
    tag=static_cast<int>(bitset<32>(tagstr).to_ulong());
    idx=static_cast<int>(bitset<32>(idxstr).to_ulong());
    l1_l2_info -> l2_assoc = l2_params.asociativity;
    l1_l2_info -> l2_tag = tag;
	l1_l2_info -> l2_idx = idx;
	
	return OK;
}

int lru_replacement_policy_l1_l2(const entry_info *l1_l2_info,
				 bool loadstore,
				 entry* l1_cache_blocks,
				 entry* l2_cache_blocks,
				 operation_result* l1_result,
				 operation_result* l2_result,
				 bool debug) 
{	
	int l2_pos=0;
	bool hitl1 = false, missl2 = false;
	for(int l1_pos = 0; l1_pos < l1_l2_info -> l1_assoc; l1_pos++){
		if(l1_cache_blocks[l1_pos].valid == 1){
			if(l1_cache_blocks[l1_pos].tag == l1_l2_info -> l1_tag){
				hitl1 = true;
				//busco la posicion de el hit en l2
				for(int i = 0; i < l1_l2_info -> l2_assoc; i++){
					if(l2_cache_blocks[i].tag == l1_l2_info -> l2_tag){
						l2_pos=i;
						break;
					}
				}
				if(loadstore == 1){
					l1_result -> miss_hit = HIT_STORE;
					l2_result -> miss_hit = HIT_STORE;
					//nos aseguramos que el dirty bit esté en 1 si es store
					l2_cache_blocks[l2_pos].dirty = true;
				}
				else{
					l1_result -> miss_hit = HIT_LOAD;
					l2_result -> miss_hit = HIT_LOAD;
				}
				
				//LRU - l1
				for(int i = 0; i < l1_l2_info -> l1_assoc; i++){
					if(l1_cache_blocks[i].rp_value > l1_cache_blocks[l1_pos].rp_value){
						l1_cache_blocks[i].rp_value--;
					}
				}
				l1_cache_blocks[l1_pos].rp_value = l1_l2_info -> l1_assoc-1;
				
				//LRU - l2
				for(int i = 0; i < l1_l2_info -> l2_assoc; i++){
					if(l2_cache_blocks[i].rp_value > l2_cache_blocks[l2_pos].rp_value){
						l2_cache_blocks[i].rp_value--;
					}
				}
				l2_cache_blocks[l2_pos].rp_value = l1_l2_info -> l2_assoc-1;
													
				return OK;
				
				
			}
		}
	}
	//MISS l1
	if(hitl1==false){
		int lruL1=0,lruL2=0;
		//si tengo un miss en l1 entonces busco en l2
		for(int l2_pos = 0; l2_pos < l1_l2_info -> l2_assoc; l2_pos++){
			if(l2_cache_blocks[l2_pos].valid==1){
				if(l2_cache_blocks[l2_pos].tag == l1_l2_info -> l2_tag){
					//hit l2
					missl2=true;
					
					if(loadstore == 1){
						l2_cache_blocks[l2_pos].dirty = true;
						l2_result -> miss_hit = HIT_STORE;
					}else{
						l2_result -> miss_hit = HIT_LOAD;
					}
					
					//envio el dato a l1 
					//se busca el lru en l1
					for(int i=0; i<l1_l2_info -> l1_assoc;i++){
						if(l1_cache_blocks[i].rp_value < l1_cache_blocks[lruL1].rp_value){
							lruL1=i;
						}
					}
					
					//falta eviction!!!!!!
					
					l1_cache_blocks[lruL1].tag = l1_l2_info -> l1_tag;//l1_tag????
					l1_cache_blocks[lruL1].valid = 1;
					
					if(loadstore == 1){
						l1_cache_blocks[lruL1].dirty = true;
						l1_result -> miss_hit = MISS_STORE;
					}else{
						l1_result -> miss_hit = MISS_LOAD;
					}
					
					//LRU - l1
					for(int i = 0; i < l1_l2_info -> l1_assoc; i++){
						if(l1_cache_blocks[i].rp_value > l1_cache_blocks[lruL1].rp_value){
							l1_cache_blocks[i].rp_value--;
						}
					}
					l1_cache_blocks[lruL1].rp_value = l1_l2_info -> l1_assoc-1;
					
					//LRU - l2
					for(int i = 0; i < l1_l2_info -> l2_assoc; i++){
						if(l2_cache_blocks[i].rp_value > l2_cache_blocks[l2_pos].rp_value){
							l2_cache_blocks[i].rp_value--;
						}
					}
					l2_cache_blocks[l2_pos].rp_value = l1_l2_info -> l2_assoc-1;
					
					
					return OK;
					
				}
			}
		}
		
		//L2 miss
		if(missl2==false){
			lruL1=0;
			lruL2=0;
			//se busca el lru en l2
			for(int i=0; i<l1_l2_info -> l2_assoc;i++){
				if(l2_cache_blocks[i].rp_value < l1_cache_blocks[lruL2].rp_value){
					lruL2=i;
				}
			}
			
			//EVICTION!!!!!!
			
			
			l1_cache_blocks[lruL2].tag = l1_l2_info -> l2_tag;
			l1_cache_blocks[lruL2].valid = 1;
					
			if(loadstore == 1){
				l2_cache_blocks[lruL2].dirty = true;
				l2_result -> miss_hit = MISS_STORE;
			}else{
				l2_result -> miss_hit = MISS_LOAD;
			}
			//envio el dato a l1 
			//se busca el lru en l1
			for(int i=0; i<l1_l2_info -> l1_assoc;i++){
				if(l1_cache_blocks[i].rp_value < l1_cache_blocks[lruL1].rp_value){
					lruL1=i;
				}
			}
			
			//EVICTION !!!!!!
			
			if(loadstore == 1){
				l1_cache_blocks[lruL1].dirty = true;
				l1_result -> miss_hit = MISS_STORE;
			}else{
				l1_result -> miss_hit = MISS_LOAD;
			}
			
			l1_cache_blocks[lruL1].tag = l1_l2_info -> l1_tag;//l1_tag????
			l1_cache_blocks[lruL1].valid = 1;
			
			//LRU - l1
			for(int i = 0; i < l1_l2_info -> l1_assoc; i++){
				if(l1_cache_blocks[i].rp_value > l1_cache_blocks[lruL1].rp_value){
					l1_cache_blocks[i].rp_value--;
				}
			}
			l1_cache_blocks[lruL1].rp_value = l1_l2_info -> l1_assoc-1;
			
			//LRU - l2
			for(int i = 0; i < l1_l2_info -> l2_assoc; i++){
				if(l2_cache_blocks[i].rp_value > l2_cache_blocks[lruL2].rp_value){
					l2_cache_blocks[i].rp_value--;
				}
			}
			l2_cache_blocks[lruL2].rp_value = l1_l2_info -> l2_assoc-1;
			
			
			return OK;
			
		}
		
		
	}
	
	
	 
	 
	return ERROR;
}
