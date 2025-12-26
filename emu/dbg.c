#include <stdio.h>
#include <string.h>
#include "dbg.h"
#include "ptrace-fake.h"

void dbg_prompt(char *buf, size_t size){
    printf("\033[1;32m(dbg)\033[0m ");
    fflush(stdout);
    fgets(buf, size, stdin);
    buf[strcspn(buf, "\n")] = 0;
}

void dbg_help(void){
    printf("\n=== Debugger Commands ===\n");
    printf("s          - Step (execute one instruction)\n");
    printf("c          - Continue execution\n");
    printf("q          - Quit debugger\n");
    printf("r reg val  - Write register value\n");
    printf("w addr val - Write memory\n");
    printf("b addr     - Set breakpoint\n");
    printf("d addr     - Delete breakpoint\n");
    printf("x addr len - Examine memory (hex + ASCII)\n");
    printf("h          - Show this help\n\n");
}

static void dbg_examine(uint32_t addr, uint32_t len) {
    uint32_t i;

    for (i=0;i<len;i+=16){
        printf("%08X: ", addr + i);

        uint32_t line_vals[16];
        int bytes_in_line = (len - i >= 16) ? 16 : (len - i);

        for (int j=0;j<bytes_in_line;j++) {
            uint32_t val;
            fake_ptrace(
                PTRACE_PEEKDATA,
                1337,
                (void*)(addr + i + j),
                &val
            );
            line_vals[j] = val & 0xFF;
            printf("%02X ", line_vals[j]);
        }

        /* Preenche espaço se a linha tiver menos de 16 bytes */
        for (int j=bytes_in_line;j<16;j++) {
            printf("   ");
        }

        printf(" |");
        for (int j=0;j<bytes_in_line;j++) {
            uint8_t c = line_vals[j];
            printf("%c", (c >= 32 && c <= 126) ? c : '.');
        }
        printf("|\n");
    }
}

void dbg_handle_cmd(struct Debugger *dbg, char *cmd, struct CPU *cpu, uint8_t *memory){
    if (cmd[0] == 'q') {
        dbg->running = -1;
        return;
    }

    if (cmd[0] == 'c') {
        dbg->running = 1;
        return;
    }

    if (cmd[0] == 's'){}
    if (cmd[0] == 'h') {
        dbg_help();
        return;
    }

    if (cmd[0] == 'b') {
        uint32_t addr;
        sscanf(cmd, "b %x", &addr);
        bp_set(addr, memory);
        return;
    }

    if (cmd[0] == 'd') {
        uint32_t addr;
        sscanf(cmd, "d %x", &addr);
        bp_clear(addr, memory);
        return;
    }

    if (cmd[0] == 'r') {
        char reg[8];
        int val;
        sscanf(cmd, "r %s %d", reg, &val);

        fake_ptrace(PTRACE_GETREGS, 1337, NULL, cpu);

        if (strcmp(reg, "eax") == 0) cpu->eax.e = val;
        else if (strcmp(reg, "ecx") == 0) cpu->ecx.e = val;

        fake_ptrace(PTRACE_SETREGS, 1337, NULL, cpu);
        return;
    }

    if (cmd[0] == 'w') {
        uint32_t addr, val;
        sscanf(cmd, "w %x %x", &addr, &val);
        fake_ptrace(PTRACE_POKEDATA, 1337, (void*)addr, &val);
        return;
    }
    
    if (cmd[0] == 'x') {
        uint32_t addr, len;
        if (sscanf(cmd, "x %x %u", &addr, &len) == 2) {
            dbg_examine(addr, len);
        } else {
            printf("Uso: x <addr> <len>\n");
        }
        return;
    }
}
