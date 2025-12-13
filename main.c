#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "cpu.h"
#include "memory.h"
#include "disasm.h"
#include "ptrace-fake.h"



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
    
    while(1){
        fake_ptrace(PTRACE_GETREGS, 1337, NULL, &cpu);
        
        if(memory[cpu.eip] == 0xB8) {
            uint32_t x = 10;
            fake_ptrace(PTRACE_POKEDATA, 1337, (void*)(cpu.eip + 1), &x);
        }
        
        fake_ptrace(PTRACE_SINGLESTEP, 1337, NULL, NULL);
        
        
         cpu_step(&cpu, memory);
         printf("EAX = %d\n", cpu.eax.e);
         disassemble(memory, cpu.eip);
         //print_state(&cpu);
         
         if(cpu.eip >= MEM_SIZE || memory[cpu.eip] == 0xF4) break; 
    }
    return 0;
}
