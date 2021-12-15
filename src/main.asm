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
extern memcmp
extern atoi
extern sprintf
extern fopen
extern fclose
extern rewind
extern fprintf
extern fscanf
extern exit


section .data

input_str:
    db "input: ", 0
fmt_d10:
    db "%d", 10, 0
fmt_x10:
    db "%x", 10, 0
fmt_sym:
    db "str: %s, addr: %d", 10, 0
fmt_objhdr:
    db "%c%s%s%s", 10, 0
fmt_objend:
    db "%c%s", 10, 0
fmt_objtxt:
    db "%c%s%s%s", 10, 0
fmt_objmdfy:
    db "%c%s%s", 10, 0
delim:
    db 9, 0x20, 0 ; str of tab
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

    mov rax, argv
    mov rdi, qword [rax + 8 * 2]
    mov rsi, mode_w
    call fopen
    mov qword [output_file], rax

    call stg1

    mov rdi, qword [input_file]
    call rewind

    call stg2

    mov rdi, qword [input_file]
    call fclose

    mov rdi, qword [output_file]
    call fclose

    leave
    ret

stg1:
    %define idx     qword [rbp - 0x8]
    %define tok     qword [rbp - 0x10]
    %define ins_id  qword [rbp - 0x18]
    %define tmp     qword [rbp - 0x20]
    %define var     qword [rbp - 0x28]
    %define is_b    qword [rbp - 0x30]
    %define buf     [rbp - 0x70]

    push rbp
    mov rbp, rsp
    sub rsp, 0x70
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

    ; call print_sym

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
    xor rdi, rdi
    mov rsi, delim
    call strtok
    mov tmp, rax

    mov rdi, tmp
    mov rsi, str_X
    mov rdx, 2
    call strncmp
    test rax, rax
    jne .byte_char
    mov is_b, 0
.byte_cnt:
    mov rdi, tmp
    add rdi, 2
    mov var, rdi
    mov rsi, 0x27
    call strchr
    test rax, rax
    mov rsi, __LINE__
    je err
    mov byte [rax], 0
    mov rdi, var
    call strlen
    jmp .check_b
.byte_char:
    mov rdi, tmp
    mov rsi, str_C
    mov rdx, 2
    call strncmp
    test rax, rax
    mov rsi, __LINE__
    jne err
    mov is_b, 1
    jmp .byte_cnt

.check_b:
    mov rdi, is_b
    test rdi, rdi
    jne .set_addrcnt
    shr rax, 1
.set_addrcnt:
    add qword [addrcnt], rax
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
    %define tmp     [rbp - 0x30]
    %define buf     [rbp - 0x90]

    push rbp
    mov rbp, rsp
    sub rsp, 0x90
    mov qword [pc], 0
    mov qword [b], 0
    mov qword [txtcnt], 0
    mov qword [txtrcrdcnt], 0
    mov qword [txtpc], 0
    mov qword [mdfyrcrdcnt], 0

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
    mov dl, byte [rdi + rax]
    cmp dl, 10
    jne .no_strip
    mov byte [rdi + rax], 0

    ; lea rdi, buf
    ; call puts wrt ..plt
.no_strip:
    mov idx, 0
    lea rdi, buf
    mov rsi, delim
    call strtok
    jmp .loop7
.loop6:
    lea rdi, tmp
    mov rsi, idx
    mov qword [rdi + rsi * 8], rax
    add idx, 0x1
    xor rdi, rdi
    mov rsi, delim
    call strtok
.loop7:
    test rax, rax
    jne .loop6

    lea rdi, tmp
    cmp idx, 0x3
    jne .over_sym
    mov rax, qword [rdi]
    mov label, rax
    mov rax, qword [rdi + 8]
    mov ins, rax
    mov rax, qword [rdi + 8*2]
    mov var, rax
    jmp .chk_ins_good
.over_sym:
    cmp idx, 0x2
    jne .no_var
    mov rax, qword [rdi + 8]
    mov var, rax
.no_var:
    mov rax, qword [rdi]
    mov ins, rax
.chk_ins_good:
    mov rdi, ins
    call chk_ins
    cmp rax, 0xffffffffffffffff
    mov rsi, __LINE__
    je err

    mov ins_id, rax

    ; mov rsi, ins_id
    ; mov rdi, fmt_d10
    ; call printf

.switch_ins:
    mov rdx, ins_id
    mov rax, jmptable
    jmp [rax + rdx * 8]

.LBYTE:
    mov rdi, var
    call set_txt_byte
    jmp .loop
.LWORD:
.LRESB:
    mov rdi, var
    mov rsi, 1
    call set_txt_res
    jmp .loop
.LRESW:
    mov rdi, var
    mov rsi, 3
    call set_txt_res
    jmp .loop
.LSTART:
    mov rdi, label
    mov rsi, ins
    mov rdx, var
    mov rcx, qword [addrcnt]
    call set_hdrrcrd
    jmp .loop
.LEND:
    mov rdi, ins
    mov rsi, var
    call set_endrcrd
    jmp .end
.LBASE:
    mov rdi, var
    mov al, byte [rdi]
    cmp al, 0x30
    jl .not_num
    cmp al, 0x39
    jg .not_num
    call atoi
    jmp .set_base
.not_num:
    call chk_sym
    mov rdi, symtab
    shl rax, 5 ; idx * symsz
    mov rax, [rdi + rax + sSym.addr]
.set_base:
    mov qword [b], rax
    jmp .loop
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
.LCOMP:
.LTD:
.LRD:
.LWD:
.LTIX:
.LPLDT:
    mov rdi, ins
    mov rsi, var
    call set_txt_fmt34
    jmp .loop
.LCLEAR:
.LCOMPR:
.LTIXR:
    mov rdi, ins
    mov rsi, var
    call set_txt_fmt2
    jmp .loop
.LJSUB:
.LPJSUB:
    mov rdi, ins
    mov rsi, var
    call set_txt_fmt34
    mov rdi, ins
    call set_mdfyrcrd
    jmp .loop
.LRSUB:
    mov rdi, ins
    call set_txt_rsub
    jmp .loop

.end:
    mov rax, hdrrcrd
    movzx eax, byte [rax + sHdrRcrd.head]
    movsx eax, al
    mov edx, eax
    mov rax, hdrrcrd
    lea rcx, [rax + sHdrRcrd.name]
    lea r8, [rax + sHdrRcrd.saddr]
    lea r9, [rax + sHdrRcrd.len]
    mov rsi, fmt_objhdr
    mov rdi, qword [output_file]
    call fprintf

    mov idx, 0
    jmp .loop4
.loop3:
    mov rax, txtrcrd
    mov rdi, idx
    shl rdi, 7
    mov cl, byte [rax + rdi + sTxtRcrd.head]
    movsx ecx, cl
    mov edx, ecx
    lea rcx, [rax + rdi + sTxtRcrd.saddr]
    lea r8, [rax + rdi + sTxtRcrd.len]
    lea r9, [rax + rdi + sTxtRcrd.code]
    mov rsi, fmt_objtxt
    mov rdi, qword [output_file]
    call fprintf
    add idx, 1
.loop4:
    mov rax, idx
    cmp rax, qword [txtrcrdcnt]
    jl .loop3

    mov idx, 0
    jmp .loop9
.loop8:
    mov rax, mdfyrcrd
    mov rdi, idx
    shl rdi, 4
    mov cl, byte [rax + rdi + sMdfyRcrd.head]
    movsx ecx, cl
    mov edx, ecx
    lea rcx, [rax + rdi + sMdfyRcrd.sloc]
    lea r8, [rax + rdi + sMdfyRcrd.len]
    mov rsi, fmt_objmdfy
    mov rdi, qword [output_file]
    call fprintf
    add idx, 1
.loop9:
    mov rax, idx
    cmp rax, qword [mdfyrcrdcnt]
    jl .loop8

    mov rax, endrcrd
    movzx eax, byte [rax + sEndRcrd.head]
    movsx eax, al
    mov edx, eax
    mov rax, endrcrd
    lea rcx, [rax + sEndRcrd.saddr]
    mov rsi, fmt_objend
    mov rdi, qword [output_file]
    call fprintf
.leave:
    leave
    ret

err:
    mov rdi, str_err
    call printf
    mov rdi, 0
    call exit