#include "utilities.h"


parameters getParams(int argc, char* argv[]){
  parameters param;
  //Default values are given:
  param.t   = 0;
  param.l   = 0;
  param.a   = 0;
  param.opt = 0;

  for (int i = 1; i < argc; i = i + 2) {
    if (strcmp("-t", argv[i]) == 0) {
      param.t = atoi(argv[i+1]);
    } else if (strcmp("-l", argv[i]) == 0) {
      param.l = atoi(argv[i+1]);
    } else if (strcmp("-a", argv[i]) == 0) {
      param.a = atoi(argv[i+1]);
    } else if (strcmp("-opt", argv[i]) == 0) {
      param.opt = atoi(argv[i+1]);
    }
  }
  return param;
}

void printParams(parameters params){
  const char* division = "------------------------------------------------------------------------";
  printf("%s\n%s\n%s\n",division, "Cache parameters:", division );

  const char* optimization;
  switch (params.opt){
    case L2:
      optimization = "Multilevel cache";
      printf("L1 Cache Size (kilobytes): \t\t\t\t %u\n", params.t);
      printf("L1 Cache Block Size (bytes): \t\t\t\t %u\n", params.l);
      printf("L1 Cache Associativity: \t\t\t\t %u\n", params.a);
      printf("L2 Cache Size (kilobytes): \t\t\t\t %u\n", FACT_C_SIZE_L2 * params.t);
      printf("L2 Cache Block Size (bytes): \t\t\t\t %u\n", params.l);
      printf("L2 Cache Associativity: \t\t\t\t %u\n", FACT_ASOC_L2 * params.a);
      printf("Optimization: \t\t\t\t\t\t %s\n", optimization);
    break;
    case VC:
      optimization = "Victim Cache";
      printf("L1 Cache Size (kilobytes): \t\t\t\t %u\n", params.t);
      printf("L1 Cache Block Size (bytes): \t\t\t\t %u\n", params.l);
      printf("L1 Cache Associativity: \t\t\t\t %u\n", params.a);
      printf("VC Cache Associativity: \t\t\t\t %u\n", VC_SIZE);
      printf("Optimization: \t\t\t\t\t\t %s\n", optimization);
    break;
    default: optimization = "Not Specified";
  }

  printf("%s\n", division);
}

sizes get_sizes(parameters params){
  sizes sizes = {};
  // L1
  sizes.l1_assoc       = params.a;
  sizes.l1_offset_bits = (u_int) log2f(params.l);
  sizes.l1_index_bits  = (u_int) log2f((params.t*(1024))/(params.a*params.l));
  sizes.l1_tag_bits    = ADDRESS_SIZE - (sizes.l1_index_bits + sizes.l1_offset_bits);
  // L2
  sizes.l2_assoc       = FACT_ASOC_L2 * params.a;
  sizes.l2_offset_bits = (u_int) log2f(params.l);
  sizes.l2_index_bits  = (u_int) log2f((FACT_C_SIZE_L2*params.t*(1024))/(sizes.l2_assoc*params.l));
  sizes.l2_tag_bits    = ADDRESS_SIZE - (sizes.l2_index_bits + sizes.l2_offset_bits);
  // VC
  sizes.vc_assoc       = VC_SIZE;
  return sizes;
}

void print_sizes(sizes sizes, int opt){
  const char* division = "------------------------------------------------------------------------";
  printf("%s\n%s\n%s\n",division, "Sizes infomation:", division);
  printf("L1\n");
  printf(" tag bits: \t\t %u\n",sizes.l1_tag_bits);
  printf(" index bits: \t\t %u\n",sizes.l1_index_bits);
  printf(" offset bits: \t\t %u\n", sizes.l1_offset_bits);
  printf(" asociativity: \t\t %u\n", sizes.l1_assoc);
  switch (opt) {
    case L2: {
      printf("L2\n");
      printf(" tag bits: \t\t %u\n",sizes.l2_tag_bits);
      printf(" index bits: \t\t %u\n",sizes.l2_index_bits);
      printf(" offset bits: \t\t %u\n", sizes.l2_offset_bits);
      printf(" asociativity: \t\t %u\n", sizes.l2_assoc);
      break;
    }
    case VC: {
      printf("VC\n");
      printf(" asociativity: \t\t %u\n", sizes.vc_assoc);
      break;
    }
    default: break;
  }
  printf("%s\n", division);
}

line_info get_line_info(char* line){
	char buffer[400];
  strcpy(buffer, line);
	int cont = 0;
	char *token = strtok(buffer," ");
  line_info info = {};
	while(token != NULL){
	  if(cont == 1){
	  	info.loadstore = atoi(token);
	  }
	  if(cont == 2){
	  	info.address = strtol(token,NULL,16);
	  }
    if(cont == 3){
	  	info.ic = atoi(token);
	  }
	  token = strtok(NULL," ");
	  cont ++;
	}
  return info;
}

entry_info get_entry_info(long address, sizes sizes){
  entry_info info = {};
  // L1 info
  info.original_address = (u_int) address;
  u_int aux_address = info.original_address;
  info.l1_offset = aux_address & ((1<< sizes.l1_offset_bits) -1 );
  aux_address = aux_address >> sizes.l1_offset_bits;
  info.l1_idx = aux_address & ((1<< sizes.l1_index_bits) -1 );
  aux_address = aux_address >> sizes.l1_index_bits;
  info.l1_tag = aux_address & ((1<< sizes.l1_tag_bits) -1 );
  info.l1_assoc = sizes.l1_assoc;
  // L2 inf
  aux_address = info.original_address;
  info.l2_offset = address & ((1<< sizes.l2_offset_bits) -1 );
  aux_address = aux_address >> sizes.l2_offset_bits;
  info.l2_idx = aux_address & ((1<< sizes.l2_index_bits) -1 );
  aux_address = aux_address >> sizes.l2_index_bits;
  info.l2_tag = aux_address & ((1<< sizes.l2_tag_bits) -1 );
  info.l2_assoc = sizes.l2_assoc;
  // VC info
  info.vc_assoc = sizes.vc_assoc;
  return info;
}

