%include "inc/ins.inc"
%include "inc/sym.inc"
global main

extern printf
extern scanf
extern puts
extern fgets
extern stdin
extern strlen
extern strncmp
extern strtok
extern strchr
extern strcpy

section .data

input_str:
    db "input: ", 0
fmt_str:
    db "%79s", 0
fmt_str2:
    db "%d", 10, 0
fmt_sym:
    db "str: %s, addr: %d", 10, 0
delim:
    db 9, 0 ; str of tab
str_ins:
    db "ins", 0
str_sym:
    db "sym", 0
str_err:
    db "err: %d", 10, 0


section .text

main:
    %define idx     qword [rbp - 0x8]
    %define tok     qword [rbp - 0x10]
    %define addrcnt qword [rbp - 0x18]
    %define inc_id  qword [rbp - 0x20]
    %define buf     [rbp - 0x60]

    push rbp
    mov rbp, rsp
    sub rsp, 0x60
    mov qword [symcnt], 0
    mov addrcnt, 0

.loop:
    mov rdi, input_str
    call printf wrt ..plt

    lea rdi, buf
    mov rsi, 0x50
    mov rdx, qword [stdin]
    call fgets wrt ..plt

    lea rdi, buf
    call strlen
    sub rax, 0x1
    lea rdi, buf
    mov byte [rdi + rax], 0

    lea rdi, buf
    call puts wrt ..plt

    lea rdi, buf
    mov rsi, delim
    call strtok
    mov tok, rax

    lea rdi, buf
    cmp byte [rdi], 9
    je .over_sym

    mov rdi, tok
    mov rsi, addrcnt
    call add_sym

    call print_sym

    xor rdi, rdi
    mov rsi, delim
    call strtok
    mov tok, rax

.over_sym:
    ; mov rdi, tok
    ; call puts

    mov rdi, tok
    call chk_ins
    cmp rax, 0xffffffffffffffff
    mov rsi, __LINE__
    je .err

    mov inc_id, rax

    ; mov rsi, inc_id
    ; mov rdi, fmt_str2
    ; call printf

    mov rdi, inc_id
    call get_inc_len
    add addrcnt, rax

    ; mov rsi, addrcnt
    ; mov rdi, fmt_str2
    ; call printf

    jmp .loop

.leave:
    leave
    ret

.err:
    mov rdi, str_err
    call printf
    jmp .leave
