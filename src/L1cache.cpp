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
#include <array>
#include <cstdlib>
#include <time.h>
#include <sstream>
#include <bitset>
#include <cstdio>

#define KB 1024
#define ADDRSIZE 32
using namespace std;

int field_size_get(struct cache_params cache_params,
                   struct cache_field_size *field_size)
{
  //offset size log2(block_size)
  field_size->offset=log2(cache_params.block_size);
  //idx size = log2(sets/a)
  field_size->idx=log2((cache_params.size*KB)/cache_params.block_size/cache_params.asociativity);
  //tag = ADDRSIZE - offset - idx
  field_size->tag=ADDRSIZE-field_size->offset-field_size->idx;
  return OK;
}

void address_tag_idx_get(long address,
                         struct cache_field_size field_size,
                         int *idx,
                         int *tag)
{
	bitset<32> b(address);
	string pcstr = b.to_string();
	string tagstr,idxstr;
    tagstr=pcstr.substr(0, field_size.tag);
    idxstr=pcstr.substr((field_size.tag), field_size.idx);
    *tag=static_cast<int>(bitset<32>(tagstr).to_ulong());
    *idx=static_cast<int>(bitset<32>(idxstr).to_ulong());

}

int srrip_replacement_policy (int idx,
                             int tag,
                             int associativity,
                             bool loadstore,
                             entry* cache_blocks,
                             operation_result* result,
                             bool debug)
{
	//se inicializa el resultado en miss
	
	bool state=false;
	
	for(int cache_pos = 0; cache_pos < associativity; cache_pos++){
		if(cache_blocks[cache_pos].valid==1){
			//si el bit de valido esta en 1 se revisa este bloque, si no se sigue
			if(cache_blocks[cache_pos].tag==tag){
				state = true;
				//se revisa si hay un hit y luego revisa si la instruccion es store
				if(loadstore==1){
					cache_blocks[cache_pos].dirty = true;
					result -> miss_hit = HIT_STORE;
				}
				else{
					result -> miss_hit = HIT_LOAD;
				}
				
				//se se guarda el valor de 0 en el rrp bit				
				cache_blocks[cache_pos].rp_value=0;
				return OK;
			}
		}
		
	}
	
	//miss
	
	
	if(state==false){
			int rrp=0;
			bool reset=true;
			int M=2;
			if(associativity <=2)M=1;
			int entrada = pow(2,M)-2;
			int distante = pow(2,M)-1;
			//busca el primer (2**M-1) en el rrp bit de izquierda a derecha y guarda la posicion en rrp 
			//reset es true si no hay ningun (2**M-1) en los rrp bits 
			while(reset==true){	
				for(int i=0; i<associativity;i++){
					if(cache_blocks[i].rp_value == distante){
						rrp=i;
						reset=false;
						break;
					}
				}
					//si no hay ningun distante entonces se suma 1 a todos los bloques
				if(reset==true){
					for(int i=0; i<associativity;i++){
						cache_blocks[i].rp_value ++;
					}
				}
			}
			
			cache_blocks[rrp].rp_value=entrada;
			
			
			//si se desaloja,se guardan los valores del bloque en los eviction
			if(cache_blocks[rrp].valid){
				result->dirty_eviction = cache_blocks[rrp].dirty;
				result->evicted_address = cache_blocks[rrp].tag;
			}
			
			cache_blocks[rrp].tag = tag;
			cache_blocks[rrp].valid =1;
			
			//si fue store se  actualiza el dirty bit y result
			if(loadstore==1){
				cache_blocks[rrp].dirty = true;
				result -> miss_hit = MISS_STORE;
			}
			else{
				result -> miss_hit = MISS_LOAD;
			}
			
			
			return OK;
		}
	/*if(state==true){
		cout <<"hit"<<endl;
	}else cout <<"miss"<<endl;
	*/
	
	
   return ERROR;
}


