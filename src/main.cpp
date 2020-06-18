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

void resultados(cache_params params, int miss_load, int miss_store, int hit_load, int hit_store, int dirt, int IC){
   double hit_ratio=(hit_load + hit_store)/(hit_load + hit_store + miss_load+miss_store);
   int lineas =hit_load + hit_store + miss_load+miss_store;
   cout << "_______________________________________________________" << endl;
   cout << "Cache Parameters" <<endl;
   cout << "_______________________________________________________ " <<endl;;
   cout << " Cache Size (KB):             " << params.size << endl;
   cout << " Cache Associativity:         " << params.asociativity << endl;
   cout << " Cache Block Size (bytes):    " << params.block_size << endl;
   cout << "_______________________________________________________" << endl;
   cout << " Simulation results" << endl;
   cout << "_______________________________________________________ "<<endl;
   cout << " CPU time (cycles):           " <<IC*(1+(1-hit_ratio)*lineas/IC*20 )<<endl;
   cout << " AMAT (cycles):               " << 1+(1-hit_ratio)*20<<endl;
   cout << " Overall miss rate:           " << ((double)miss_load + (double)miss_store)/((double)miss_load+(double)hit_load + (double)miss_store + (double)hit_store) << endl;
   cout << " Read miss rate:              " << (double)miss_load/(miss_load+hit_load) << endl;
   cout << " Dirty evictions:             " << dirt << endl;
   cout << " Load misses:                 " << miss_load <<endl;
   cout << " Store misses:                " << miss_store << endl;
   cout << " Total misses:                " << miss_load+miss_store <<endl;
   cout << " Load hits:                   " << hit_load << endl;
   cout << " Store hits:                  " << hit_store << endl;
   cout << " Total hits:                  " << hit_load + hit_store << endl;
   cout << "_______________________________________________________" << endl;
}


int main(int argc, char * argv []) {
  clock_t start = clock();
  int size=0, block_size=0,asociativity=0,tag=0,idx=0,ls,IC;
  int miss_load=0,  miss_store=0,  hit_load=0,  hit_store=0,  dirt =0;
  uint8_t rp_value=0;
  string pc,trace1, offset,trace,ICstr;
  
  
  struct cache_params cache_params;
  struct cache_field_size field_size;
  struct operation_result result;
  
  enum replacement_policy RP;

  sscanf(argv[2], "%d",&size);
  sscanf(argv[4],"%d",&asociativity);
  sscanf(argv[6], "%d",&block_size);
  sscanf(argv[8],"%c",&rp_value);
  
  if(rp_value=='0') RP=LRU;
  if(rp_value=='1') RP=NRU;
  if(rp_value=='2') RP=RRIP;
  
  cache_params.size=size;
  cache_params.block_size=block_size;
  cache_params.asociativity=asociativity;
  //se entra a la funciona para obtener los tamaños de los indices y tag 
  field_size_get(cache_params, &field_size);	
  
  int sets=pow(2,field_size.idx);
  //se crea L1
  struct entry cache_blocks[sets][cache_params.asociativity];
	//se inicializa L1, los valores de rp cambian dependiendo de la politica de remplazo
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
	//se extrae el address del trace
	pc=trace1.substr(4, 8);
	stringstream ss;
	ss << hex << pc;
	unsigned n;
	ss >> n;
	//se convierte a binario para luego enviarlo como long a la funcion
	bitset<32> b(n);
	long bulong=b.to_ulong();
	
	//Esta funcion devuelve el indice y la etiqueta de el bloque
	address_tag_idx_get(bulong,field_size, &idx,&tag);
	//se obtiene el valor de ls y de IC
	string loadstore = trace1.substr(2,1);
	ls=stoi(loadstore);
    ICstr=trace1.substr(13,2);
	IC+=stoi(ICstr);
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
		return 1;
		cout<<"ERROR RP"<<endl;
	 }
		//suma de los resultados del cache para probar el trace
		if(result.miss_hit == MISS_LOAD) miss_load++;
		if(result.miss_hit == MISS_STORE) miss_store++;
		if(result.miss_hit == HIT_LOAD) hit_load++;
		if(result.miss_hit == HIT_STORE) hit_store++;
		if(result.dirty_eviction == 1) dirt++; 
    
	}
	//esta funcion imprime los parametros para probar el trace
	resultados(cache_params, miss_load, miss_store, hit_load, hit_store, dirt,IC);
	
	cout<<"tiempo: "<<(((double)clock()-start)/CLOCKS_PER_SEC)/60<< " min." <<endl;
	
return 0;
}
