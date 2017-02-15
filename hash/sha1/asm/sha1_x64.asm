

; SHA-1 in x64 assembly
; Odzhan

ifdef __JWASM__
.x64
.model flat
endif

option casemap:none
option prologue:none
option epilogue:none

include sha.inc

.code

; initialize SHA-1 context
  public SHA1_Init
SHA1_Init proc
    push   rdi
    push   rax
    
    push   rcx
    pop    rdi
    ; ctx->state.v32[0] = 0x67452301
    mov    eax, 067452301h
    stosd
    ; ctx->state.v32[1] = 0xefcdab89
    mov    eax, 0efcdab89h
    stosd
    ; ctx->state.v32[2] = 0x98badcfe
    mov    eax, 098badcfeh
    stosd
    ; ctx->state.v32[3] = 0x10325476
    mov    eax, 010325476h
    stosd
    ; ctx->state.v32[4] = 0xc3d2e1f0
    mov    eax, 0c3d2e1f0h
    stosd
    xor    eax, eax
    mov    qword ptr[rcx][SHA_CTX.len], rax
    
    pop    rax
    pop    rdi
    ret
SHA1_Init endp
    
; update context
  public SHA1_Update
SHA1_Update proc
    push   rax
    push   rbx
    push   rcx
    push   rdx
    push   rsi
    push   rdi
    
    ; init
    push   rcx
    pop    rbx
    
    push   rdx
    pop    rsi

    ; idx = ctx->len & SHA_CBLOCK - 1;
    mov    rdx, qword ptr[rbx][SHA_CTX.len]
    and    rdx, SHA_CBLOCK - 1
    
    ; limit of (2^32)-1 bytes each update
    ; ctx->len += len;
    mov    rax, r8
    add    qword ptr[rbx][SHA_CTX.len], rax
    
update_loop:
    ; r = MIN(len, SHA_CBLOCK - idx);
    push   SHA_CBLOCK
    pop    rcx
    sub    ecx, edx
    cmp    ecx, eax
    cmovae ecx, eax
    ; memcpy ((void*)&ctx->buffer[idx], p, r);
    lea    rdi, [rbx][SHA_CTX.buffer][rdx]
    ; idx += r
    add    edx, ecx
    ; len -= r
    sub    eax, ecx
    rep    movsb
    ; if ((idx + r) < SHA_CBLOCK) break;
    cmp    edx, SHA_CBLOCK
    jb     exit_update
    push   rbx
    pop    rcx
    call   SHA1_Transform
    xor    edx, edx
    jmp    update_loop
exit_update:
    pop    rdi
    pop    rsi
    pop    rdx
    pop    rcx
    pop    rbx
    pop    rax
    
    ret
SHA1_Update endp

; finalize context
  public SHA1_Final
SHA1_Final proc
    push   rax
    push   rbx
    push   rcx
    push   rdx
    push   rsi
    push   rdi
    
    push   rcx
    pop    rdi
    
    push   rdx
    pop    rcx
    
    push   rdx
    pop    rsi
    
    ; uint64_t len=ctx->len & SHA_CBLOCK - 1;
    mov    rax, qword ptr[rcx][SHA_CTX.len]
    mov    rdx, rax
    and    rax, SHA_CBLOCK - 1
    
    ; fill with zeros
    ; memset (&ctx->buffer.v8[len], 0, SHA_CBLOCK - len);
    push   rcx
    push   rdi
    push   rax
    xchg   eax, ecx
    lea    rdi, [rsi][SHA_CTX.buffer][rcx]
    neg    rcx
    add    rcx, SHA_CBLOCK
    xor    eax, eax
    rep    stosb
    pop    rax
    pop    rdi
    pop    rcx
    
    ; ctx->buffer[len] = 0x80;
    mov    byte ptr[rcx][SHA_CTX.buffer][rax], 80h
    ; if (len >= sizeof (ctx->buffer))
    cmp    eax, 56
    jb     calc_len
    call SHA1_Transform
    
    ; memset (ctx->buffer.v8, 0, SHA_CBLOCK);
    push   rdi
    push   rcx
    lea    rdi, [rsi][SHA_CTX.buffer]
    push   SHA_CBLOCK/4
    pop    rcx
    xor    eax, eax
    rep    stosd
    pop    rcx
    pop    rdi
