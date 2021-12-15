struc sHdrRcrd
    .head resb 8
    .name resb 8
    .saddr resb 8
    .len resb 8
endstruc

struc sTxtRcrd
    .head resb 8
    .saddr resb 8
    .len resb 8
    .code resb 0x68
endstruc

struc sEndRcrd
    .head resb 8
    .saddr resb 8
endstruc

struc sMdfyRcrd
    .head resb 8
    .sloc resb 8
    .len resb 8
endstruc

section .bss

txtcnt          resq 1
txtpc           resq 1
txtrcrdcnt      resq 1
hdrrcrd         resb sHdrRcrd_size
txtrcrd         resb sTxtRcrd_size * 100
mdfyrcrd        resb sMdfyRcrd_size * 20
endrcrd         resb sEndRcrd_size
pc              resq 1
b               resq 1


section .data

fmt_06x:        db "%06x", 0
fmt_03x:        db "%03x", 0
fmt_05x:        db "%05x", 0
str_#:          db "#", 0
str_@:          db "@", 0
str_X:          db "X'", 0
str_C:          db "C'", 0
reg_A:          db "X", 0
reg_X:          db "X", 0
reg_S:          db "X", 0
reg_T:          db "X", 0
reg_AS:          db "X", 0

section .text

set_hdrrcrd:
    %define label   qword [rbp - 0x8]
    %define ins     qword [rbp - 0x10]
    %define var     qword [rbp - 0x18]
    %define len     qword [rbp - 0x20]
    %define buf     [rbp - 0x30]

    push rbp
    mov rbp, rsp
    sub rsp, 0x30
    mov label, rdi
    mov ins, rsi
    mov var, rdx
    mov len, rcx

    mov rax, hdrrcrd
    mov byte [rax + sHdrRcrd.head], "H"
    
    mov rdi, label
    call strlen
    mov rdx, rax
    mov rsi, label
    lea rdi, buf
    call strncpy

    mov rdi, label
    call strlen
    lea rdi, buf
    add rdi, rax
    mov rdx, 6
    sub rdx, rax
    mov rsi, 0x20
    call memset

    mov rax, hdrrcrd
    lea rdi, [rax + sHdrRcrd.name]
    lea rsi, buf
    mov rdx, 6
    call memcpy

    mov rdi, var
    call atoi
    mov rsi, fmt_06x
    mov rdx, rax
    lea rdi, buf
    call sprintf

    mov rax, hdrrcrd
    lea rdi, [rax + sHdrRcrd.saddr]
    lea rsi, buf
    mov rdx, 6
    call memcpy

    ; mov rax, hdrrcrd
    ; lea rdi, [rax + sHdrRcrd.saddr]
    ; call puts

    mov rdi, var
    call atoi
    mov rsi, fmt_06x
    mov rdx, len
    sub rdx, rax
    lea rdi, buf
    call sprintf

    mov rax, hdrrcrd
    lea rdi, [rax + sHdrRcrd.len]
    lea rsi, buf
    mov rdx, 6
    call memcpy

    ; mov rax, hdrrcrd
    ; lea rdi, [rax + sHdrRcrd.len]
    ; call puts

    leave
    ret

set_endrcrd:
    %define ins     qword [rbp - 0x8]
    %define var     qword [rbp - 0x10]
    %define idx     qword [rbp - 0x18]
    %define buf     [rbp - 0x30]

    push rbp
    mov rbp, rsp
    sub rsp, 0x30
    mov ins, rdi
    mov var, rsi

    mov rax, endrcrd
    mov byte [rax + sEndRcrd.head], "E"

    mov rdi, var
    call chk_sym
    cmp rax, 0xffffffffffffffff
    je err
    mov rdi, symtab
    shl rax, 5 ; idx * symsz
    mov rdx, [rdi + rax + sSym.addr]
    mov rsi, fmt_06x
    mov rax, endrcrd
    lea rdi, buf
    call sprintf

    mov rdx, 6
    lea rsi, buf
    mov rax, endrcrd
    lea rdi, [rax + sEndRcrd.saddr]
    call memcpy

    leave
    ret

