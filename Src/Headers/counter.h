// Header file for counter.h
// Copyright © cxdxn1 2024, all rights reserved

#ifndef COUNTER_H
#define COUNTER_H

#include <stdint.h>

// Include necessary headers from the crate
#include "Crate/crate_header1.h"
#include "Crate/crate_header2.h"

// A counter that can be used to see how much time has passed
extern uint64_t CTR;

// Continuously increment the counter variable to get a sense of how much time has passed.
void counter_thread(void);

// Sample the current counter value
// This has NOT been tested and was deprecated on the original repository - use at your own risk
// See timer_multithread.c instead
uint64_t read_counter(void);

#endif // COUNTER_H
