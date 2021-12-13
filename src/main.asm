%include "inc/ins.inc"
%include "inc/sym.inc"
%include "inc/obj.inc"

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
extern strncpy
extern memset
extern memcpy
extern atoi
extern sprintf
extern fopen
extern fclose
extern rewind
extern fprintf
extern fscanf


section .data

input_str:
    db "input: ", 0
fmt_d10:
    db "%d", 10, 0
fmt_sym:
    db "str: %s, addr: %d", 10, 0
fmt_objhdr:
    db "hdr: %c%6s%6s%6s", 10, 0
fmt_objend:
    db "end: %c%6s", 10, 0
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
jmptable:
    dq stg2.LBYTE  ;  0
    dq stg2.LWORD  ;  1
    dq stg2.LRESB  ;  2
    dq stg2.LRESW  ;  3
    dq stg2.LSTART ;  4
    dq stg2.LEND   ;  5
    dq stg2.LBASE  ;  6
    dq stg2.LLDA   ;  7
    dq stg2.LLDB   ;  8
    dq stg2.LLDCH  ;  9
    dq stg2.LLDX   ; 10
    dq stg2.LLDT   ; 11
    dq stg2.LSTA   ; 12
    dq stg2.LSTCH  ; 13
    dq stg2.LSTX   ; 14
    dq stg2.LSTL   ; 15
    dq stg2.LJ     ; 16
    dq stg2.LJEQ   ; 17
    dq stg2.LJLT   ; 18
    dq stg2.LJSUB  ; 19
    dq stg2.LRSUB  ; 20
    dq stg2.LCOMP  ; 21
    dq stg2.LTD    ; 22
    dq stg2.LRD    ; 23
    dq stg2.LWD    ; 24
    dq stg2.LTIX   ; 25
    dq stg2.LCOMPR ; 26
    dq stg2.LTIXR  ; 27
    dq stg2.LCLEAR ; 28
    dq stg2.LPLDT  ; 29
    dq stg2.LPJSUB ; 30


section .bss
input_file:
    resq 1
output_file:
    resq 1
addrcnt:
    resq 1
addrcnt2:
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

    call stg2

    mov rdi, qword [input_file]
    call fclose

    ; mov rdi, qword [output_file]
    ; call fclose

    leave
    ret

stg1:
    %define idx     qword [rbp - 0x8]
    %define tok     qword [rbp - 0x10]
    %define ins_id  qword [rbp - 0x18]
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

    lea rdi, buf
    mov rsi, delim
    call strtok

    xor rdi, rdi
    mov rsi, delim
    call strtok

    xor rdi, rdi
    mov rsi, delim
    call strtok
    mov rdi, rax
    call atoi
    mov qword [addrcnt], rax

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

    mov ins_id, rax

    ; mov rsi, ins_id
    ; mov rdi, fmt_d10
    ; call printf

    mov rax, ins_id
    cmp rax, 0x5
    je .leave
.addbyte:
    mov rax, ins_id
    test rax, rax
    jne .addword
    add qword [addrcnt], 0x1
    jmp .loop

.addword:
    mov rax, ins_id
    cmp rax, 0x1
    jne .addnbyte
    add qword [addrcnt], 0x3
    jmp .loop

.addnbyte:
    mov rax, ins_id
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
    mov rax, ins_id
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
    mov rdi, ins_id
    call get_ins_len
    add qword [addrcnt], rax

    ; mov rsi, qword [addrcnt]
    ; mov rdi, fmt_d10
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
    %define idx     qword [rbp - 0x8]
    %define label   qword [rbp - 0x10]
    %define ins     qword [rbp - 0x18]
    %define var     qword [rbp - 0x20]
    %define ins_id  qword [rbp - 0x28]
    %define buf     [rbp - 0x70]

    push rbp
    mov rbp, rsp
    sub rsp, 0x70
    mov qword [addrcnt2], 0

.loop:
    ; mov rdi, input_str
    ; call printf wrt ..plt

    mov label, 0
    mov ins, 0
    mov var, 0

    lea rdi, buf
    mov rsi, 0x48
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

    lea rdi, buf
    cmp byte [rdi], 9
    je .over_sym
    
    mov label, rax

    xor rdi, rdi
    mov rsi, delim
    call strtok

.over_sym:
    ; mov rdi, tok
    ; call puts

    mov ins, rax

    mov rdi, ins
    call chk_ins
    cmp rax, 0xffffffffffffffff
    mov rsi, __LINE__
    je .err

    mov ins_id, rax

    ; mov rsi, ins_id
    ; mov rdi, fmt_d10
    ; call printf

    xor rdi, rdi
    mov rsi, delim
    call strtok
    mov var, rax

    mov rdx, ins_id
    mov rax, jmptable
    jmp [rax + rdx * 8]

.LBYTE:
.LWORD:
.LRESB:
.LRESW:
.LSTART:
    mov rdi, label
    mov rsi, ins
    mov rdx, var
    mov rcx, qword [addrcnt]
    call set_hdrrcrd
    jmp .loop
.LEND:
    mov rdi, ins
    mov rdx, var
    call set_endrcrd
    jmp .end
.LBASE:
.LLDA:
.LLDB:
.LLDCH:
.LLDX:
.LLDT:
.LSTA:
.LSTCH:
.LSTX:
.LSTL:
.LJ:
.LJEQ:
.LJLT:
.LJSUB:
.LRSUB:
.LCOMP:
.LTD:
.LRD:
.LWD:
.LTIX:
.LCOMPR:
.LTIXR:
.LCLEAR:
.LPLDT:
.LPJSUB:

.end:
    mov rax, hdrrcrd
    movzx eax, byte [rax + sHdrRcrd.head]
    movsx eax, al
    mov esi, eax
    mov rax, hdrrcrd
    lea rdx, [rax + sHdrRcrd.name]
    lea rcx, [rax + sHdrRcrd.saddr]
    lea r8, [rax + sHdrRcrd.len]
    mov rdi, fmt_objhdr
    call printf

    mov rax, endrcrd
    movzx eax, byte [rax + sEndRcrd.head]
    movsx eax, al
    mov esi, eax
    mov rax, endrcrd
    lea rdx, [rax + sEndRcrd.saddr]
    mov rdi, fmt_objend
    call printf
.leave:
    leave
    ret

.err:
    mov rdi, str_err
    call printf
    jmp .leave