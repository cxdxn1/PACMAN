// Header file for attacks.h

#ifndef ATTACK_H
#define ATTACK_H

// Evict+Reload implementation for creating latency graphs
#include "evict_reload.h"

// Prime+Probe testbed for determining effective prime+probe parameters
#include "prime_probe.h"

// Spectre testbed for testing speculative execution in the kernel
#include "spectre.h"

// The actual PACMAN attack code itself
#include "pacman.h"

#endif // ATTACK_H

