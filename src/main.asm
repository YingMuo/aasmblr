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
extern atoi
extern fopen
extern fclose
extern rewind
extern fprintf
extern fscanf


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
mode_r:
    db "r", 0
mode_w:
    db "w", 0


section .bss
input_file:
    resq 1
output_file:
    resq 1
addrcnt:
    resq 1


section .text

main:
    %define argv     qword [rbp - 0x10]
    %define argc     qword [rbp - 0x8]

    push rbp
    mov rbp, rsp
    sub rsp, 0x20
    mov argc, rdi
    mov argv, rsi

    mov rax, argv
    mov rdi, qword [rax + 8 * 1]
    mov rsi, mode_r
    call fopen
    mov qword [input_file], rax

    ; mov rax, argv
    ; mov rdi, qword [rax + 8 * 2]
    ; mov rsi, mode_w
    ; call fopen
    ; mov qword [output_file], rax

    call stg1

    mov rdi, qword [input_file]
    call rewind

    mov rdi, qword [input_file]
    call fclose

    ; mov rdi, qword [output_file]
    ; call fclose

    leave
    ret

stg1:
    %define idx     qword [rbp - 0x8]
    %define tok     qword [rbp - 0x10]
    %define inc_id  qword [rbp - 0x18]
    %define buf     [rbp - 0x60]

    push rbp
    mov rbp, rsp
    sub rsp, 0x60
    mov qword [symcnt], 0
    mov qword [addrcnt], 0

    lea rdi, buf
    mov rsi, 0x50
    mov rdx, qword [input_file]
    call fgets wrt ..plt

.loop:
    ; mov rdi, input_str
    ; call printf wrt ..plt

    lea rdi, buf
    mov rsi, 0x50
    mov rdx, qword [input_file]
    call fgets wrt ..plt

    lea rdi, buf
    call strlen
    sub rax, 0x1
    lea rdi, buf
    mov byte [rdi + rax], 0

    ; lea rdi, buf
    ; call puts wrt ..plt

    lea rdi, buf
    mov rsi, delim
    call strtok
    mov tok, rax

    lea rdi, buf
    cmp byte [rdi], 9
    je .over_sym

    mov rdi, tok
    mov rsi, qword [addrcnt]
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

    mov rax, inc_id
    cmp rax, 0x5
    je .leave
.addbyte:
    mov rax, inc_id
    test rax, rax
    jne .addword
    add qword [addrcnt], 0x1
    jmp .loop

.addword:
    mov rax, inc_id
    cmp rax, 0x1
    jne .addnbyte
    add qword [addrcnt], 0x3
    jmp .loop

.addnbyte:
    mov rax, inc_id
    cmp rax, 0x2
    jne .addnword
    xor rdi, rdi
    mov rsi, delim
    call strtok
    mov rdi, rax
    call atoi
    add qword [addrcnt], rax
    jmp .loop
    
.addnword:
    mov rax, inc_id
    cmp rax, 0x3
    jne .addlen
    xor rdi, rdi
    mov rsi, delim
    call strtok
    mov rdi, rax
    call atoi
    mov rcx, 3
    mul rcx
    add qword [addrcnt], rax
    jmp .loop

.addlen:
    mov rdi, inc_id
    call get_ins_len
    add qword [addrcnt], rax

    ; mov rsi, qword [addrcnt]
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

stg2:
