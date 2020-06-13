#include "utilities.h"


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
