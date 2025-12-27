#include "kernel.h"
#include "../mem.h"
#include <stdio.h>

static int kernel_initialized = 0;

void kernel_init(void) {
    if (kernel_initialized) return;
    
    KDEBUG("Inicializado\n");
    kernel_initialized = 1;
}

static void handle_write(struct CPU *cpu, uint8_t *memory) {
    if (cpu->ebx.e != 1) return (void)(cpu->eax.e = -1);
    
    uint32_t start = cpu->ecx.e;
    uint32_t count = cpu->edx.e;
 
    if (start >= MEM_SIZE || start + count > MEM_SIZE) {
        cpu->eax.e = -1;
        return;
    }

    printf("\033[1;37m");
    for (uint32_t i=0;i<cpu->edx.e;i++) {
        putchar(memory[cpu->ecx.e + i]);
    }
    printf("\033[0m\n");
    
    cpu->eax.e = count;
}

static void handle_read(struct CPU *cpu, uint8_t *memory, struct fake_process *proc) {
    int fd = cpu->ebx.e;
    char *buf = (char*)&memory[cpu->ecx.e];
    size_t max_count = cpu->edx.e;
    
    if (fd != 0) {
        cpu->eax.e = -1;
        return;
    }
    
    printf("\033[1;33m> \033[0m");
    fflush(stdout);
    
    size_t bytes_read = 0;
    int c;

    while(bytes_read < max_count) {
        c = getchar();
        
        if(c==EOF) {
            break;
        }
        
        buf[bytes_read++] = (char)c;
        
        if(c=='\n') {
            break;
        }
    }
    
    cpu->eax.e = (uint32_t)bytes_read;
    
    KDEBUG("\033[90mread() = %zu bytes: \"", bytes_read);
}

static void handle_exit(struct CPU *cpu, uint8_t *memory) {
    printf("Processo terminou %d\n", cpu->ebx.e);
    memory[0] = 0xFF;
}


void kernel_handle_syscall(struct fake_process *proc) {
    if (!kernel_initialized) kernel_init();
	
    struct CPU *cpu = &proc->cpu;
    uint8_t *memory = proc->memory;
    int syscall_num = cpu->eax.e;
    
    KDEBUG("Syscall %d em EIP=0x%08X\n", syscall_num, cpu->eip);
    
    switch (syscall_num) {
        case SYS_EXIT:
            handle_exit(cpu, memory);
            break;            
        case SYS_WRITE:
            handle_write(cpu, memory);
            break;
		 case SYS_READ:
		     handle_read(cpu, memory, proc);
            break;
        case SYS_GETPID:
            cpu->eax.e = proc->pid;
            break;
        default:
            KDEBUG("Syscall %d não implementada\n", syscall_num);
            cpu->eax.e = -1;
            break;
    }
}