int lru_replacement_policy (int idx,
                             int tag,
                             int associativity,
                             bool loadstore,
                             entry* cache_blocks,
                             operation_result* result,
                             bool debug)
{
		//se inicializa el resultado en miss
		
		bool state=false;
			
		
		for(int cache_pos = 0; cache_pos < associativity; cache_pos++){
			if(cache_blocks[cache_pos].valid==1){
				//si el bit de valido esta en 1 se revisa este bloque, si no se sigue
				if(cache_blocks[cache_pos].tag==tag){
					state = true;
					
					//se revisa si hay un hit y luego revisa si la instruccion es store
					if(loadstore){
						cache_blocks[cache_pos].dirty = true;
						result -> miss_hit = HIT_STORE;
					}
					else{
						result -> miss_hit = HIT_LOAD;
					}
					//se les hace update a los valores del LRU 
					
					for(int i = 0; i < associativity; i++){
						if(cache_blocks[i].rp_value > cache_blocks[cache_pos].rp_value){
							cache_blocks[i].rp_value--;
						}
					}
				cache_blocks[cache_pos].rp_value=associativity-1;
				break;
				
				}
				
			}
		}
		
		//miss
		
		
		if(state==false){
				int lru=0;
				//busca el rp menor para remplazarlo por el nuevo tag
				for(int i=0; i<associativity;i++){
					if(cache_blocks[i].rp_value < cache_blocks[lru].rp_value){
						lru=i;
					}
				}
				
				
				//si se desaloja,se guardan los valores del bloque en los eviction
				if(cache_blocks[lru].valid){
					result->dirty_eviction = cache_blocks[lru].dirty;
					result->evicted_address = cache_blocks[lru].tag;
				}
				
				cache_blocks[lru].tag = tag;
				cache_blocks[lru].valid =1;
				
				//si fue store se  actualiza el dirty bit y result
				if(loadstore==1){
					cache_blocks[lru].dirty = true;
					result -> miss_hit = MISS_STORE;
				}
				else{
					result -> miss_hit = MISS_LOAD;
				}
				
				//se remplazan los valores nuevos de LRU 
				
				for(int i = 0; i < associativity; i++){
					if(cache_blocks[i].rp_value > cache_blocks[lru].rp_value){
						cache_blocks[i].rp_value--;
					}
				}
				cache_blocks[lru].rp_value= associativity -1;
				
				return OK;
			}
		/*if(state==true){
			cout <<"hit"<<endl;
		}else cout <<"miss"<<endl;
		*/
		
		
   return ERROR;
}

int nru_replacement_policy(int idx,
                           int tag,
                           int associativity,
                           bool loadstore,
                           entry* cache_blocks,
                           operation_result* result,
                           bool debug)
{
	//se inicializa el resultado en miss
	
	bool state=false;
	
	for(int cache_pos = 0; cache_pos < associativity; cache_pos++){
		if(cache_blocks[cache_pos].valid==1){
			//si el bit de valido esta en 1 se revisa este bloque, si no se sigue
			if(cache_blocks[cache_pos].tag==tag){
				state = true;
				//se revisa si hay un hit y luego revisa si la instruccion es store
				if(loadstore==1){
					cache_blocks[cache_pos].dirty = true;
					result -> miss_hit = HIT_STORE;
				}
				else{
					result -> miss_hit = HIT_LOAD;
				}
				
				//se les hace update a los valores del NRU bit 
				
				
				cache_blocks[cache_pos].rp_value=0;
			}
			
			
			
			
		}
	}
	
	//miss
	
	
	if(state==false){
			int nru=0;
			bool reset=true;
			
			//busca el primer 1 en el nru bit de izquierda a derecha y guarda la posicion en nru 
			//reset es true si no hay ningun uno en los nru bits 
			for(int i=0; i<associativity;i++){
				if(cache_blocks[i].rp_value == 1){
					nru=i;
					reset=false;
					break;
				}
				
			}
			//reinicia la matriz 
			if(reset==true){
				for(int i=0; i<associativity;i++){
					cache_blocks[i].rp_value = 1;
				
				}
				
			}
			cache_blocks[nru].rp_value=0;
			
			
			//si se desaloja,se guardan los valores del bloque en los eviction
			if(cache_blocks[nru].valid){
				result->dirty_eviction = cache_blocks[nru].dirty;
				result->evicted_address = cache_blocks[nru].tag;
			}
			
			cache_blocks[nru].tag = tag;
			cache_blocks[nru].valid =1;
			
			//si fue store se  actualiza el dirty bit y result
			if(loadstore==1){
				cache_blocks[nru].dirty = true;
				result -> miss_hit = MISS_STORE;
			}
			else{
				result -> miss_hit = MISS_LOAD;
			}
			
			
			return OK;
		}
	/*if(state==true){
		cout <<"hit"<<endl;
	}else cout <<"miss"<<endl;
	*/
	
	
   return ERROR;
   
}
