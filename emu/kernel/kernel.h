#ifndef KERNEL_H
#define KERNEL_H

#include "../cpu.h"

// syscalls
#define SYS_EXIT  1
#define SYS_WRITE 4

void kernel_init(void);
void kernel_handle_syscall(struct CPU *cpu, uint8_t *memory);

#endif
