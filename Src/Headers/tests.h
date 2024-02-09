// tests.h


#ifndef TEST_CODE_H
#define TEST_CODE_H

#include <stdint.h>
#include <stdio.h>

// Define necessary structures and functions used in Rust code
typedef struct {
    // Define PacmanKitConnection structure members
    // ...
} PacmanKitConnection;

typedef enum {
    INTEGER_CORE,
    FLOATING_POINT_CORE,
    VECTOR_CORE,
    STRING_CORE,
    BOOLEAN_CORE,
    POINTER_CORE,
    STRUCTURE_CORE,
    ARRAY_CORE,
    FUNCTION_CORE,
    // Add more core types as needed
} CoreKind;


typedef struct {
    // Define other necessary structures used in Rust code
    // ...
} CacheSet;

// Function declarations
void test_timers(uint8_t* shared_mem);
void data_ev_set_test(uint8_t* shared_mem);
// void inst_ev_set_test(uint8_t* shared_mem);
void inst_pev_set_test(uint8_t* shared_mem);
void test_pacmankit();
void test_forge_pacs();

// Placeholder for read_counter
uint64_t read_counter();

// Placeholder for other functions used in Rust code
// ...

#endif // TEST_CODE_H



