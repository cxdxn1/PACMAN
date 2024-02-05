// Header file for attacks.h
// Copyright © cxdxn1 2024, all rights reserved

#ifndef ATTACK_H
#define ATTACK_H

// Evict+Reload implementation for creating latency graphs
#include "Attacks/evict_reload.h"

// Prime+Probe testbed for determining effective prime+probe parameters
#include "Attacks/prime_probe.h"

// Spectre testbed for testing speculative execution in the kernel
#include "Attacks/spectre.h"

// The actual PACMAN attack code itself
#include "Attacks/pacman.h"

#endif // ATTACK_H

