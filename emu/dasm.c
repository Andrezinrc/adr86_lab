#include "dasm.h"
#include "decode.h"
#include "mem.h"
#include <stdio.h>

static const char *reg32_name(uint8_t reg) {
    static const char *names[8] = {
        "eax", "ecx", "edx", "ebx",
        "esp", "ebp", "esi", "edi"
    };
    return (reg < 8) ? names[reg] : "???";
}

void disassemble(uint8_t *memory, uint32_t eip) {
    uint8_t op = memory[eip];
    printf("0x%08X: ", eip);

    switch (op) {
        // MOV reg32, imm32
        case 0xB8: case 0xB9: case 0xBA: case 0xBB:
        case 0xBC: case 0xBD: case 0xBE: case 0xBF: {
            uint8_t reg = op - 0xB8;
            uint32_t imm = mem_read32(memory, eip + 1);
            printf("mov %s, 0x%08X\n", reg32_name(reg), imm);
            break;
        }

        // ADD r/m32, r32 / SUB r/m32, r32 / CMP r/m32, r32
        case 0x01: case 0x29: case 0x39: {
            const char *mnemonic = (op == 0x01) ? "add" : 
                                  (op == 0x29) ? "sub" : "cmp";
            
            uint8_t modrm = memory[eip + 1];
            uint8_t reg, rm;
            
            if (modrm_reg_reg(modrm, &reg, &rm)) {
                printf("%s %s, %s\n", mnemonic, reg32_name(rm), reg32_name(reg));
            } else {
                printf("%s mem, %s\n", mnemonic, reg32_name(reg));
            }
            break;
        }

        // XOR r/m32, r32
        case 0x31: {
            uint8_t modrm = memory[eip + 1];
            uint8_t reg, rm;
            
            if (modrm_reg_reg(modrm, &reg, &rm)) {
                printf("xor %s, %s\n", reg32_name(rm), reg32_name(reg));
            } else {
                printf("xor mem, %s\n", reg32_name(reg));
            }
            break;
        }

        // MOV r/m32, r32
        case 0x89: {
            uint8_t modrm = memory[eip + 1];
            uint8_t reg, rm;
            
            if (modrm_reg_reg(modrm, &reg, &rm)) {
                printf("mov %s, %s\n", reg32_name(rm), reg32_name(reg));
            } else {
                printf("mov mem, %s\n", reg32_name(reg));
            }
            break;
        }

        // MOV r32, r/m32
        case 0x8B: {
            uint8_t modrm = memory[eip + 1];
            uint8_t reg, rm;
            
            if (modrm_reg_reg(modrm, &reg, &rm)) {
                printf("mov %s, %s\n", reg32_name(reg), reg32_name(rm));
            } else {
                printf("mov %s, [mem]\n", reg32_name(reg));
            }
            break;
        }

        // ADD EAX, imm32
        case 0x05: {
            uint32_t imm = mem_read32(memory, eip + 1);
            printf("add eax, 0x%08X\n", imm);
            break;
        }

        // INC reg32 / DEC reg32
        case 0x40: case 0x41: case 0x42: case 0x43:
        case 0x44: case 0x45: case 0x46: case 0x47:
        case 0x48: case 0x49: case 0x4A: case 0x4B:
        case 0x4C: case 0x4D: case 0x4E: case 0x4F: {
            uint8_t reg = op - (op < 0x48 ? 0x40 : 0x48);
            const char *mnemonic = (op < 0x48) ? "inc" : "dec";
            printf("%s %s\n", mnemonic, reg32_name(reg));
            break;
        }

        // ADD/SUB/CMP r/m32, imm8
        case 0x83: {
            uint8_t modrm = memory[eip + 1];
            int8_t imm = (int8_t)memory[eip + 2];
            
            uint8_t subop = (modrm >> 3) & 7;
            
            const char *mnemonic = "???";
            switch (subop) {
                case 0: mnemonic = "add"; break;
                case 5: mnemonic = "sub"; break;
                case 7: mnemonic = "cmp"; break;
                default: break;
            }
            
			
            uint8_t reg, rm;
            if (modrm_reg_reg(modrm, &reg, &rm)) {
                printf("%s %s, %d\n", mnemonic, reg32_name(rm), imm);
            } else {
                printf("%s mem, %d\n", mnemonic, imm);
            }
            break;
        }


        case 0x74: {  // JE/JZ
            int8_t rel = (int8_t)memory[eip + 1];
            printf("je 0x%08X\n", eip + 2 + rel);
            break;
        }
        case 0x75: {  // JNE/JNZ
            int8_t rel = (int8_t)memory[eip + 1];
            printf("jne 0x%08X\n", eip + 2 + rel);
            break;
        }
        case 0xEB: {  // JMP curto
            int8_t rel = (int8_t)memory[eip + 1];
            printf("jmp 0x%08X\n", eip + 2 + rel);
            break;
        }

        case 0xCD: {
            uint8_t int_num = memory[eip + 1];
            printf("int 0x%02X\n", int_num);
            break;
        }

        case 0xCC: printf("int3\n"); break;
        case 0x90: printf("nop\n"); break;
        case 0xF8: printf("clc\n"); break;
        case 0xF4: printf("hlt\n"); break;
		
        default:
            printf("db 0x%02X\n", op);
            break;
    }
}
