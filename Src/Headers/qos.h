// Header file for qos.h

#include <stdint.h>

// sys/qos.h

// Constants for the different kinds of cores on the M1 SoC.
typedef enum {
    PCORE = 0x21, // QOS_CLASS_USER_INTERACTIVE
    ECORE = 0x09, // QOS_CLASS_BACKGROUND
} CoreKind;

// See pthread/qos.h
extern int pthread_set_qos_class_self_np(CoreKind flavor, uint64_t priority);

// Function to set the QoS class for the calling thread based on the provided 'kind'.
// Returns 1 if successful, 0 otherwise.
int set_core(CoreKind kind) {
    return pthread_set_qos_class_self_np(kind, 0) == 0 ? 1 : 0;
}


// Function to retrieve the ID of the current core.
// Uses some (simple) assembly to read the TPIDRRO_EL0 register and extracts the core ID.
uint64_t core_id() {
    uint64_t cur_core;
    asm volatile (
            "mrs %0, TPIDRRO_EL0"
            : "=r" (cur_core)
            );
    return cur_core & 0x07;
}

