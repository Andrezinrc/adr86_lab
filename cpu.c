#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"

void cpu_init(struct CPU *cpu) {
    cpu->eax.e = 0;
    cpu->ebx.e = 0;
    cpu->ecx.e = 0;
    cpu->edx.e = 0;
    cpu->esi.e = 0;
    cpu->edi.e = 0;
    cpu->ebp.e = 0;
    cpu->esp.e = MEM_SIZE - 4;
    cpu->eip = 0;
    cpu->flags.CF = cpu->flags.ZF = cpu->flags.SF = cpu->flags.OF = 0;
}

void cpu_step(struct CPU *cpu, uint8_t *memory) {
    uint8_t opcode = memory[cpu->eip];

    switch (opcode) {

        case 0xB8: { // MOV EAX, imm32
            uint32_t imm = *(uint32_t *)&memory[cpu->eip + 1];
            cpu->eax.e = imm;
            cpu->eip += 5;
            break;
        }

        case 0xB9: { // MOV ECX, imm32
            uint32_t imm = *(uint32_t *)&memory[cpu->eip + 1];
            cpu->ecx.e = imm;
            cpu->eip += 5;
            break;
        }
        
        case 0xF4: {
            printf("Encerrando.\n");
            exit(1);
        }
        
        
        default:
            printf("Opcode desconhecido em EIP=0x%08X: 0x%02X\n",
                   cpu->eip, opcode);
            exit(1);
    }
}