calc_len:
    ; ctx->buffer.v64[7] = SWAP64(ctx->len * 8);
    shl    rdx, 3
    bswap  rdx
    mov    qword ptr[rcx][SHA_CTX.buffer+56], rdx
    call   SHA1_Transform
    push   SHA_LBLOCK
    pop    rcx
swap_words:
    lodsd
    bswap  eax
    stosd
    loop   swap_words
    
    pop    rdi
    pop    rsi
    pop    rdx
    pop    rcx
    pop    rbx
    pop    rax
    ret    
SHA1_Final endp

_a equ <eax>  ; don't change
_b equ <ebx>
_c equ <ebp>
_d equ <edx>
_e equ <r8d>

_i equ <r9>
_w equ <r10>

t1 equ <edi>
t2 equ <r12d>

ctx equ <rcx>

; update context with input
  public SHA1_Transform
SHA1_Transform proc
    push   rax
    push   rbx
    push   rcx
    push   rdx
    push   rsi
    push   rdi
    push   rbp
    push   r9
    push   r10
    push   r11
    push   r12
    
    ; allocate space for work
    sub    rsp, 80*4
    mov    rdi, rsp
    mov    _w, rsp
    
    ; convert 16 words to big endian
    lea    rsi, [rcx][SHA_CTX.buffer]
    push   rcx
    push   16
    pop    rcx
swap_bytes:
    lodsd
    bswap  eax
    stosd
    loop   swap_bytes
    
    ; expand 16 words to 80
    mov    cl, 64
expand:
    mov    eax, [rdi -  3*4]
    xor    eax, [rdi -  8*4]
    xor    eax, [rdi - 14*4]
    xor    eax, [rdi - 16*4]
    rol    eax, 1
    stosd
    loop   expand
    
    pop    rcx
    ; load context
    push   rcx
    pop    rsi
    ; a = ctx->state.v32[0]
    lodsd
    xchg   _e, eax
    ; b = ctx->state.v32[1]
    lodsd
    xchg   _b, eax
    ; c = ctx->state.v32[2]
    lodsd
    xchg   _c, eax
    ; d = ctx->state.v32[3]
    lodsd
    xchg   _d, eax
    ; e = ctx->state.v32[4]
    lodsd
    xchg   _e, eax
    
    xor    _i, _i
    
    ; update context
transform_loop:
    mov    t1, _c
    cmp    _i, 20
    jae    @chk_g
    
    xor	   t1, _d
    and	   t1, _b
    xor	   t1, _d
    
    add    t1, 05a827999h
    jmp    @rnd_end
@chk_g:
    cmp    _i, 40
    jae    @chk_i
    
    xor	   t1, _d
    xor	   t1, _b
    
    add    t1, 06ed9eba1h
    jmp    @rnd_end
@chk_i:
    cmp    _i, 60
    jae    @do_h
    
    mov    t2, _c
    or     t1, _d
    and    t1, _b
    and    t2, _d
    or     t1, t2
    
    add    t1, 08f1bbcdch
    jmp    @rnd_end
@do_h:
    xor	   t1, _d
    xor	   t1, _b
    
    add    t1, 0ca62c1d6h
@rnd_end:
    ; t += ROL32(a, 5) + e
    mov    t2, _a
    rol    t2, 5
    add    t2, _e
    add    t1, t2
    add    t1, [_w + 4 * _i]

    ; e = d
    mov   _e, _d
    ; d = c
    mov   _d, _c
    ; c = ROL32(b, 30)
    mov   _c, _b
    rol   _c, 30
    ; b = a
    mov   _b, _a
    ; a = t
    mov   _a, t1
    ; i++
    inc   _i
    cmp   _i, 80
    jne   transform_loop
    
    add    rsp, 80*4
    
    push   rcx
    pop    rdi
    
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
    scasd
    ; ctx->state.v32[4] += e;
    add    [rdi], _e
    
    pop    r12
    pop    r11
    pop    r10
    pop    r9
    pop    rbp
    pop    rdi
    pop    rsi
    pop    rdx
    pop    rcx
    pop    rbx
    pop    rax
    
    ret
SHA1_Transform endp

    end
