// cache.c
// Created by Rayyan Khan

#include "Headers/cache.h"

uint64_t get_cache_tag_generic(uint64_t addr) {
    uint64_t set_shift = (uint64_t)ceil(log2(L2_SETS));
    uint64_t offset_shift = (uint64_t)ceil(log2(L2_LINESIZE));
    return (addr >> (set_shift + offset_shift));
}

uint64_t get_cache_set_generic(uint64_t addr) {
    uint64_t offset_shift = (uint64_t)ceil(log2(L2_LINESIZE));
    return (addr >> offset_shift) & (L2_SETS - 1u);
}

uint64_t get_cache_offset_generic(uint64_t addr) {
    return addr & (L2_LINESIZE - 1u);
}

inline uint64_t get_cache_tag_m1(uint64_t addr) {
    return (addr >> 20) & 0xFFFFFFFFFFFULL;
}

inline uint64_t get_cache_set_m1(uint64_t addr) {
    return (addr >> 7) & 0xFFFF;
}

inline uint64_t get_l1_cache_set_m1(uint64_t addr) {
    return (addr >> 6) & 0x1FF;
}

inline uint64_t get_cache_offset_m1(uint64_t addr) {
    return addr & 0x7F;
}


