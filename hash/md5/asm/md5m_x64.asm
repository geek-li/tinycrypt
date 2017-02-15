

; MD5 in x64 assembly
; Odzhan

ifdef __JWASM__
.x64
.model flat
endif

option casemap:none
option prologue:none
option epilogue:none

include md5.inc

.code

; initialize MD4 context
  public MD5_Init
MD5_Init proc ;C ctx:MD5_CTX
    push   rdi
    push   rax
    
    mov    rdi, rcx
    ; ctx->h[0] = 0x67452301
    mov    eax, 067452301h
    stosd
    ; ctx->h[1] = 0xefcdab89
    mov    eax, 0efcdab89h
    stosd
    ; ctx->h[2] = 0x98badcfe
    mov    eax, 098badcfeh
    stosd
    ; ctx->h[3] = 0x10325476
    mov    eax, 010325476h
    stosd
    xor    eax, eax
    mov    qword ptr[rcx][MD5_CTX.len], rax
    
    pop    rax
    pop    rdi 
    ret
MD5_Init endp
    
; update context
  public MD5_Update
MD5_Update proc ;C uses rax rbx rcx rdx rsi rdi ctx:ptr MD5_CTX, input:ptr byte, len:size_t
    push   rax
    push   rbx
    push   rcx
    push   rdx
    push   rsi
    push   rdi    
    
    ; get buffer index
    mov    rbx, rcx
    mov    rsi, rdx

    ; idx = ctx->len & MD5_CBLOCK - 1;
    mov    rdx, qword ptr[rbx][MD5_CTX.len]
    and    rdx, MD5_CBLOCK - 1
    
    ; limit of (2^32)-1 bytes each update
    ; ctx->len += len;
    mov    rax, r8
    add    qword ptr[rbx][MD5_CTX.len], rax
    
update_loop:
    push   rcx
    ; r = MIN(len, MD5_CBLOCK - idx);
    push   MD5_CBLOCK
    pop    rcx
    sub    ecx, edx
    cmp    ecx, eax
    cmovae ecx, eax
    ; memcpy ((void*)&ctx->buffer[idx], p, r);
    lea    rdi, [rbx][MD5_CTX.buffer][rdx]
    ; idx += r
    add    edx, ecx
    ; len -= r
    sub    eax, ecx
    rep    movsb
    pop    rcx
    ; if ((idx + r) < MD5_CBLOCK) break;
    cmp    edx, MD5_CBLOCK
    jb     exit_update
    call   MD5_Transform
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
MD5_Update endp

; finalize context
  public MD5_Final
MD5_Final proc ; C uses rax rbx rcx rdx rsi dgst:ptr byte, ctx:ptr MD5_CTX
    push   rax
    push   rbx
    push   rcx
    push   rdx
    push   rsi
    push   rdi
    
    mov    rsi, rdx
    mov    rdi, rcx
    
    xchg   rdx, rcx

    ; uint64_t len=ctx->len & MD5_CBLOCK - 1;
    mov    rax, qword ptr[rsi][MD5_CTX.len]
    and    rax, MD5_CBLOCK - 1
    
    ; fill with zeros
    ; memset (&ctx->buffer.v8[len], 0, MD4_CBLOCK - len);
    push   rcx
    push   rdi
    push   rax
    xchg   eax, ecx
    lea    rdi, [rsi][MD5_CTX.buffer][rcx]
    neg    rcx
    add    rcx, MD5_CBLOCK
    xor    eax, eax
    rep    stosb
    pop    rax
    pop    rdi
    pop    rcx
    
    ; ctx->buffer[len] = 0x80;
    mov    byte ptr[rsi][MD5_CTX.buffer][rax], 80h
    ; if (len >= sizeof (ctx->buffer))
    cmp    eax, 56
    jb     calc_len
    call   MD5_Transform
    
    ; memset (ctx->buffer.v8, 0, MD5_CBLOCK);
    push   rdi
    push   rcx
    lea    rdi, [rsi][MD5_CTX.buffer]
    push   MD5_CBLOCK/4
    pop    rcx
    xor    eax, eax
    rep    stosd
    pop    rcx
    pop    rdi
calc_len:
    ; ctx->bitlen = ctx->len * 8;
    mov    rax, qword ptr[rsi][MD5_CTX.len]
    shl    rax, 3
    mov    qword ptr[rsi][MD5_CTX.buffer+56], rax 
    call   MD5_Transform
    
    push   MD5_LBLOCK
    pop    rcx
    rep    movsd
    
    pop    rdi
    pop    rsi
    pop    rdx
    pop    rcx
    pop    rbx
    pop    rax
    ret     
MD5_Final endp

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

rotf label qword
  db 7,12,17,22
rotg label qword
  db 5, 9,14,20
roth label qword
  db 4,11,16,23
roti label qword
  db 6,10,15,21
idx  label qword
  db 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
  db 1,6,11,0,5,10,15,4,9,14,3,8,13,2,7,12
  db 5,8,11,14,1,4,7,10,13,0,3,6,9,12,15,2
  db 0,7,14,5,12,3,10,1,8,15,6,13,4,11,2,9

ifndef DYNAMIC

