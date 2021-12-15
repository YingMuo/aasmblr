struc sIns
    .str resb 8
    .len resq 1
    .code resq 1
endstruc

%define INSCNT 31


section .data

INS_ARR         dq INS_BYTE  ;  0
                dq INS_WORD  ;  1
                dq INS_RESB  ;  2
                dq INS_RESW  ;  3
                dq INS_START ;  4
                dq INS_END   ;  5
                dq INS_BASE  ;  6
                dq INS_LDA   ;  7
                dq INS_LDB   ;  8
                dq INS_LDCH  ;  9
                dq INS_LDX   ; 10
                dq INS_LDT   ; 11
                dq INS_STA   ; 12
                dq INS_STCH  ; 13
                dq INS_STX   ; 14
                dq INS_STL   ; 15
                dq INS_J     ; 16
                dq INS_JEQ   ; 17
                dq INS_JLT   ; 18
                dq INS_JSUB  ; 19
                dq INS_RSUB  ; 20
                dq INS_COMP  ; 21
                dq INS_TD    ; 22
                dq INS_RD    ; 23
                dq INS_WD    ; 24
                dq INS_TIX   ; 25
                dq INS_COMPR ; 26
                dq INS_TIXR  ; 27
                dq INS_CLEAR ; 28
                dq INS_PLDT  ; 29
                dq INS_PJSUB ; 30

INS_BYTE        istruc sIns
                    at sIns.str, db "BYTE", 0
                    at sIns.len, dq 0
                    at sIns.code, dq 0x00
                iend

INS_WORD        istruc sIns
                    at sIns.str, db "WORD", 0
                    at sIns.len, dq 0
                    at sIns.code, dq 0x00
                iend

INS_RESB        istruc sIns
                    at sIns.str, db "RESB", 0
                    at sIns.len, dq 0
                    at sIns.code, dq 0x00
                iend

INS_RESW        istruc sIns
                    at sIns.str, db "RESW", 0
                    at sIns.len, dq 0
                    at sIns.code, dq 0x00
                iend

INS_START       istruc sIns
                    at sIns.str, db "START", 0
                    at sIns.len, dq 0
                    at sIns.code, dq 0x00
                iend

INS_END         istruc sIns
                    at sIns.str, db "END", 0
                    at sIns.len, dq 0
                    at sIns.code, dq 0x00
                iend

INS_BASE        istruc sIns
                    at sIns.str, db "BASE", 0
                    at sIns.len, dq 0
                    at sIns.code, dq 0x00
                iend

INS_LDA         istruc sIns
                    at sIns.str, db "LDA", 0
                    at sIns.len, dq 3
                    at sIns.code, dq 0x00
                iend

INS_LDB         istruc sIns
                    at sIns.str, db "LDB", 0
                    at sIns.len, dq 3
                    at sIns.code, dq 0x68
                iend

INS_LDCH        istruc sIns
                    at sIns.str, db "LDCH", 0
                    at sIns.len, dq 3
                    at sIns.code, dq 0x50
                iend

INS_LDX         istruc sIns
                    at sIns.str, db "LDX", 0
                    at sIns.len, dq 3
                    at sIns.code, dq 0x04
                iend

INS_LDT         istruc sIns
                    at sIns.str, db "LDT", 0
                    at sIns.len, dq 3
                    at sIns.code, dq 0x74
                iend

INS_STA         istruc sIns
                    at sIns.str, db "STA", 0
                    at sIns.len, dq 3
                    at sIns.code, dq 0x0C
                iend

INS_STCH        istruc sIns
                    at sIns.str, db "STCH", 0
                    at sIns.len, dq 3
                    at sIns.code, dq 0x54
                iend

INS_STX         istruc sIns
                    at sIns.str, db "STX", 0
                    at sIns.len, dq 3
                    at sIns.code, dq 0x10
                iend

INS_STL         istruc sIns
                    at sIns.str, db "STL", 0
                    at sIns.len, dq 3
                    at sIns.code, dq 0x14
                iend

INS_J           istruc sIns
                    at sIns.str, db "J", 0
                    at sIns.len, dq 3
                    at sIns.code, dq 0x3C
                iend

INS_JEQ         istruc sIns
                    at sIns.str, db "JEQ", 0
                    at sIns.len, dq 3
                    at sIns.code, dq 0x30
                iend

INS_JLT         istruc sIns
                    at sIns.str, db "JLT", 0
                    at sIns.len, dq 3
                    at sIns.code, dq 0x38
                iend

INS_JSUB        istruc sIns
                    at sIns.str, db "JSUB", 0
                    at sIns.len, dq 3
                    at sIns.code, dq 0x48
                iend

INS_RSUB        istruc sIns
                    at sIns.str, db "RSUB", 0
                    at sIns.len, dq 3
                    at sIns.code, dq 0x4C
                iend

INS_COMP        istruc sIns
                    at sIns.str, db "COMP", 0
                    at sIns.len, dq 3
                    at sIns.code, dq 0x28
                iend

INS_TD          istruc sIns
                    at sIns.str, db "TD", 0
                    at sIns.len, dq 3
                    at sIns.code, dq 0xE0
                iend

INS_RD          istruc sIns
                    at sIns.str, db "RD", 0
                    at sIns.len, dq 3
                    at sIns.code, dq 0xD8
                iend

INS_WD          istruc sIns
                    at sIns.str, db "WD", 0
                    at sIns.len, dq 3
                    at sIns.code, dq 0xDC
                iend

INS_TIX         istruc sIns
                    at sIns.str, db "TIX", 0
                    at sIns.len, dq 3
                    at sIns.code, dq 0x2C
                iend

INS_COMPR        istruc sIns
                    at sIns.str, db "COMPR", 0
                    at sIns.len, dq 2
                    at sIns.code, dq 0xA0
                iend

INS_TIXR        istruc sIns
                    at sIns.str, db "TIXR", 0
                    at sIns.len, dq 2
                    at sIns.code, dq 0xB8
                iend

INS_CLEAR       istruc sIns
                    at sIns.str, db "CLEAR", 0
                    at sIns.len, dq 2
                    at sIns.code, dq 0xB4
                iend

INS_PLDT        istruc sIns
                    at sIns.str, db "+LDT", 0
                    at sIns.len, dq 4
                    at sIns.code, dq 0x74
                iend

INS_PJSUB       istruc sIns
                    at sIns.str, db "+JSUB", 0
                    at sIns.len, dq 4
                    at sIns.code, dq 0x48
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
    lea rsi, [rdi + sIns.str]
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

get_ins_str:
    %define idx     qword [rbp - 0x10]

    push rbp
    mov rbp, rsp
    sub rsp, 0x10
    mov idx, rdi

    mov rax, INS_ARR
    mov rsi, idx
    mov rdi, [rax + rsi * 8]
    lea rax, [rdi + sIns.str]

    leave
    ret

get_ins_len:
    %define idx     qword [rbp - 0x10]

    push rbp
    mov rbp, rsp
    sub rsp, 0x10
    mov idx, rdi

    mov rax, INS_ARR
    mov rsi, idx
    mov rdi, [rax + rsi * 8]
    mov rax, [rdi + sIns.len]

    leave
    ret

get_ins_code:
    %define idx     qword [rbp - 0x10]

    push rbp
    mov rbp, rsp
    sub rsp, 0x10
    mov idx, rdi

    mov rax, INS_ARR
    mov rsi, idx
    mov rdi, [rax + rsi * 8]
    mov rax, [rdi + sIns.code]

    leave
    ret