#ifndef PTRACE_FAKE
#define PTRACE_FAKE

#include <stdint.h>
#include <sys/types.h>
#include "cpu.h"


enum {
    PTRACE_ATTACH = 1,
    PTRACE_DETACH,
    PTRACE_CONT,
    PTRACE_SINGLESTEP,
    PTRACE_GETREGS,
    PTRACE_SETREGS,
    PTRACE_PEEKDATA,
    PTRACE_POKEDATA
};

struct fake_process {
    pid_t pid;
    struct CPU cpu;
    uint8_t *memory;
    int stopped;
};
long fake_ptrace(int request, pid_t pid, void *addr, void *data);

struct fake_process *fp_get(pid_t pid);
void fp_register(struct fake_process *p);

#endif
