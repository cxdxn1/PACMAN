// counter.h
// Created by Rayyan Khan

#ifndef COUNTER_H
#define COUNTER_H

#include <stdint.h>

// A counter that can be used to see how much time has passed
extern uint64_t CTR;

// Continuously increment the counter variable to get a sense of how much time has passed.
void counter_thread(void);

// Sample the current counter value
// This has NOT been tested and was deprecated on the original repository - use at your own risk
// See timer_multithread.c instead
uint64_t read_counter(void);

#endif // COUNTER_H
