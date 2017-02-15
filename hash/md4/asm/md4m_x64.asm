

; MD4 in x86-64 assembly
; Odzhan

ifdef __JWASM__
.x64
.model flat
endif

option casemap:none
option prologue:none
option epilogue:none

include md4.inc

.code

; initialize MD4 context
  public MD4_Init
MD4_Init proc
    push   rdi
    push   rax
    
    mov    rdi, rcx
    ; ctx->state.v32[0] = 0x67452301
    mov    eax, 067452301h
    stosd
    ; ctx->state.v32[1] = 0xefcdab89
    mov    eax, 0efcdab89h
    stosd
    ; ctx->state.v32[2] = 0x98badcfe
    mov    eax, 098badcfeh
    stosd
    ; ctx->state.v32[3] = 0x20325476
    mov    eax, 010325476h
    stosd
    xor    eax, eax
    mov    qword ptr[rcx][MD4_CTX.len], rax
    
    pop    rax
    pop    rdi 
    ret
MD4_Init endp
    
; update context
  public MD4_Update
MD4_Update proc
    push   rax
    push   rbx
    push   rcx
    push   rdx
    push   rsi
    push   rdi    
    
    ; get buffer index
    mov    rbx, rcx
    mov    rsi, rdx

    ; idx = ctx->len & MD4_CBLOCK - 1;
    mov    rdx, qword ptr[rbx][MD4_CTX.len]
    and    rdx, MD4_CBLOCK - 1
    
    ; limit of (2^32)-1 bytes each update
    ; ctx->len += len;
    mov    rax, r8
    add    qword ptr[rbx][MD4_CTX.len], rax
    
update_loop:
    push   rcx
    ; r = MIN(len, MD4_CBLOCK - idx);
    push   MD4_CBLOCK
    pop    rcx
    sub    ecx, edx
    cmp    ecx, eax
    cmovae ecx, eax
    ; memcpy ((void*)&ctx->buffer[idx], p, r);
    lea    rdi, [rbx][MD4_CTX.buffer][rdx]
    ; idx += r
    add    edx, ecx
    ; len -= r
    sub    eax, ecx
    rep    movsb
    pop    rcx
    ; if ((idx + r) < MD4_CBLOCK) break;
    cmp    edx, MD4_CBLOCK
    jb     exit_update
    call   MD4_Transform
    cdq
    jmp    update_loop
exit_update:
    pop    rdi
    pop    rsi
    pop    rdx
    pop    rcx
    pop    rbx
    pop    rax
    ret
MD4_Update endp

; finalize context
  public MD4_Final
MD4_Final proc
    push   rax
    push   rbx
    push   rcx
    push   rdx
    push   rsi
    push   rdi
    
    mov    rsi, rdx
    mov    rdi, rcx
    
    xchg   rdx, rcx

    ; uint64_t len=ctx->len & MD4_CBLOCK - 1;
    mov    rax, qword ptr[rsi][MD4_CTX.len]
    and    rax, MD4_CBLOCK - 1
    
    ; fill with zeros
    ; memset (&ctx->buffer.v8[len], 0, MD4_CBLOCK - len);
    push   rcx
    push   rdi
    push   rax
    xchg   eax, ecx
    lea    rdi, [rsi][MD4_CTX.buffer][rcx]
    neg    rcx
    add    rcx, MD4_CBLOCK
    xor    eax, eax
    rep    stosb
    pop    rax
    pop    rdi
    pop    rcx
    
    ; ctx->buffer[len] = 0x80;
    mov    byte ptr[rsi][MD4_CTX.buffer][rax], 80h
    ; if (len >= sizeof (ctx->buffer))
    cmp    eax, 56
    jb     calc_len
    call   MD4_Transform
    
    ; memset (ctx->buffer.v8, 0, MD4_CBLOCK);
    push   rdi
    push   rcx
    lea    rdi, [rsi][MD4_CTX.buffer]
    push   MD4_CBLOCK/4
    pop    rcx
    xor    eax, eax
    rep    stosd
    pop    rcx
    pop    rdi
