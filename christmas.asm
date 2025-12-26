BITS 32
org 0x0

section .data
    message db "FELIZ NATAL", 10, 10, 0
    msg_len equ $ - message - 1
    
    lines:
        db "    1", 10
        db "   10", 10
        db "  101", 10
        db " 1010", 10
        db "10101", 10
        db "    11", 10, 10
        db "FELIZ NATAL!", 10, 0
    
    lines_len equ $ - lines

section .text
global _start

_start:
    mov eax, 4
    mov ebx, 1
    mov ecx, message
    mov edx, msg_len
    int 0x80
    
    mov eax, 4
    mov ebx, 1
    mov ecx, lines
    mov edx, lines_len
    int 0x80
    
    mov eax, 1
    mov ebx, 0
    int 0x80
