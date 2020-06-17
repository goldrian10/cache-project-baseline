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
#include <Victimcache.h>

#define KB 1024
#define ADDRSIZE 32
using namespace std;

int lru_replacement_policy_l1_vc(const entry_info *l1_vc_info,
    	                      	 bool loadstore,
       	                    	 entry* l1_cache_blocks,
       	                  	 entry* vc_cache_blocks,
        	                 operation_result* l1_result,
              	              	 operation_result* vc_result,
                	         bool debug)
{
	
	
	/*se revisa en serie o paralelo con el L1
	 * es exclusivo con L1
	 * miss de l1 con hit en vc, los datos son intercambiados vc->L1 & L1-> vc
	 * en un mis de L1 y de Vc el dato desplazado del vc se puede descartar
	 * politica FIFO
	 * 16 entradas
	 * completamente asociativo
	*/
	bool hitl1 = false, hitvc=false;
	
	
	for(int cache_pos=0;cache_pos<l1_vc_info->l1_assoc; cache_pos++){
		if(l1_cache_blocks[cache_pos].valid ==1){	
			if(l1_cache_blocks[cache_pos].tag == l1_vc_info->l1_tag){
				hitl1 = true;
				
				if(loadstore==1){
				l1_result->miss_hit = HIT_STORE;
					l1_cache_blocks[cache_pos].dirty=true;
				}
				else{
					l1_result->miss_hit = HIT_LOAD;
					l1_cache_blocks[cache_pos].dirty=false;
				}
				
				//politica LRU de L1
				
				for(int i=0; i< l1_vc_info->l1_assoc; i++){
					if(l1_cache_blocks[i].rp_value > l1_cache_blocks[cache_pos].rp_value) l1_cache_blocks[i].rp_value --;
				}
				l1_cache_blocks[cache_pos].rp_value=l1_vc_info ->l1_assoc -1;
				return OK;
				//si hay hit en L1 el victim no hace nada
				break;
			}
		}
	}
	
	//miss
	if(hitl1 ==false){
		int lru = 0, vc=0;
		
		//set LRU
		for(int i=0; i< l1_vc_info->l1_assoc; i++){
			if(l1_cache_blocks[i].rp_value < l1_cache_blocks[lru].rp_value){
				lru = i;
			} 
		}
		
		//si se va a remplazar el bloque cambio el evicted
		if(l1_cache_blocks[lru].valid ==1){
			l1_result->evicted_address = l1_cache_blocks[lru].tag;
			l1_result->dirty_eviction = l1_cache_blocks[lru].dirty;
		}
		
		l1_cache_blocks[lru].valid = 1;
		
		//hit vc
		for(int vc_pos = 0; vc_pos <l1_vc_info->vc_assoc;vc_pos++){
			if(vc_cache_blocks[vc_pos].valid ==1){	
				if(vc_cache_blocks[vc_pos].tag == l1_vc_info->l1_tag){
					//hit en vc y miss en L1 se cambia el valor de vc a l1 y viceversa
					hitvc=true;
					l1_cache_blocks[lru].tag = vc_cache_blocks[vc_pos].tag;
					l1_cache_blocks[lru].valid = vc_cache_blocks[vc_pos].valid;
					l1_cache_blocks[lru].dirty = vc_cache_blocks[vc_pos].dirty;
					
					vc_cache_blocks[0].tag = l1_result -> evicted_address;
					vc_cache_blocks[0].dirty = l1_result -> dirty_eviction;
					vc_cache_blocks[0].valid = 1;
					if(loadstore==1)vc_result->miss_hit = HIT_STORE;
					else{vc_result->miss_hit = HIT_LOAD;}	
					break;	
				}
			}
		}
		
		//miss vc
		if(hitvc==false){
			//se guarda el evicted de vc del bloque que sale si está lleno
			if(vc_cache_blocks[l1_vc_info -> vc_assoc -1].valid =1){
				vc_result -> evicted_address = vc_cache_blocks[l1_vc_info -> vc_assoc-1].tag;
				vc_result -> dirty_eviction = vc_cache_blocks[l1_vc_info -> vc_assoc-1].dirty;
			}
			//FIFO remplazo valores hacia atras
			for(int i = l1_vc_info -> vc_assoc-1; i>0; i--){
				vc_cache_blocks[i].tag = vc_cache_blocks[i-1].tag;
				vc_cache_blocks[i].valid = vc_cache_blocks[i-1].valid;
				vc_cache_blocks[i].dirty = vc_cache_blocks[i-1].dirty;
			}
			
			vc_cache_blocks[0].tag = l1_result -> evicted_address;
			vc_cache_blocks[0].dirty = l1_result -> dirty_eviction;
			vc_cache_blocks[0].valid = 1;
			
			if(loadstore==1)vc_result->miss_hit = MISS_STORE;
			else{vc_result->miss_hit = MISS_LOAD;}	
			l1_cache_blocks[lru].tag = l1_vc_info->l1_tag;				
		}//fin vc
		
		if(loadstore==1){
			l1_result->miss_hit = MISS_STORE;
			l1_cache_blocks[lru].dirty = true;
		}
		else{
			l1_result->miss_hit = MISS_LOAD;
		}	
		
		for(int i=0; i< l1_vc_info->l1_assoc; i++){
			if(l1_cache_blocks[i].rp_value > l1_cache_blocks[lru].rp_value) {
				l1_cache_blocks[i].rp_value --;
			}
		}
		l1_cache_blocks[lru].rp_value=l1_vc_info ->l1_assoc -1;
		
		
		return OK;
	}
	 
	return ERROR;
}
