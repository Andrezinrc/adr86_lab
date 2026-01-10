[BITS 32]
[ORG 0x0000]

start:
    xor al, al
    mov [0x540], al

    mov eax, 4
    mov ebx, 1
    mov ecx, banner
    mov edx, banner_len
    int 0x80

    mov eax, 4
    mov ebx, 1
    mov ecx, 0x500
    mov edx, 11
    int 0x80

ler:
    mov eax, 3
    mov ebx, 0
    mov ecx, 0x520
    mov edx, 1
    int 0x80

    mov al, [0x520]

    sub al, 'i'
    jz is_i
    add al, 'i'

    sub al, 'l'
    jz is_l
    add al, 'l'

    sub al, 'o'
    jz is_o
    add al, 'o'

    sub al, 'v'
    jz is_v
    add al, 'v'

    sub al, 'e'
    jz is_e
    add al, 'e'

    sub al, 'c'
    jz is_c

    jmp erro

is_i:
    mov al, 'i'
    mov [0x500], al
    jmp atualiza

is_l:
    mov al, 'l'
    mov [0x502], al
    jmp atualiza

is_o:
    mov al, 'o'
    mov [0x504], al
    jmp atualiza

is_v:
    mov al, 'v'
    mov [0x506], al
    jmp atualiza

is_e:
    mov al, 'e'
    mov [0x508], al
    jmp atualiza

is_c:
    mov al, 'c'
    mov [0x50A], al
    jmp atualiza

erro:
    mov al, [0x540]
    inc al
    mov [0x540], al

    mov eax, 4
    mov ebx, 1
    mov ecx, 0x50C
    mov edx, 6
    int 0x80

    mov al, [0x540]
    cmp al, 1
    je forca1
    cmp al, 2
    je forca2
    cmp al, 3
    je perdeu

    jmp ler

forca1:
    mov eax, 4
    mov ebx, 1
    mov ecx, f1
    mov edx, f1_len
    int 0x80
    jmp ler

forca2:
    mov eax, 4
    mov ebx, 1
    mov ecx, f2
    mov edx, f2_len
    int 0x80
    jmp ler

perdeu:
    mov eax, 4
    mov ebx, 1
    mov ecx, f3
    mov edx, f3_len
    int 0x80

    mov eax, 1
    int 0x80

atualiza:
    mov eax, 4
    mov ebx, 1
    mov ecx, 0x500
    mov edx, 11
    int 0x80

    mov al, [0x500]
    sub al, 'i'
    jnz ler
    mov al, [0x502]
    sub al, 'l'
    jnz ler
    mov al, [0x504]
    sub al, 'o'
    jnz ler
    mov al, [0x506]
    sub al, 'v'
    jnz ler
    mov al, [0x508]
    sub al, 'e'
    jnz ler
    mov al, [0x50A]
    sub al, 'c'
    jnz ler

    mov eax, 4
    mov ebx, 1
    mov ecx, 0x512
    mov edx, 7
    int 0x80

    mov eax, 1
    int 0x80

times 0x500-($-$$) db 0
db '_', ' ', '_', ' ', '_', ' ', '_', ' ', '_', ' ', '_', 0x0A
db 0
db 'Errou', 0x0A
db 'GANHOU', 0x0A

banner:
db "=== JOGO DA FORCA ===", 0x0A, 0x0A
banner_len equ $-banner

f1:
db " O",0x0A,0x0A
f1_len equ $-f1

f2:
db " O",0x0A," |",0x0A,0x0A
f2_len equ $-f2

f3:
db " O",0x0A,"/|\",0x0A,"/ \",0x0A
f3_len equ $-f3
