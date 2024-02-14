// evset.h
// Created by Rayyan Khan

#ifndef EVSET_H
#define EVSET_H

#include <stdint.h>
#include <stdlib.h>

// Define constants
#define STRIDE (4096 * PAGE_SIZE)
#define EVSET_SIZE_MAX 1024

// Define page size (assuming PAGE_SIZE is defined elsewhere)
#define PAGE_SIZE 4096

// Define L1D_WAYS, L1D_SETS, L1D_LINESIZE, L1I_WAYS, L1I_SETS, and L1I_LINESIZE
// These constants are assumed to be defined elsewhere

// PacmanKitConnection structure, replace with the actual structure definition
typedef struct {
    // Define the structure members here
} PacmanKitConnection;

// Function to get cache set for L2 cache from physical address
uint64_t get_cache_set_m1(uint64_t paddr);

// Returns a vector of addresses within kernel_memory that will contend with target_paddr
// Assumes PacmanKitConnection is defined elsewhere
void data_kpevset(uint64_t target_vaddr, uint64_t target_paddr, uint64_t kernel_memory, size_t kernel_memory_size);

#endif /* EVSET_H */
