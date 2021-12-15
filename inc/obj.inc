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
    .head resb 4
    .len resb 4
    .sloc resb 8
endstruc

section .bss

txtcnt          resq 1
txtpc           resq 1
txtrcrdcnt      resq 1
hdrrcrd         resb sHdrRcrd_size
txtrcrd         resb sTxtRcrd_size * 100
mdfyrcrdcnt     resq 1
mdfyrcrd        resb sMdfyRcrd_size * 20
endrcrd         resb sEndRcrd_size
pc              resq 1
b               resq 1
is_res          resq 1


section .data

fmt_06x:        db "%06x", 0
fmt_03x:        db "%03x", 0
fmt_05x:        db "%05x", 0
fmt_02x:        db "%02x", 0
fmt_04x:        db "%04x", 0
fmt_x:          db "%x", 0
str_#:          db "#", 0
str_@:          db "@", 0
str_X:          db "X'", 0
str_C:          db "C'", 0
reg_A:          db "A", 0
reg_X:          db "X", 0
reg_S:          db "S", 0
reg_T:          db "T", 0
reg_AS:          db "A,S", 0

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
    mov qword [pc], rax
    mov qword [b], rax
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

    mov qword [is_res], 0
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

    mov rdi, 0
    call add_txtrcrd

    mov qword [is_res], 0
    leave
    ret

set_txt_byte:
    %define var     qword [rbp - 0x8]
    %define tmp     qword [rbp - 0x10]
    %define len     qword [rbp - 0x18]
    %define addr    qword [rbp - 0x20]
    %define idx     qword [rbp - 0x28]
    %define buf2    [rbp - 0x50]
    %define buf     [rbp - 0x70]
    push rbp
    mov rbp, rsp
    sub rsp, 0x70
    mov var, rdi
    
    mov rdi, var
    mov rsi, str_X
    mov rdx, 2
    call strncmp
    test rax, rax
    jne .set_char
    mov rdi, var
    add rdi, 2
    mov addr, rdi
    mov rsi, 0x27
    call strchr
    test rax, rax
    mov rsi, __LINE__
    je err
    mov byte [rax], 0
    mov rdi, addr
    call strlen
    mov len, rax
    jmp .set_byte

.set_char:
    mov rdi, var
    mov rsi, str_C
    mov rdx, 2
    call strncmp
    test rax, rax
    mov rsi, __LINE__
    jne err
    mov rdi, var
    add rdi, 2
    mov addr, rdi
    mov rsi, 0x27
    call strchr
    test rax, rax
    mov rsi, __LINE__
    je err
    mov byte [rax], 0
    mov idx, 0
    jmp .loop2
.loop:
    mov rax, idx
    mov rdi, addr
    mov dl, byte [rdi + rax]
    and rdx, 0xff
    lea rdi, buf2
    mov rsi, idx
    lea rdi, [rdi + rsi * 2]
    mov rsi, fmt_x
    call sprintf
    add idx, 0x1
.loop2:
    mov rax, idx
    mov rdi, addr
    mov al, byte [rdi + rax]
    test rax, rax
    jne .loop
    lea rax, buf2
    mov addr, rax
    lea rdi, buf2
    call strlen
    mov len, rax

.set_byte:
    mov rsi, addr
    lea rdi, buf
    call strcpy
    mov rdx, len
    mov rax, txtrcrd
    mov rdi, qword [txtrcrdcnt]
    shl rdi, 7
    lea rdi, [rax + rdi + sTxtRcrd.code]
    add rdi, qword [txtpc]
    lea rsi, buf
    call memcpy

    mov rax, len
    shr rax, 1
    add qword [pc], rax

    mov rax, len
    mov rdx, qword [txtpc]
    add rax, rdx
    cmp rax, 0x3c
    jge .add_txtrcrdcnt
    mov qword [txtpc], rax
    jmp .end
.add_txtrcrdcnt:
    mov rdi, len
    call add_txtrcrd

.end:
    mov qword [is_res], 0
    leave
    ret

set_txt_res:
    %define var     qword [rbp - 0x8]
    %define size    qword [rbp - 0x10]
    %define len     qword [rbp - 0x18]
    %define addr    qword [rbp - 0x20]
    %define idx     qword [rbp - 0x28]
    %define buf2    [rbp - 0x50]
    %define buf     [rbp - 0x70]
    push rbp
    mov rbp, rsp
    sub rsp, 0x70
    mov var, rdi
    mov size, rsi

    mov rdi, var
    call atoi
    mov rsi, size
    mul rsi
    mov len, rax

    mov rax, qword [is_res]
    test rax, rax
    jne .add_pc
    mov rdi, 0
    call add_txtrcrd
