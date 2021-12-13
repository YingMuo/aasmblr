struc sInc
    .str resb 8
    .len resq 1
endstruc

%define INSCNT 16


section .data

INS_ARR         dq INS_START
                dq INS_END
                dq INS_BYTE
                dq INS_WORD
                dq INS_RESB
                dq INS_RESW
                dq INS_LDA
                dq INS_LDB
                dq INS_LDX
                dq INS_LDT
                dq INS_STA
                dq INS_STX
                dq INS_STL
                dq INS_J
                dq INS_JEQ
                dq INS_JSUB

INS_START       istruc sInc
                    at sInc.str, db "START", 0
                    at sInc.len, dd 0
                iend

INS_END         istruc sInc
                    at sInc.str, db "END", 0
                    at sInc.len, dd 0
                iend

INS_BYTE        istruc sInc
                    at sInc.str, db "BYTE", 0
                    at sInc.len, dd 0
                iend

INS_WORD        istruc sInc
                    at sInc.str, db "WORD", 0
                    at sInc.len, dd 0
                iend

INS_RESB        istruc sInc
                    at sInc.str, db "RESB", 0
                    at sInc.len, dd 0
                iend

INS_RESW        istruc sInc
                    at sInc.str, db "REW", 0
                    at sInc.len, dd 0
                iend


INS_LDA        istruc sInc
                    at sInc.str, db "LDA", 0
                    at sInc.len, dd 3
                iend

INS_LDB        istruc sInc
                    at sInc.str, db "LDB", 0
                    at sInc.len, dd 3
                iend

INS_LDX        istruc sInc
                    at sInc.str, db "LDX", 0
                    at sInc.len, dd 3
                iend

INS_LDT        istruc sInc
                    at sInc.str, db "LDT", 0
                    at sInc.len, dd 3
                iend

INS_STA        istruc sInc
                    at sInc.str, db "STL", 0
                    at sInc.len, dd 3
                iend

INS_STX        istruc sInc
                    at sInc.str, db "STX", 0
                    at sInc.len, dd 3
                iend

INS_STL        istruc sInc
                    at sInc.str, db "STL", 0
                    at sInc.len, dd 3
                iend

INS_J        istruc sInc
                    at sInc.str, db "J", 0
                    at sInc.len, dd 3
                iend

INS_JEQ        istruc sInc
                    at sInc.str, db "JEQ", 0
                    at sInc.len, dd 3
                iend

INS_JSUB        istruc sInc
                    at sInc.str, db "JSUB", 0
                    at sInc.len, dd 3
                iend


section .text

chk_ins:
    %define str     qword [rbp - 0x8]
    %define idx     qword [rbp - 0x10]

    push rbp
    mov rbp, rsp
    sub rsp, 0x10
    mov str, rdi

    mov idx, 0
    jmp .loop2
.loop1:
    mov rdx, 7
    mov rax, INS_ARR
    mov rsi, idx
    mov rdi, [rax + rsi * 8]
    lea rsi, [rdi + sInc.str]
    mov rdi, str
    call strncmp wrt ..plt

    test rax, rax
    je .leave
    add idx, 0x1
.loop2:
    cmp idx, INSCNT
    jl .loop1
    mov idx, 0xffffffffffffffff

.leave:
    mov rax, idx
    leave
    ret

get_inc_len:
    %define idx     qword [rbp - 0x10]

    push rbp
    mov rbp, rsp
    sub rsp, 0x10
    mov idx, rdi

    mov rax, INS_ARR
    mov rsi, idx
    mov rdi, [rax + rsi * 8]
    mov rax, [rdi + sInc.len]

    leave
    ret
