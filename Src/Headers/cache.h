// Header file for cache.h

#ifndef CACHE_H
#define CACHE_H

#include <stdint.h>
#include <math.h>

#define L1D_WAYS 8
#define L1D_SETS 256
#define L1D_LINESIZE 64

#define L1I_WAYS 6
#define L1I_SETS 512
#define L1I_LINESIZE 64

#define L2_WAYS 12
#define L2_SETS 8192
#define L2_LINESIZE 128

#define L2_SIZE 0xC00000
#define L1I_SIZE 0x30000
#define PAGE_SIZE 0x4000
#define PAGE_OFFSET_MASK 0x3FFF
#define TLB_OFFSET_MASK 0x7FFFFFFFFFULL

uint64_t get_cache_tag_generic(uint64_t addr);
uint64_t get_cache_set_generic(uint64_t addr);
uint64_t get_cache_offset_generic(uint64_t addr);
uint64_t get_cache_tag_m1(uint64_t addr);
uint64_t get_cache_set_m1(uint64_t addr);
uint64_t get_l1_cache_set_m1(uint64_t addr);
uint64_t get_cache_offset_m1(uint64_t addr);

#endif // CACHE_H

