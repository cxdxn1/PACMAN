#include "Headers/msr.h"

uint64_t read_ctr_el0() {
    uint64_t val;
    asm volatile("mrs %0, ctr_el0" : "=r"(val));
    return val;
}