set_txt_fmt3:
    %define ins     qword [rbp - 0x8]
    %define var     qword [rbp - 0x10]
    %define addr    qword [rbp - 0x18]
    %define len     qword [rbp - 0x20]
    %define code    qword [rbp - 0x28]
    %define ins_id  qword [rbp - 0x28]
    %define buf     [rbp - 0x40]

    push rbp
    mov rbp, rsp
    sub rsp, 0x40
    mov ins, rdi
    mov var, rsi

    mov rax, txtrcrd
    mov byte [rax + sTxtRcrd.head], "T"
    
    mov rdi, ins
    call chk_ins
    mov ins_id, rax
    cmp rax, 0xffffffffffffffff
    je err

    mov rdi, ins_id
    call get_ins_len
    mov len, rax

    mov rdi, ins_id
    call get_ins_code
    mov code, rax

    mov rax, len
    add qword [pc], rax
    add qword [txtcnt], 0x1
    mov rax, qword [txtpc]
    add rax, len
    cmp rax, 0x60
    jge .add_txtrcrdcnt
    mov qword [txtpc], rax
    jmp .txt_content
.add_txtrcrdcnt:
    add qword [txtrcrdcnt], 0x1
    mov rax, len
    mov qword [txtpc], rax

    mov rdi, ins_id
    call get_ins_code
    mov code, rax

.txt_content:
    mov rdi, var
    mov rsi, str_#
    mov rdx, 1
    call strncmp
    cmp rax, 0
    jne .b_not_imm

    mov rax, code
    inc rax
    shl rax, 0x4
    mov code, rax

    mov rdi, var
    add rdi, 0x1
    mov rax, var
    mov al, byte [rax + 1]
    jmp .set_addr
    
.b_not_imm:
    mov rdi, var
    mov rsi, str_@
    mov rdx, 1
    call strncmp
    cmp rax, 0
    jne .b_not_indirect

    mov rax, code
    inc rax
    inc rax
    shl rax, 0x4
    mov code, rax

    mov rdi, var
    add rdi, 0x1
    mov rax, var
    mov al, byte [rax + 1]
    jmp .set_addr

.b_not_indirect:
    mov rax, code
    shl rax, 0x4
    mov code, rax

    mov rdi, var
    mov rax, var
    mov al, byte [rax]

.set_addr:
    cmp al, 0x30
    jl .not_num
    cmp al, 0x39
    jg .not_num
    call atoi
    cmp rax, 0x1000
    jge err
    mov addr, rax
    jmp .set_rcrd

.not_num:
    call chk_sym
    mov rdi, symtab
    shl rax, 5 ; idx * symsz
    mov rax, [rdi + rax + sSym.addr]
    mov addr, rax
    sub rax, qword [pc]
    cmp rax, 0x7ff
    jg .nop_pc
    cmp rax, 0xfffffffffffff800
    jl .nop_pc
    and rax, 0xfff
    mov addr, rax
    add code, 2
    jmp .set_rcrd

.nop_pc:
    mov rax, addr
    sub rax, qword [b]
    cmp rax, 0x1000
    jge err
    mov addr, rax
    add code, 4
    jmp .set_rcrd

.set_rcrd:
    lea rdi, buf
    mov rsi, fmt_03x
    mov rdx, code
    call sprintf
    mov rax, txtrcrd
    mov rdi, qword [txtrcrdcnt]
    shl rdi, 7
    lea rdi, [rax + rdi + sTxtRcrd.code]
    add rdi, qword [txtpc]
    sub rdi, len
    lea rsi, buf
    mov rdx, 3
    call memcpy

    lea rdi, buf
    mov rsi, fmt_03x
    mov rdx, addr
    call sprintf
    mov rax, txtrcrd
    mov rdi, qword [txtrcrdcnt]
    shl rdi, 7
    lea rdi, [rax + rdi + sTxtRcrd.code]
    add rdi, qword [txtpc]
    sub rdi, len
    add rdi, 3
    lea rsi, buf
    mov rdx, 3
    call memcpy

    leave
    ret