.add_pc:
    mov rax, len
    add qword [pc], rax
    mov qword [is_res], 1

    leave
    ret

set_txt_rsub:
    %define ins     qword [rbp - 0x8]
    %define code    qword [rbp - 0x10]
    %define len     qword [rbp - 0x18]
    %define ins_id  qword [rbp - 0x20]
    %define buf     [rbp - 0x30]
    push rbp
    mov rbp, rsp
    sub rsp, 0x30
    mov ins, rdi
    
    mov rdi, ins
    call chk_ins
    mov ins_id, rax
    cmp rax, 0xffffffffffffffff
    je err

    mov rdi, ins_id
    call get_ins_code
    add rax, 0x3
    mov code, rax

    mov rdi, ins_id
    call get_ins_len
    mov len, rax

    mov rax, len
    add qword [pc], rax
    add qword [txtcnt], 0x1
    mov rax, qword [txtpc]
    mov rdi, len
    add rax, rdi
    add rax, rdi
    cmp rax, 0x3c
    jge .add_txtrcrdcnt
    mov qword [txtpc], rax
    jmp .txt_content
.add_txtrcrdcnt:
    mov rdi, len
    add rdi, rdi
    call add_txtrcrd

.txt_content:
    lea rdi, buf
    mov rsi, fmt_02x
    mov rdx, code
    call sprintf
    mov rax, txtrcrd
    mov rdi, qword [txtrcrdcnt]
    shl rdi, 7
    lea rdi, [rax + rdi + sTxtRcrd.code]
    add rdi, qword [txtpc]
    mov rsi, len
    sub rdi, rsi
    sub rdi, rsi
    lea rsi, buf
    mov rdx, 2
    call memcpy

    lea rdi, buf
    mov rsi, fmt_04x
    mov rdx, 0
    call sprintf
    mov rax, txtrcrd
    mov rdi, qword [txtrcrdcnt]
    shl rdi, 7
    lea rdi, [rax + rdi + sTxtRcrd.code]
    add rdi, qword [txtpc]
    mov rsi, len
    sub rdi, rsi
    sub rdi, rsi
    add rdi, 2
    lea rsi, buf
    mov rdx, 4
    call memcpy

    leave
    ret

set_txt_fmt2:
    %define ins     qword [rbp - 0x8]
    %define var     qword [rbp - 0x10]
    %define reg     qword [rbp - 0x18]
    %define len     qword [rbp - 0x20]
    %define code    qword [rbp - 0x28]
    %define ins_id  qword [rbp - 0x30]
    %define tmp     qword [rbp - 0x38]
    %define tmp2    qword [rbp - 0x40]
    %define buf     [rbp - 0x60]

    push rbp
    mov rbp, rsp
    sub rsp, 0x60
    mov ins, rdi
    mov var, rsi

    mov rdi, ins
    call chk_ins
    mov ins_id, rax
    cmp rax, 0xffffffffffffffff
    je err

    mov rdi, ins_id
    call get_ins_code
    mov code, rax

    mov rdi, ins_id
    call get_ins_len
    mov len, rax

    mov rax, len
    add qword [pc], rax
    add qword [txtcnt], 0x1
    mov rax, qword [txtpc]
    mov rdi, len
    add rax, rdi
    add rax, rdi
    cmp rax, 0x3c
    jge .add_txtrcrdcnt
    mov qword [txtpc], rax
    jmp .txt_content
.add_txtrcrdcnt:
    mov rdi, len
    add rdi, rdi
    call add_txtrcrd

.txt_content:
    mov rdi, var
    mov rsi, reg_AS
    mov rdx, 0x2
    call strncmp
    test rax, rax
    jne .not_reg_AS
    mov reg, 0x4
    jmp .set_rcrd
.not_reg_AS:
    mov rdi, var
    mov rsi, reg_A
    mov rdx, 0x3
    call strncmp
    test rax, rax
    jne .not_reg_A
    mov reg, 0x0
    jmp .set_rcrd
.not_reg_A:
    mov rdi, var
    mov rsi, reg_X
    mov rdx, 0x1
    call strncmp
    test rax, rax
    jne .not_reg_X
    mov reg, 0x10
    jmp .set_rcrd
