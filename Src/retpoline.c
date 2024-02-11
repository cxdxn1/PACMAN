// retpoline.c
// Copyright © cxdxn1 2024, all rights reserved

#include "Headers/retpoline.h"
#include <mach/mach_vm.h>

int mk_retpoline_page(uint8_t *page, size_t page_size) {
    if (page_size < vm_page_size) {
        return -1;
    }

    kern_return_t result;
    result = mach_vm_protect(mach_task_self(), (mach_vm_address_t)page, vm_page_size, 0, VM_PROT_READ | VM_PROT_WRITE);
    if (result != KERN_SUCCESS) {
        printf("Error calling mach_vm_protect! Error code 0x%X\n", result);
        return -1;
    }

    uint32_t *page_as_u32 = (uint32_t *)page;
    for (size_t i = 0; i < vm_page_size / sizeof(uint32_t); i++) {
        page_as_u32[i] = RET_INST;
    }

    result = mach_vm_protect(mach_task_self(), (mach_vm_address_t)page, vm_page_size, 0, VM_PROT_READ | VM_PROT_EXECUTE);
    if (result != KERN_SUCCESS) {
        printf("Error calling mach_vm_protect! Error code 0x%X\n", result);
        return -1;
    }

    return 0;
}

int mk_retpoline_addr(uint64_t addr, size_t size) {
    kern_return_t result;

    result = mach_vm_protect(mach_task_self(), (mach_vm_address_t)addr, size, 0, VM_PROT_READ | VM_PROT_WRITE);
    if (result != KERN_SUCCESS) {
        printf("Error calling mach_vm_protect! Error code 0x%X\n", result);
        return -1;
    }

    uint32_t *page_as_u32 = (uint32_t *)addr;
    for (size_t i = 0; i < size / sizeof(uint32_t); i++) {
        page_as_u32[i] = RET_INST;
    }

    result = mach_vm_protect(mach_task_self(), (mach_vm_address_t)addr, size, 0, VM_PROT_READ | VM_PROT_EXECUTE);
    if (result != KERN_SUCCESS) {
        printf("Error calling mach_vm_protect! Error code 0x%X\n", result);
        return -1;
    }

    return 0;
}
