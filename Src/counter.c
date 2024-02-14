// A thread used to measure a variable counting up
// Fork of Joseph Ravichandran (aka jprx) PacmanAttack, originally coded in Rust

// Copyright © cxdxn1 2024, all rights reserved

#ifndef COUNTER_H
#define COUNTER_H

#include <stdint.h>

// A globally visible counter that can be sampled to get a rough measurement of how far time has passed.
// Make sure you synchronize before sampling
extern uint64_t CTR;

// Continuously increments the counter variable to get a sense of how much time has passed.
void counter_thread(void);

// Sample the current counter value. Handles all synchronization as appropriate.
// This has NOT been tested and is deprecated
// See timer_multithread.c instead.
uint64_t read_counter(void);

#endif // COUNTER_H
