


#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <math.h>
#include <sstream>
#include <bitset>
#include <cstdio>


#include <array>
#include <cstdlib>
#include <time.h>

#define ADDRSIZE 32;
using namespace std;


/* 
 * ENUMERATIONS 
 */

/* Return Values */
enum returns_types {
 OK,
 PARAM,
 ERROR
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


struct entry {
 /* Indicates if the line is valid */
 bool valid;

 /* Indicates if the entry was written */
 bool dirty;

 /* Tag value */
 int tag;

 /* Replacement policy value */
 uint8_t rp_value;

 /* Prefetch tag, only used in OBL */
 bool obl_tag;
};

/* Cache replacement policy results */
struct operation_result {
 /* Result of the operation */
 enum miss_hit_status miss_hit;

 /* Set to one if the evicted line was dirty */
 bool dirty_eviction;

 /* Block address of the evited line */
 int  evicted_address;
};
/* Cache params */
struct cache_params {
  /* Total size of the cache in Kbytes */
  int size;

  /* Number of ways of the cache */  
  int asociativity;

  /* Size of each cache line in bytes */
  int block_size;
};

/* Address field size */
struct cache_field_size {
  /* Number of bits used for the tag */
  int tag;

  /* Number of bits used for the idx */
  int idx;

  /* Number of bits used for the offset */
  int offset;
};

int field_size_get(struct cache_params cache_params,
                   struct cache_field_size *field_size)
{
  //offset size log2(block_size)
  field_size->offset=log2(cache_params.block_size);
  //idx size = log2(sets/a)
  
  field_size->idx=log2((cache_params.size*1024)/cache_params.block_size/cache_params.asociativity);
  //tag = ADDRSIZE - offset - idx
  field_size->tag=32-field_size->offset-field_size->idx;
  return 0;
}

void address_tag_idx_get(long address,
                         struct cache_field_size field_size,
                         int *idx,
                         int *tag)                         
{
	bitset<32> b(address);
	string pcstr = b.to_string();
	string tagstr,idxstr;
    tagstr=pcstr.substr(4, field_size.tag);
    idxstr=pcstr.substr((field_size.tag), field_size.idx);
    //offset=prueba.substr(field_size.idx, field_size.offset);
    //cout<<"tagstr:"<<tagstr<<endl;
    *tag=static_cast<int>(bitset<32>(tagstr).to_ulong());
    *idx=static_cast<int>(bitset<32>(idxstr).to_ulong());

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
					if(cache_blocks[i].rp_value < cache_blocks[cache_pos].rp_value){
						cache_blocks[i].rp_value++;
					}
					cache_blocks[cache_pos].rp_value=0;
				}
				break;
				
				}
				
			}
		}
		
		//miss
		
		
		if(state==false){
				int lru=0;
			
				//busca el rp mayor para remplazarlo por el nuevo tag
				for(int i=0; i<associativity;i++){
					if(cache_blocks[i].rp_value > cache_blocks[lru].rp_value){
						lru=i;
					}
				}
				cache_blocks[lru].rp_value=0;
				
				
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
					if(i==lru){
						continue;
						//cache_blocks[i].rp_value = 0;
					}
					cache_blocks[i].rp_value++;
				}
				
				
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
				
				//se pone en 0 el rrip bit 
				
				
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
  

int main(int arcg, char * argv[]){

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
			if(RP==RRIP){
				int M=2;
				if(cache_params.asociativity <= 2)M=1;
				
				cache_blocks[i][j].rp_value=pow(2,M)-1;
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
	address_tag_idx_get(bulong,field_size, &idx,&tag);
	string loadstore = trace1.substr(2,1);
	ls=stoi(loadstore);
    
   
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
