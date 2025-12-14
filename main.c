#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "cpu.h"
#include "memory.h"
#include "disasm.h"
#include "ptrace-fake.h"


/*
 * Debugger commands:
 * s = step       -> executa uma instrução
 * c = continue   -> executa até parar/encerrar
 * q = quit       -> sai do emulador
 * r <reg> <val>  -> escreve registrador
 * w <addr> <val> -> escreve memória
 * b <addr>       -> set breakpoint (INT3)
 * d <addr>       -> delete breakpoint
 */

void debug_prompt(char *buf, size_t size) {
    printf("(dbg) ");
    fflush(stdout);
    fgets(buf, size, stdin);
}

int main(){
    struct CPU cpu;
    uint8_t *memory = mem_create();
    cpu_init(&cpu, MEM_SIZE);
    
    
    static struct fake_process proc;
    proc.pid = 1337;
    proc.cpu = cpu;
    proc.memory = memory;
    proc.stopped = 0;
    fp_register(&proc);

    FILE *f = fopen("test.bin", "rb");
    if(!f) {
       printf("Erro ao abrir arquivo\n");
       return -1;
    }
    fseek(f, 0, SEEK_END);
    long t = ftell(f);
    rewind(f);

    fread(memory, 1, t, f);
    fclose(f);
 
 
    fake_ptrace(PTRACE_ATTACH, 1337, NULL, NULL);
    
    int running = 0;
    char cmd[64];
    
    while(1){
        fake_ptrace(PTRACE_GETREGS, 1337, NULL, &cpu);
        
        if (!running) {
            disassemble(memory, cpu.eip);
            printf("EAX = %d\n", cpu.eax.e);
            debug_prompt(cmd, sizeof(cmd));

            if (cmd[0] == 'q') break;
            if (cmd[0] == 'c'){ running = 1; continue; }
            if (cmd[0] == 's') { /* step */ }
            if (cmd[0] == 'b'){
                uint32_t addr;
                sscanf(cmd, "b %x", &addr);
                bp_set(addr, memory);
                continue;
            }
            if (cmd[0] == 'd'){
                uint32_t addr;
                sscanf(cmd, "d %x", &addr);
                bp_clear(addr, memory);
                continue;
            }
            if (cmd[0] == 'r') {
                char reg[8];
                int val;
                sscanf(cmd, "r %s %d", reg, &val);
                fake_ptrace(PTRACE_GETREGS, 1337, NULL, &cpu);
                if (strcmp(reg, "eax") == 0) cpu.eax.e = val;
                else if (strcmp(reg, "ecx") == 0) cpu.ecx.e = val;
                fake_ptrace(PTRACE_SETREGS, 1337, NULL, &cpu);
                continue;
           }
           if (cmd[0] == 'w') {
               uint32_t addr, val;
               sscanf(cmd, "w %x %x", &addr, &val);
               fake_ptrace(PTRACE_POKEDATA, 1337, (void*)addr, &val);
               continue;
           }
        }

        // Checa se hitou breakpoint
        int idx = bp_check(&cpu);
        if (idx >= 0) {
            printf("Breakpoint atingido em 0x%X\n", cpu.eip);
            bp_clear(cpu.eip, memory);
            running = 0;
        }
        
         fake_ptrace(PTRACE_SINGLESTEP, 1337, NULL, NULL);
         cpu_step(&cpu, memory);
         
         if(cpu.eip >= MEM_SIZE || memory[cpu.eip] == 0xF4) break; 
    }
    return 0;
}