.not_reg_X:
    mov rdi, var
    mov rsi, reg_S
    mov rdx, 0x1
    call strncmp
    test rax, rax
    jne .not_reg_S
    mov reg, 0x40
    jmp .set_rcrd
.not_reg_S:
    mov rdi, var
    mov rsi, reg_T
    mov rdx, 0x1
    call strncmp
    test rax, rax
    jne .not_reg_T
    mov reg, 0x50
    jmp .set_rcrd
.not_reg_T:
    mov rsi, __LINE__
    jmp err

.set_rcrd:
    lea rdi, buf
    mov rsi, fmt_02x
    mov rdx, code
    call sprintf
    mov rax, txtrcrd
    mov rdi, qword [txtrcrdcnt]
    shl rdi, 7
    lea rdi, [rax + rdi + sTxtRcrd.code]
    add rdi, qword [txtpc]
    mov rsi, len
    sub rdi, rsi
    sub rdi, rsi
    lea rsi, buf
    mov rdx, 2
    call memcpy

    lea rdi, buf
    mov rsi, fmt_02x
    mov rdx, reg
    call sprintf
    mov rax, txtrcrd
    mov rdi, qword [txtrcrdcnt]
    shl rdi, 7
    lea rdi, [rax + rdi + sTxtRcrd.code]
    add rdi, qword [txtpc]
    mov rsi, len
    sub rdi, rsi
    sub rdi, rsi
    add rdi, 2
    lea rsi, buf
    mov rdx, len
    call memcpy

    leave
    ret

set_txt_fmt34:
    %define ins     qword [rbp - 0x8]
    %define var     qword [rbp - 0x10]
    %define addr    qword [rbp - 0x18]
    %define len     qword [rbp - 0x20]
    %define code    qword [rbp - 0x28]
    %define ins_id  qword [rbp - 0x30]
    %define tmp     qword [rbp - 0x38]
    %define tmp2    qword [rbp - 0x40]
    %define buf     [rbp - 0x60]

    push rbp
    mov rbp, rsp
    sub rsp, 0x60
    mov ins, rdi
    mov var, rsi
    
    mov rdi, ins
    call chk_ins
    mov ins_id, rax
    cmp rax, 0xffffffffffffffff
    je err

    mov rdi, ins_id
    call get_ins_code
    mov code, rax

    mov rdi, ins_id
    call get_ins_len
    mov len, rax

    mov rax, len
    add qword [pc], rax
    add qword [txtcnt], 0x1
    mov rax, qword [txtpc]
    mov rdi, len
    add rax, rdi
    add rax, rdi
    cmp rax, 0x3c
    jge .add_txtrcrdcnt
    mov qword [txtpc], rax
    jmp .txt_content
.add_txtrcrdcnt:
    mov rdi, len
    add rdi, rdi
    call add_txtrcrd

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
    inc rax
    inc rax
    inc rax
    shl rax, 0x4
    mov code, rax

    mov rdi, var
    mov rax, var
    mov al, byte [rax]

.set_addr:
    mov tmp, rdi
    mov tmp2, rax
    mov rsi, 0x2c ; ','
    call strchr
    test rax, rax
    je .not_x
    mov byte [rax], 0
    lea rdi, [rax+1]
    mov rsi, reg_X
    mov rdx, 2
    call strncmp
    test rax, rax
    mov rsi, __LINE__
    jne err
    mov rax, code
    and rax, 0x10
    test rax, rax
    mov rsi, __LINE__
    je err
    mov rax, code
    and rax, 0x20
    test rax, rax
    mov rsi, __LINE__
    je err
    add code, 8

.not_x:
    mov rdi, tmp
    mov rax, tmp2
    mov rsi, len
    cmp rsi, 0x4
    jne .is_not_fmt4

    cmp al, 0x30
    jl .not_num_4
    cmp al, 0x39
    jg .not_num_4
    call atoi
    cmp rax, 0x100000
    mov rsi, __LINE__
    jge err
    mov addr, rax
    jmp .set_rcrd
.not_num_4:
    call chk_sym
    mov rdi, symtab
    shl rax, 5 ; idx * symsz
    mov rax, [rdi + rax + sSym.addr]
    cmp rax, 0x100000
    mov rsi, __LINE__
    jge err
    mov addr, rax
    jmp .set_rcrd

