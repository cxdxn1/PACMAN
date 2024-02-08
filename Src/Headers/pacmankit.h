// pacmankit.h
// Copyright © cxdxn1 2024, all rights reserved

#ifndef PACMANKIT_H
#define PACMANKIT_H

#include <stdint.h>
#include <stdio.h>
#include <mach/i386/kern_return.h>

// Offset in bytes within a PacmanUser IOUserClient to the helper field
#define PACMANKIT_TO_HELPER 0xE0

// Operations supported by the PacmanKit kext
typedef enum {
    KernelBase = 0x00,
    Read = 0x01,
    Write = 0x02,
    KernelVirt2Phys = 0x03,
    UserVirt2Phys = 0x04,
    IOUserClientLeak = 0x05,
    GimmeMemory = 0x06,
    FreeMemory = 0x07,
    TellMeRegs = 0x08,
    ReadForTiming = 0x09,
    ExecForTiming = 0x0A,
    LeakMethod = 0x0B,
    ReadForSpectre = 0x0C,
    ExecForSpectre = 0x0D,
    CallServiceRoutine = 0x0E,
    ForgeSignData = 0x0F,
    ForgeAuthData = 0x10,
    ForgeSignInst = 0x11,
    ForgeAuthInst = 0x12,
    LeakCurProc = 0x13,
} PacmanKitOp;

// PacmanKitConnection structure
typedef struct {
    uint64_t handle;
    uint64_t service;
} PacmanKitConnection;

// Function declarations
kern_return_t PacmanKitConnection_init(PacmanKitConnection *conn);

#endif // PACMANKIT_H
