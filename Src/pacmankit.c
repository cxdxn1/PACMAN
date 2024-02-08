// pacmankit.c
// Copyright © cxdxn1 2024, all rights reserved

#include "Headers/pacmankit.h"
#include <IOKit/IOKitLib.h>
#include <mach/mach_error.h>
#include <stdio.h>

// Function implementations
kern_return_t PacmanKitConnection_init(PacmanKitConnection *conn) {
    kern_return_t kret;
    io_name_t name;

    io_service_t serv = IOServiceGetMatchingService(kIOMainPortDefault, IOServiceMatching("PacmanKit"));
    if (IO_OBJECT_NULL == serv) {
        printf("Couldn't find PacmanKit. Did you remember to install it?\n");
        return KERN_FAILURE;
    }

    IORegistryEntryGetName(serv, name);
    kret = IOServiceOpen(serv, mach_task_self(), 0, &conn->handle);
    if (KERN_SUCCESS != kret) {
        printf("Couldn't connect to IOService %s (error %s)\n", name, mach_error_string(kret));
        IOObjectRelease(serv);
        return kret;
    }

    conn->service = serv;
    return KERN_SUCCESS;
}

// Implement other functions similarly