.is_not_fmt4:
    cmp al, 0x30
    jl .not_num
    cmp al, 0x39
    jg .not_num
    call atoi
    cmp rax, 0x1000
    mov rsi, __LINE__
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
    ; mov rsi, qword [b]
    ; mov rdi, fmt_x10
    ; call printf
    
    mov rax, addr
    sub rax, qword [b]
    cmp rax, 0x1000
    mov rsi, __LINE__
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
    mov rsi, len
    sub rdi, rsi
    sub rdi, rsi
    lea rsi, buf
    mov rdx, 3
    call memcpy

    lea rdi, buf
    mov rsi, len
    cmp rsi, 0x4
    je .set_fmt_05x
    mov rsi, fmt_03x
    jmp .set_fmt_done
.set_fmt_05x:
    mov rsi, fmt_05x
.set_fmt_done:
    mov rdx, addr
    call sprintf
    mov rax, txtrcrd
    mov rdi, qword [txtrcrdcnt]
    shl rdi, 7
    lea rdi, [rax + rdi + sTxtRcrd.code]
    add rdi, qword [txtpc]
    mov rsi, len
    sub rdi, rsi
    sub rdi, rsi
    add rdi, 3
    lea rsi, buf
    mov rdx, len
    add rdx, rdx
    sub rdx, 3
    call memcpy

    mov qword [is_res], 0
    leave
    ret

add_txtrcrd:
    %define next_txtpc  qword [rbp - 0x8]
    %define buf         [rbp - 0x20]

    push rbp
    mov rbp, rsp
    sub rsp, 0x20
    mov next_txtpc, rdi

    mov rax, txtrcrd
    mov rdx, qword [txtrcrdcnt]
    shl rdx, 7
    lea rdi, [rax + rdx + sTxtRcrd.head]
    mov byte [rdi], 'T'
    
    mov rdx, qword [pc]
    mov rax, qword [txtpc]
    shr rax, 1
    sub rdx, rax
    mov rax, next_txtpc
    shr rax, 1
    sub rdx, rax
    mov rsi, fmt_06x
    lea rdi, buf
    call sprintf
    mov rax, txtrcrd
    mov rdx, qword [txtrcrdcnt]
    shl rdx, 7
    lea rdi, [rax + rdx + sTxtRcrd.saddr]
    lea rsi, buf
    mov rdx, 6
    call memcpy

    mov rdx, qword [txtpc]
    shr rdx, 1
    mov rsi, fmt_02x
    lea rdi, buf
    call sprintf
    mov rax, txtrcrd
    mov rdx, qword [txtrcrdcnt]
    shl rdx, 7
    lea rdi, [rax + rdx + sTxtRcrd.len]
    lea rsi, buf
    mov rdx, 2
    call memcpy

    add qword [txtrcrdcnt], 0x1
    mov rax, next_txtpc
    mov qword [txtpc], rax
    
    leave
    ret

set_mdfyrcrd:
    %define ins     qword [rbp - 0x8]
    %define len     qword [rbp - 0x10]
    %define ins_id  qword [rbp - 0x20]
    %define tmp     qword [rbp - 0x28]
    %define tmp2    qword [rbp - 0x30]
    %define buf     [rbp - 0x60]

    push rbp
    mov rbp, rsp
    sub rsp, 0x60
    mov ins, rdi
    
    mov rdi, ins
    call chk_ins
    mov ins_id, rax
    cmp rax, 0xffffffffffffffff
    je err

    mov rdi, ins_id
    call get_ins_len
    mov len, rax

    mov rax, mdfyrcrd
    mov rdi, qword [mdfyrcrdcnt]
    shl rdi, 4
    mov byte [rax + rdi + sMdfyRcrd.head], 'M'

    mov rdx, qword [pc]
    mov rax, len
    sub rdx, rax
    inc rdx
    mov rsi, fmt_06x
    lea rdi, buf
    call sprintf

    mov rax, mdfyrcrd
    mov rdi, qword [mdfyrcrdcnt]
    shl rdi, 4
    lea rdi, [rax + rdi + sMdfyRcrd.sloc]
    lea rsi, buf
    mov rdx, 0x6
    call memcpy

    mov rdx, len
    add rdx, rdx
    sub rdx, 0x3
    mov rsi, fmt_02x
    lea rdi, buf
    call sprintf

    mov rax, mdfyrcrd
    mov rdi, qword [mdfyrcrdcnt]
    shl rdi, 4
    lea rdi, [rax + rdi + sMdfyRcrd.len]
    lea rsi, buf
    mov rdx, 0x2
    call memcpy

    add qword [mdfyrcrdcnt], 0x1

    leave
    ret