calc_len:
    ; ctx->buffer.v64[7] = ctx->len * 8;
    mov    rax, qword ptr[rsi][MD4_CTX.len]
    shl    rax, 3
    mov    qword ptr[rsi][MD4_CTX.buffer+56], rax 
    call   MD4_Transform
    
    push   MD4_LBLOCK
    pop    rcx
    rep    movsd
    
    pop    rdi
    pop    rsi
    pop    rdx
    pop    rcx
    pop    rbx
    pop    rax
    ret     
MD4_Final endp

_a equ <eax>  ; don't change
_b equ <ebx>
_c equ <ebp>
_d equ <edx>

_i equ <r8>
x  equ <r9>

x1 equ <r10>
x2 equ <r11>

t1 equ <r12d>
t2 equ <r13d>

; update context with input
MD4_Transform proc ; stdcall uses rax rbx rcx rdx rsi rdi ctx:ptr MD4_CTX
    push   rax
    push   rbx
    push   rcx
    push   rdx
    push   rsi
    push   rdi
    push   rbp
    push   r12
    push   r13
    
    mov    rsi, rcx
    push   rcx
    lea    x, [rsi][MD4_CTX.buffer]
    
    ; load context
    ; a = ctx->state.v32[0]
    lodsd
    xchg   _d, eax
    ; b = ctx->state.v32[1]
    lodsd
    xchg   _b, eax
    ; c = ctx->state.v32[2]
    lodsd
    xchg   _c, eax
    ; d = ctx->state.v32[3]
    lodsd
    xchg   _d, eax

    xor    _i, _i
transform_loop:
    lea    x1, [idx]
    movzx  x1, byte ptr [x1+_i]     ; idx[i]
    add    _a, [x+4*x1]             ; a += x[i]
    mov    t1, _c
    mov    x2, _i
    and    x2, 3
    cmp    _i, 16
    jae    chk_g
    ; perform FF
    lea    x1, [rotf]
    mov    cl, byte ptr [x1+x2]    ; rotf[i%4]
    xor    t1, _d
    and    t1, _b
    xor    t1, _d
    jmp    rnd_end
chk_g:
    cmp    _i, 32
    jae    do_h
    ; perform GG
    lea    x1, [rotg]
    mov    cl, byte ptr [x1+x2]    ; rotg[i%4]
    mov    t2, _c
    or     t1, _d
    and    t1, _b
    and    t2, _d
    or     t1, t2
    add    t1, 05a827999h
    jmp    rnd_end
do_h:
    ; perform HH
    lea    x1, [roth]
    mov    cl, byte ptr [x1+x2]    ; roth[i%4]
    mov    t1, _b
    xor    t1, _c
    xor    t1, _d
    add    t1, 06ed9eba1h
rnd_end:
    add    _a, t1
    rol    _a, cl

    mov    t1, _a
    mov    _a, _d
    mov    _d, _c
    mov    _c, _b
    mov    _b, t1

    inc    _i
    cmp    _i, 48
    jne    transform_loop
    
    ; restore registers
    pop    rcx
    push   rcx
    pop    rdi
    ; save context
    ; ctx->state.v32[0] += a;
    add    [rdi], _a
    scasd
    ; ctx->state.v32[1] += b;
    add    [rdi], _b
    scasd
    ; ctx->state.v32[2] += c;
    add    [rdi], _c
    scasd
    ; ctx->state.v32[3] += d;
    add    [rdi], _d

    pop    r13
    pop    r12
    pop    rbp
    pop    rdi
    pop    rsi
    pop    rdx
    pop    rcx
    pop    rbx
    pop    rax
    ret
MD4_Transform endp

rotf label qword
     db 3, 7, 11, 19
rotg label qword
     db 3, 5,  9, 13
roth label qword
     db 3, 9, 11, 15
idx  label qword
     db 0, 1, 2,  3, 4,  5, 6,  7, 8, 9, 10, 11, 12, 13, 14, 15
     db 0, 4, 8, 12, 1,  5, 9, 13, 2, 6, 10, 14,  3,  7, 11, 15
     db 0, 8, 4, 12, 2, 10, 6, 14, 1, 9,  5, 13,  3, 11,  7, 15
     
    end
