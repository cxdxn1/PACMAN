// Header file for pac.h

#ifndef PAC_H
#define PAC_H

#include <stdint.h>
#include <stdbool.h>

typedef uint16_t PAC;

#define PAC_BITMASK 0xFFFF800000000000ULL
#define NUM_PACS 0xFFFF
#define MAX_PAC 0xFFFF

PAC extract_pac(uint64_t signed_pointer);
uint64_t encode_pac(PAC pac, uint64_t pointer);
bool is_kernel_pointer(uint64_t addr);

typedef struct {
    PAC cur_pac;
    bool reported_last;
    uint64_t addr;
} PACIterator;

uint64_t PACIterator_next(PACIterator* iterator);
PACIterator iterate_pacs(uint64_t addr);

#endif // PAC_H
