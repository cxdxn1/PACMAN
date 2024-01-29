// Source file utility for working with PACs
// Fork of Joseph Ravichandran (aka jprx) PacmanAttack, originally coded in Rust

// Copyright © cxdxn1 2024, all rights reserved

#include "Headers/pac.h"

// Function to extract the Pointer Authentication Code (PAC) from a signed pointer.
PAC extract_pac(uint64_t signed_pointer) {
    PAC pac_lower = (signed_pointer >> 47) & 0x0FF;
    PAC pac_upper = (signed_pointer >> 56) & 0x0FF;

    // Combine the upper and lower parts to form the complete 16-bit PAC.
    return (pac_upper << 8) | pac_lower;
}

uint64_t encode_pac(PAC pac, uint64_t pointer) {
    uint64_t pac_lower = (pac & 0x0FF) << 47;
    uint64_t pac_upper = ((pac >> 8) & 0x0FF) << 56;

    uint64_t pac_kernel_bit = is_kernel_pointer(pointer) ? (1ULL << 55) : 0;

    return (pointer & (~PAC_BITMASK)) | pac_lower | pac_upper | pac_kernel_bit;
}

bool is_kernel_pointer(uint64_t addr) {
    return (addr & (1ULL << 55)) != 0;
}

uint64_t PACIterator_next(PACIterator* iterator) {
    PAC old_pac = iterator->cur_pac;

    if (iterator->cur_pac == MAX_PAC) {
        if (iterator->reported_last) {
            return 0;
        }
        iterator->reported_last = true;
    }
    else {
        iterator->cur_pac = iterator->cur_pac + 1;
    }

    return encode_pac(old_pac, iterator->addr);
}

PACIterator iterate_pacs(uint64_t addr) {
    PACIterator iterator;
    iterator.cur_pac = 0;
    iterator.reported_last = false;

    if (is_kernel_pointer(addr)) {
        iterator.addr = addr | PAC_BITMASK;
    }
    else {
        iterator.addr = addr & (~PAC_BITMASK);
    }

    return iterator;
}

