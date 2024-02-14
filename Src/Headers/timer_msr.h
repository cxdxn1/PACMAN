// timer_msr.h
// Created by Rayyan Khan

// Kept comments from the original repository, please note that this is untested and may not work.
// If anyone finds issues please contact me or feel free to contribute yourself
// I will probably fix this at some point

#include <stdint.h>

// The overhead of doing timer measurements with a NOP
// This can be measured for your system with timer_overhead()
// Set this to 0 to ignore. This value will be different depending on if
// you are running on a P or E core.
#define TIMER_OVERHEAD_PCORE 56
#define TIMER_OVERHEAD_ECORE 52

/**
 * Returns the time to access a given address using the high resolution timers
 *
 * `S3_2_c15_c0_0` == `SREG_PMC0` (Cycle Counter).
 * Assumes the Pacman kernel patches are applied such that the timers are usable from EL0.
 */
uint64_t time_access(uint64_t addr) {
    uint64_t t1, t2, val_out;
    asm volatile (
            "dsb sy\n"
            "isb\n"
            "mrs %0, S3_2_c15_c0_0\n"
            "isb\n"
            "ldr %2, [%3]\n"
            "isb\n"
            "mrs %1, S3_2_c15_c0_0\n"
            "isb\n"
            "dsb sy\n"
            : "=r"(t1), "=r"(t2), "=r"(val_out)
            : "r"(addr)
            );
    // Doing no load at all with 2 ISB's in between results in 56 cycles.
    // Doing only 1 ISB in between results in 28 (sometimes 26) cycles.
    return t2 - t1 - TIMER_OVERHEAD_PCORE;
}

/**
 * Returns the time to write to a given address using the high resolution timers
 *
 * `S3_2_c15_c0_0` == `SREG_PMC0` (Cycle Counter).
 * Assumes the Pacman kernel patches are applied such that the timers are usable from EL0.
 */
uint64_t time_store(uint64_t addr) {
    uint64_t t1, t2, val_in = 0x3131313131313131;
    asm volatile (
            "dsb sy\n"
            "isb\n"
            "mrs %0, S3_2_c15_c0_0\n"
            "isb\n"
            "str %2, [%3]\n"
            "isb\n"
            "mrs %1, S3_2_c15_c0_0\n"
            "isb\n"
            "dsb sy\n"
            : "=r"(t1), "=r"(t2)
            : "r"(val_in), "r"(addr)
            );
    // Doing no load at all with 2 ISB's in between results in 56 cycles.
    // Doing only 1 ISB in between results in 28 (sometimes 26) cycles.
    return t2 - t1 - TIMER_OVERHEAD_PCORE;
}

/**
 * Returns the time to execute a given address using the high resolution timers
 *
 * `S3_2_c15_c0_0` == `SREG_PMC0` (Cycle Counter).
 * Assumes the Pacman kernel patches are applied such that the timers are usable from EL0.
 */
uint64_t time_exec(uint64_t addr) {
    uint64_t t1, t2;
    asm volatile (
            "dsb sy\n"
            "isb\n"
            "mrs %0, S3_2_c15_c0_0\n"
            "isb\n"
            "blr %1\n"
            "isb\n"
            "mrs %1, S3_2_c15_c0_0\n"
            "isb\n"
            "dsb sy\n"
            : "=r"(t1), "=r"(t2)
            : "r"(addr)
            );
    // Doing no load at all with 2 ISB's in between results in 56 cycles.
    // Doing only 1 ISB in between results in 28 (sometimes 26) cycles.
    return t2 - t1 - TIMER_OVERHEAD_PCORE;
}

/**
 * Returns the constant time offset associated with performing measurements.
 * This number can be measured for a platform and then treated as a constant.
 */
uint64_t timer_overhead() {
    uint64_t t1, t2;
    asm volatile (
            "isb\n"
            "mrs %0, S3_2_c15_c0_0\n"
            "isb\n"
            "nop\n" // Do a NOP instead of a LDR here
            "isb\n"
            "mrs %1, S3_2_c15_c0_0\n"
            "isb\n"
            : "=r"(t1), "=r"(t2)
            );
    return t2 - t1;
}

/**
 * Reports the time for a cache miss.
 *
 * # Arguments
 * * `untouched_page`: A page that has been allocated but never written to/ read from
 *   (and is therefore not present in the TLB).
 *
 * # Return Value
 * Returns the number of cycles on a cache miss as reported by `timer_access`.
 *
 * # Side Effects
 * Will load several addresses from the page.
 *
 * # References
 * See 'Branch Different' by Hetterich and Schwarz Section 3.2 Listing 1.
 */


#define L2_LINESIZE 64  // Replace with the actual value if it's different

uint64_t time_miss(uint8_t *untouched_page) {
    time_access((uint64_t)&untouched_page[0]);
    return time_access((uint64_t)&untouched_page[L2_LINESIZE * 3]);
}
/**
 * Reports the time for a cache hit.
 *
 * # Arguments
 * * `page`: A page that can be read from.
 *
 * # Return Value
 * Returns the number of cycles on a cache hit as reported by `timer_access`.
 *
 * # Side Effects
 * Will load several addresses from the page.
 *
 * # References
 * See 'Branch Different' by Hetterich and Schwarz Section 3.2 Listing 1.
 */
uint64_t time_hit(uint8_t *page) {
    time_access((uint64_t)&page[0]);
    return time_access((uint64_t)&page[0]);
}

