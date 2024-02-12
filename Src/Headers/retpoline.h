// retpoline.h
// Copyright © cxdxn1 2024, all rights reserved

#ifndef RETPOLINE_H
#define RETPOLINE_H

#include <stdio.h>
#include <mach/mach.h>

#define RET_INST 0xd65f03c0
#define NOP_INST 0xd503201f

int mk_retpoline_page(uint8_t *page, size_t page_size);
int mk_retpoline_addr(uint64_t addr, size_t size);

#endif /* RETPOLINE_H */