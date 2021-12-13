struc sHdrRcrd
    .head resb 1
    .name resb 8
    .saddr resb 8
    .len resb 8
endstruc

struc sTxtRcrd
    .head resb 1
    .saddr resb 8
    .len resb 3
    .code resb 60
endstruc

struc sEndRcrd
    .head resb 1
    .saddr resb 8
endstruc

struc sMdfyRcrd
    .head resb 1
    .sloc resb 8
    .len resb 8
endstruc

section .bss

txtcnt          resq 1
txtrcrdcnt      resq 1
hdrrcrd         resb sHdrRcrd_size
txtrcrd         resb sTxtRcrd_size * 100
mdfyrcrd        resb sMdfyRcrd_size * 20
endrcrd         resb sEndRcrd_size


section .data

fmt_06x:        db "%06x", 0


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