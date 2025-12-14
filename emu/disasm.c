#include "disasm.h"
#include "memory.h"
#include <stdio.h>



void disassemble(uint8_t *memory, uint32_t eip) {
    uint8_t op = memory[eip];
    printf("0x%08X: ", eip);

    switch (op) {
        case 0xB8: printf("mov eax, 0x%08X\n",
                          mem_read32(memory, eip+1)); break;
        case 0xB9: printf("mov ecx, 0x%08X\n",
                          mem_read32(memory, eip+1)); break;
        case 0x39: printf("cmp eax, ecx\n"); break;
        case 0x40: printf("inc eax\n"); break;
        case 0x48: printf("dec eax\n"); break;
        case 0x01: printf("add eax, ecx\n"); break;
        case 0x29: printf("sub eax, ecx\n"); break;
        case 0x05: printf("add eax, 0x%08X\n",
                          mem_read32(memory, eip+1)); break;
        case 0x89: {
            uint8_t modrm = memory[eip+1];
            if(modrm == 0xC1) printf("mov ecx, eax\n");
            break;
        }
         case 0x31: {
            uint8_t modrm = memory[eip+1];
            if (modrm == 0xC0) printf("xor eax, eax\n");
            else if (modrm == 0xC9) printf("xor ecx, ecx\n");
            else printf("xor ??? (modrm=%02X)\n", modrm);
            break;
        }

        case 0x83: {
            uint8_t modrm = memory[eip+1];
            int8_t imm = memory[eip+2];

            if (modrm == 0xC0)
                printf("add eax, %d\n", imm);
            else if (modrm == 0xE8)
                printf("sub eax, %d\n", imm);
            else if (modrm == 0xF8)
                printf("cmp eax, %d\n", imm);
            else
                printf("83 /r op (modrm=%02X, imm=%d)\n", modrm, imm);

            break;
        }

        case 0x74: {
            int8_t rel = memory[eip+1];
            printf("74 %02X          je 0x%08X\n", 
                   rel & 0xFF, 
                   eip + 2 + rel);
            break;
        }

        case 0x75: {
            int8_t rel = memory[eip+1];
            printf("jne %d\n", rel);
            break;
        }
        
        case 0xEB: {
            int8_t rel = memory[eip+1];
            printf("jmp %d\n", rel);
            break;
        } 
        
        case 0xCC: printf("int3\n"); break;
        case 0x90: printf("nop\n"); break;
        case 0xF8: printf("clc\n"); break;
        case 0xF4: printf("hlt\n"); break;
        default: printf("db 0x%02X\n", op);
    }
}


