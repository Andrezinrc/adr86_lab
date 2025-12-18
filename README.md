# adr86

Emulador educacional de CPU x86 em C.

O projeto implementa:
- Emulação de instruções x86 (32-bit)
- Sistema de memória virtual simplificado
- Kernel educacional com suporte a syscalls básicas
- ptrace fake para integração com debugger
- Debugger interativo

Compilar

```bash
gcc main.c emu/cpu.c emu/mem.c emu/dasm.c \
emu/ptrace-fake.c emu/dbg.c \
emu/decode.c emu/kernel/kernel.c -I emu -o emulator
```

Executar

```bash
./emulator
```

Sobre

Emula uma CPU x86 32-bit com sistema de memória e debugger integrado. Permite modificar registradores e memória durante a execução via comandos interativos.

O objetivo do projeto é estudar:
- Arquitetura x86
- Interação user-space <-> kernel-space
- Funcionamento de syscalls
- Fundamentos de debuggers e ptrace

Projeto para aprendizado de arquitetura de computadores e funcionamento de debuggers.