void print_entry_info(entry_info info, int opt){
  const char* division = "------------------------------------------------------------------------";
  printf("%s\n%s\n%s\n",division, "Entry infomation:", division);
  printf(" Origianl Address: 0x%X\n", info.original_address);
  printf("L1\n");
  printf(" tag: \t\t\t 0x%X\n",info.l1_tag);
  printf(" index: \t\t 0x%X\n",info.l1_idx);
  printf(" offset: \t\t 0x%X\n",info.l1_offset);
  printf(" asociativity: \t\t %u\n", info.l1_assoc);
  switch (opt) {
    case L2: {
      printf("L2\n");
      printf(" tag: \t\t\t 0x%X\n",info.l2_tag);
      printf(" index: \t\t 0x%X\n",info.l2_idx);
      printf(" offset: \t\t 0x%X\n", info.l2_offset);
      printf(" asociativity: \t\t %u\n", info.l2_assoc);
      break;
    }
    case VC: {
      printf("VC\n");
      printf(" asociativity: \t\t %u\n", info.vc_assoc);
      break;
    }
    default: break;
  }
  printf("%s\n", division);
}

void print_address(u_int addr, const char* msj){
  printf("%s 0x%X\n",msj,addr);
}

void print_entry(entry block,const char* msj){
   printf("%s{v=%u, t=0x%X, db=%u, rpv=%u} ",msj,block.valid,block.tag,block.dirty,block.rp_value);
}

void print_set(entry* set, int assoc, const char* msj){
   printf("%s",msj);
   for(size_t i = 0; i < assoc; i++){
      print_entry(set[i]);
   }
   printf("\n");
}

void print_result(operation_result results,const char* msj){
   printf("%s{mh=%i, de=%i, ea=%X}\n",msj,results.miss_hit,results.dirty_eviction,results.evicted_address);
}

void printStats(statistics stats, double exec_time,int opt){
  double L1_miss_rate      = (double) stats.L1_misses/stats.references;
  double L2_miss_rate      = (double) stats.L2_misses/stats.L1_misses; // o es? = stats.L2_misses/stats.references;
  double global_miss_rate  = (double) stats.global_misses/stats.references;
  double L1_VC_miss_rate   = (double) (stats.L1_misses + stats.VC_misses)/stats.references;
  const char* division = "------------------------------------------------------------------------";

  switch (opt)
  {
  case L2:
    printf("L1 miss rate: \t\t\t\t\t\t %f\n", L1_miss_rate);
    printf("L2 miss rate: \t\t\t\t\t\t %f\n", L2_miss_rate);
    printf("Global miss rate: \t\t\t\t\t %f\n", global_miss_rate);
    printf("Misses L1: \t\t\t\t\t\t %u\n", stats.L1_misses);
    printf("Hits L1: \t\t\t\t\t\t %u\n", stats.L1_hits);
    printf("Misses L2: \t\t\t\t\t\t %u\n", stats.L2_misses);
    printf("Hits L2: \t\t\t\t\t\t %u\n", stats.L2_hits);
    printf("Dirty evictions L2: \t\t\t\t\t %u\n",stats.dirty_evictions);
    break;
  case VC:
    printf("Miss rate (L1+VC): \t\t\t\t\t %f\n", L1_VC_miss_rate);
    printf("Misses (L1+VC): \t\t\t\t\t %u\n", stats.L1_misses + stats.VC_misses);
    printf("Hits (L1+VC): \t\t\t\t\t\t %u\n", stats.L1_hits + stats.VC_hits);
    printf("Victim cache hits: \t\t\t\t\t %u\n",stats.VC_hits);
    printf("Dirty evictions: \t\t\t\t\t %u\n", stats.dirty_evictions);
    break;
  default:
    break;
  }

  printf("%s\n", division);
  printf("Tiempo de ejecución: %lf s\n",exec_time);
}

void updateStats(statistics* stats, operation_result* l1_results, operation_result* vc_l2_results, u_int opt){
  stats->references++;

  //L1 hit: no access to L2 or VC is needed, so the L2 and VC statistics won't be affected.
  if(l1_results->miss_hit == HIT_LOAD || l1_results->miss_hit == HIT_STORE){
    stats->L1_hits++;
  } else { //L1 miss: L2 or VC access is required. Statistics for L2 or VC will be affected.
    stats->L1_misses++;
    if (vc_l2_results->miss_hit == HIT_LOAD || vc_l2_results->miss_hit == HIT_STORE) { //VC hit or L2 hit:
      if(opt == VC){stats->VC_hits++;}
      if(opt == L2){stats->L2_hits++;}
    } else { //VC miss or L2 miss
      if(opt == VC){stats->VC_misses++;}
      if(opt == L2){stats->L2_misses++; stats->global_misses++;}

      // dirty eviction for VC optimization: we assume an L1 eviction is passed to MM and inserted in VC as clean.
      if(l1_results->dirty_eviction && opt == VC){
        stats->dirty_evictions++;
      }
      //dirty_eviction for L2 optimization: since caches are inclusive the evictions are passed to MM when evicted from L2
      if(vc_l2_results->dirty_eviction && opt == L2){
        stats->dirty_evictions++;
      }
    }
  }
}
