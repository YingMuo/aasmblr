struc sSym
    .str    resb 0x18
    .addr   resq 1
endstruc

%define SYMSZ 0x10

section .bss

symcnt          resq 1
symtab          resb sSym_size * 100


section .text

print_sym:
    %define idx     qword [rbp - 0x8]

    push rbp
    mov rbp, rsp
    sub rsp, 0x10

    mov rsi, qword [symcnt]
    mov rdi, fmt_d10
    call printf

    mov rdi, str_sym
    call puts

    mov idx, 0
    jmp .loop2
.loop1:
    mov rax, idx
    shl rax, 5 ; idx * symsz
    lea rsi, [symtab + rax + sSym.str]
    mov rdx, [symtab + rax + sSym.addr]
    mov rdi, fmt_sym
    call printf
    add idx, 0x1

.loop2:
    mov rax, qword [symcnt]
    cmp idx, rax
    jl .loop1

    leave
    ret

add_sym:
    %define addr    qword [rbp - 0x10]
    %define str     qword [rbp - 0x8]

    push rbp
    mov rbp, rsp
    sub rsp, 0x10
    mov str, rdi
    mov addr, rsi

    mov rdi, str
    call chk_sym
    cmp rax, 0xffffffffffffffff
    jne .leave

    mov rax, qword [symcnt]
    shl rax, 5 ; idx * symsz
    mov rsi, addr
    mov qword [symtab + rax + sSym.addr], rsi
    lea rdi, [symtab + rax + sSym.str]
    mov rsi, str
    call strcpy

    add qword [symcnt], 0x1

.leave:
    leave
    ret

chk_sym:
    %define str     qword [rbp - 0x8]
    %define idx     qword [rbp - 0x10]

    push rbp
    mov rbp, rsp
    sub rsp, 0x10
    mov str, rdi

    mov idx, 0
    jmp .loop2
.loop1:
    mov rdx, 0x17
    mov rax, symtab
    mov rdi, idx
    shl rdi, 5 ; idx * symsz
    lea rsi, [rax + rdi + sSym.str]
    mov rdi, str
    call strncmp wrt ..plt

    test rax, rax
    je .leave
    add idx, 0x1
.loop2:
    mov rax, qword [symcnt]
    cmp idx, rax
    jl .loop1
    mov idx, 0xffffffffffffffff

.leave:
    mov rax, idx
    leave
    ret