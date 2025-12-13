#include "ptrace-fake.h"
#include <string.h>
#include <errno.h>

#include "cpu.h"
#include "memory.h"


static struct fake_process *proc_table[32];
static int proc_count = 0;

void fp_register(struct fake_process *p){
    proc_table[proc_count++] = p;
}

struct fake_process *fp_get(pid_t pid) {
    for(int i=0;i<proc_count;i++)
        if(proc_table[i]->pid == pid)
            return proc_table[i];
    return NULL;
}

long fake_ptrace(int request, pid_t pid, void *addr, void *data) {
    struct fake_process *p = fp_get(pid);
    if(!p) return -ESRCH;

    switch(request) {

    case PTRACE_ATTACH:
        p->stopped = 1;
        return 0;

    case PTRACE_DETACH:
        p->stopped = 0;
        return 0;

    case PTRACE_SINGLESTEP:
        cpu_step(&p->cpu, p->memory);
        p->stopped = 1;
        return 0;

    case PTRACE_CONT:
        p->stopped = 0;
        return 0;

    case PTRACE_GETREGS:
        memcpy(data, &p->cpu, sizeof(struct CPU));
        return 0;

    case PTRACE_SETREGS:
        memcpy(&p->cpu, data, sizeof(struct CPU));
        return 0;

    case PTRACE_PEEKDATA:
        *(uint32_t*)data =
            *(uint32_t*)(p->memory + (uintptr_t)addr);
        return 0;

    case PTRACE_POKEDATA:
        *(uint32_t*)(p->memory + (uintptr_t)addr) =
            *(uint32_t*)data;
        return 0;
    }

    return -EINVAL;
}