tc label qword
  dd 0D76AA478h, 0E8C7B756h, 0242070DBh, 0C1BDCEEEh
  dd 0F57C0FAFh, 04787C62Ah, 0A8304613h, 0FD469501h
  dd 0698098D8h, 08B44F7AFh, 0FFFF5BB1h, 0895CD7BEh
  dd 06B901122h, 0FD987193h, 0A679438Eh, 049B40821h
  dd 0F61E2562h, 0C040B340h, 0265E5A51h, 0E9B6C7AAh
  dd 0D62F105Dh, 002441453h, 0D8A1E681h, 0E7D3FBC8h
  dd 021E1CDE6h, 0C33707D6h, 0F4D50D87h, 0455A14EDh
  dd 0A9E3E905h, 0FCEFA3F8h, 0676F02D9h, 08D2A4C8Ah
  dd 0FFFA3942h, 08771F681h, 06D9D6122h, 0FDE5380Ch
  dd 0A4BEEA44h, 04BDECFA9h, 0F6BB4B60h, 0BEBFBC70h
  dd 0289B7EC6h, 0EAA127FAh, 0D4EF3085h, 004881D05h
  dd 0D9D4D039h, 0E6DB99E5h, 01FA27CF8h, 0C4AC5665h
  dd 0F4292244h, 0432AFF97h, 0AB9423A7h, 0FC93A039h
  dd 0655B59C3h, 08F0CCC92h, 0FFEFF47Dh, 085845DD1h
  dd 06FA87E4Fh, 0FE2CE6E0h, 0A3014314h, 04E0811A1h
  dd 0F7537E82h, 0BD3AF235h, 02AD7D2BBh, 0EB86D391h
  
else

init_fpu proc
    ; round numbers down
    push   rax
    fstcw  [rsp]            ; store control word
    pop    rax
    or     ax, 00400H       ; set round down bit
    and    ax, 0F7FFH       ; clear bit
    push   rax
    fldcw  [rsp]            ; load control word
    pop    rax
    ret
init_fpu endp

    ; get sine of i+1 and return 32-bit fractional part as integer
sin2int proc
    push   1
    pop    r12
    shl    r12, 32
    push   r12
    fild   qword ptr[rsp]   ; load 2^32 or 4294967296UL
    lea    r12, [_i+1]
    push   r12
    fild   qword ptr[rsp]
    fsin                    ; fsin (integer)
    fabs
    fmul                    ; fabs(fsin (integer)) * 4294967296UL
    fistp  qword ptr[rsp]
    mov    r12d, [rsp]
    add    rsp, 2*8
    ret
sin2int endp

endif

; update context with input
MD5_Transform proc
    push   rax
    push   rbx
    push   rcx
    push   rdx
    push   rsi
    push   rdi
    push   rbp
    push   r12
    push   r13
    
ifdef DYNAMIC
    ; set control word to round down numbers
    call   init_fpu
endif
    ; load context
    mov    rsi, rcx ; ctx
    push   rcx
    lea    x, [rsi][MD5_CTX.buffer]
    
    ; a = ctx->h[0]
    lodsd
    xchg   _d, eax
    ; b = ctx->h[1]
    lodsd
    xchg   _b, eax
    ; c = ctx->h[2]
    lodsd
    xchg   _c, eax
    ; d = ctx->h[3]
    lodsd
    xchg   _d, eax

    xor    _i, _i
    
transform_loop:
    lea    x1, [idx]
    movzx  x1, byte ptr[x1+_i]      ; idx[i]
    add    _a, dword ptr[x+4*x1]   ; a += x[i]
    mov    t1, _c
    mov    x2, _i
    and    x2, 3                    ; i %= 4
    ; ========================================
    cmp    _i, 16
    jae    @chk_g
    ; ========================================
    lea    x1, [rotf]
    mov    cl, byte ptr [x1+x2]    ; rotf[i%4]
    xor    t1, _d
    and    t1, _b
    xor    t1, _d
    jmp    @rnd_end
    ; ========================================
@chk_g:
    cmp    _i, 32
    jae    @chk_h
    ; ========================================
    lea    x1, [rotg]
    mov    cl, byte ptr [x1+x2]    ; rotg[i%4]
    xor    t1, _b
    and    t1, _d
    xor    t1, _c
    jmp    @rnd_end
    ; =======================================
@chk_h:
    cmp    _i, 48
    jae    @do_i
    ; =======================================
    lea    x1, [roth]
    mov    cl, byte ptr [x1+x2]    ; roth[i%4]
    xor    t1, _d
    xor    t1, _b
    jmp    @rnd_end
    ; ========================================
@do_i:
    lea    x1, [roti]
    mov    cl, byte ptr [x1+x2]
    mov    t1, _d
    xor    t1, -1
    or     t1, _b
    xor    t1, _c
@rnd_end:  
    add    _a, t1
ifdef DYNAMIC
    call   sin2int
else
    lea    x1, [tc]
    mov    t1, dword ptr [x1+_i*4]
endif
    
    add    _a, t1
    rol    _a, cl
    add    _a, _b
    
    mov    t1, _a
    mov    _a, _d
    mov    _d, _c
    mov    _c, _b
    mov    _b, t1
    
    inc    _i
    cmp    _i, 64
    jne    transform_loop
    
    ; restore registers
    pop    rcx
    push   rcx
    pop    rdi
    ; save context
    ; ctx->h[0] += a;
    add    [rdi], _a
    scasd
    ; ctx->h[1] += b;
    add    [rdi], _b
    scasd
    ; ctx->h[2] += c;
    add    [rdi], _c
    scasd
    ; ctx->h[3] += d;
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
MD5_Transform endp

    end
