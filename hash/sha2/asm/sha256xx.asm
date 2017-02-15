

; SHA-256 in x86 assembly
; Odzhan

.686
.model flat, C

option casemap:none
option prologue:none
option epilogue:none

; context
sha256_ctx struct
  state  dd 8 dup (?)
  len    dd ?
  buffer db 64 dup (?)
sha256_ctx ends

; workspace for compression
sha256_ws struct
  h dd  8 dup (?)
  w dd 64 dup (?)
sha256_ws ends

.code

sha256x_start label dword

SHA256 proc
    pushad
    
    push   eax
    fstcw  [esp]            ; store control word
    pop    eax
    or     ax, 00400H       ; set round down bit
    and    ax, 0F7FFH       ; clear bit
    push   eax
    fldcw  [esp]            ; load control word
    pop    eax
    
    mov    ebx, esp          ; 
    sub    esp, 4+64+8*4     ; alloc space for ctx
    mov    edi, esp
    push   edi               ; save ptr to ctx
    
    push   8                 ; load 8 32-bit words
    pop    ecx
    call   load_iv
  dd 06a09e667h, 0bb67ae85h 
  dd 03c6ef372h, 0a54ff53ah
  dd 0510e527fh, 09b05688ch
  dd 01f83d9abh, 05be0cd19h
load_iv:
    pop    esi
    rep    movsd
    
    xor    eax, eax
    stosd                    ; ctx.len = 0
    cdq                      ; idx = 0
    
    mov    ebp, [ebx+32+ 4]  ; ecx = inlen
    mov    esi, [ebx+32+ 8]  ; esi = in
    mov    ebx, [ebx+32+12]  ; ebx = out
chk_len:
    test   ebp, ebp
    jnz    upd_buf
    
    push   edi
    add    edi, edx
    mov    ecx, edx
    mov    al, 80h           ; ctx.buffer[idx++] = 0x80;
    stosb
    xor    eax, eax
    neg    ecx
    add    ecx, 63
    rep    stosb
    pop    edi
    
    ; if (edx >= 56)
    cmp    edx, 56
    jb     add_bits
    
    call   SHA256_Transform
    
    push   edi               ; zero ctx.buffer
    mov    cl, 64
    rep    stosb
    pop    edi
    
add_bits:
    mov    eax, [edi-4]      ; eax = ctx.len
    shl    eax, 3            ; multiply by 8
    bswap  eax               ; convert to big endian
    mov    [edi+60], eax     ; save in buffer
    jmp    upd_ctx
upd_buf:
    lodsb                    ; al = *p++
    mov    [edi+edx], al
    inc    dword ptr[edi-4]  ; ctx.len++
    inc    edx               ; idx++
    cmp    edx, 64           ; buffer full?
    jnz    upd_len
upd_ctx:
    call   SHA256_Transform
    xor    edx, edx          ; idx=0
upd_len:
    dec    ebp               ; while (--inlen >= 0)
    jns    chk_len

    mov    edi, ebx          ; edi = out
    pop    esi               ; esi = ctx.state
    push   8                 ; save 256-bit state/hash
    pop    ecx
save_state:
    lodsd                    ; load word
    bswap  eax               ; convert to little endian
    stosd                    ; save in out buffer
    loop   save_state
    add    esp, 4+64+8*4     ; fix up stack
    popad
    ret
SHA256 endp

_a textequ <dword ptr[esp][sha256_ws.h[0*4]]>
_b textequ <dword ptr[esp][sha256_ws.h[1*4]]>
_c textequ <dword ptr[esp][sha256_ws.h[2*4]]>
_d textequ <dword ptr[esp][sha256_ws.h[3*4]]>
_e textequ <dword ptr[esp][sha256_ws.h[4*4]]>
_f textequ <dword ptr[esp][sha256_ws.h[5*4]]>
_g textequ <dword ptr[esp][sha256_ws.h[6*4]]>
_h textequ <dword ptr[esp][sha256_ws.h[7*4]]>

s0 equ <eax>
s1 equ <ebx>
i  equ <ecx>
t1 equ <edx>
t2 equ <esi>
t3 equ <ebp>

SHA256_Transform proc
    pushad    
    
     ; save pointer to context
    mov    esi, edi
    sub    esi, 4 + 8*4
    push   esi
    
    ; allocate work space
    sub    esp, sizeof sha256_ws
    
    ; Initialize working variables 
    ; to current hash value:
    mov    edi, esp
    push   8
    pop    ecx
    rep    movsd
    ; skip len
    lodsd
    
    ; convert 16 words to big endian
    mov    cl, 16
swap_bytes:
    lodsd
    bswap  eax
    stosd
    loop   swap_bytes

    ; Extend the first 16 words into the 
    ; remaining 48 words w[16..63] of 
    ; the message schedule array:
    mov    cl, 48
expand:
    ; s0 := (w[i-15] rightrotate 7) 
    ;   xor (w[i-15] rightrotate 18) 
    ;   xor (w[i-15] rightshift 3)
    mov    s0, [edi - 15*4]
    mov    t1, s0
    mov    t2, s0
    ror    s0, 7
    ror    t1, 18
    shr    t2, 3
    xor    s0, t1
    xor    s0, t2
    ; s1 := (w[i-2] rightrotate 17) 
    ;   xor (w[i-2] rightrotate 19) 
    ;   xor (w[i-2] rightshift 10)
    mov    s1, [edi - 2*4]
    mov    t1, s1
    mov    t2, s1
    ror    s1, 17
    ror    t1, 19
    shr    t2, 10
    xor    s1, t1
    xor    s1, t2
    ; w[i] := w[i-16] + s0 + w[i-7] + s1
    add    s0, [edi - 16*4]
    add    s1, [edi -  7*4]
    add    s0, s1
    stosd
    loop   expand
    
    ; update context
update_loop:
    ; S1 := EP1(e)
    
    ; S1 := (e rightrotate 6) 
    ;   xor (e rightrotate 11) 
    ;   xor (e rightrotate 25)
    mov    s1, _e
    mov    t1, s1
    ror    s1, 25
    ror    t1, 6
    xor    s1, t1
    ror    t1, 11-6
    xor    s1, t1
    ; CH(e, f, g)
    ; ch := (e and f) xor ((not e) and g)
    mov    t1, _f
    xor    t1, _g
    and    t1, _e
    xor    t1, _g
    ; temp1 := h + S1 + ch + k[i] + w[i]
    add    t1, s1
    add    t1, _h
    call   cbr2int
    add    t1, eax
    add    t1, dword ptr [esp][sha256_ws.w[4*i]]
    ; S0 := EP0(a)
    ; S0 := (a rightrotate 2) 
    ;   xor (a rightrotate 13) 
    ;   xor (a rightrotate 22)
    mov    s0, _a
    mov    t2, s0
    ror    s0, 22
    ror    t2, 2
    xor    s0, t2
    ror    t2, 13-2
    xor    s0, t2
    ; MAJ(a, b, c)
    ; maj := (a and b) xor (a and c) xor (b and c)
    mov    t2, _a
    mov    t3, _a
    or     t2, _b
    and    t2, _c
    and    t3, _b
    or     t2, t3
    ; temp2 := S0 + maj
    add    t2, s0
    ; d = d + temp1
    add    _d, t1
    ; h = temp1 + temp2
    mov    _h, t1
    add    _h, t2
    ; shift state
    mov    esi, esp
    mov    edi, esp    
    push   i
    push   edi
    mov    cl, 7
    lodsd                   ; load a
shift_state:
    scasd                   ; skip a
    xchg   eax, [edi]       ; 
    loop   shift_state
    pop    edi
    stosd
    pop    i
    
    ; i++
    inc    i
    cmp    i, 64
    jne    update_loop
    
    mov    esi, esp
    add    esp, sizeof sha256_ws
    pop    edi
    mov    cl, 8
save_state:
    lodsd
    add    eax, [edi]
    stosd
    loop   save_state
    popad
    ret
SHA256_Transform endp

    ; get cubic root of number 
    ; return 32-bit fractional part as integer
cbr2int:
    push   1
    push   0
    fild   qword ptr[esp]   ; load 2^32
    push   1
    fild   dword ptr[esp]
    push   3
    fild   dword ptr[esp]
    fdivp                   ; 1.0/3.0
    movzx  eax, word ptr[primes+2*i]
    push   eax
    fild   dword ptr[esp]   ;
    fyl2x                   ; ->log2(Src1)*exponent
    fld    st(0)            ; copy the logarithm
    frndint                 ; keep only the characteristic
    fsub   st(1), st        ; keeps only the mantissa
    fxch                    ; get the mantissa on top
    f2xm1                   ; ->2^(mantissa)-1
    fscale
    fstp   st(1)            ; copy result over and "pop" it
    fmul                    ; * 2^32
    fistp  qword ptr[esp]   ; save integer
    pop    eax
    add    esp, 4*4         ; release stack
    ret

primes dw 2, 3, 5, 7, 11, 13, 17, 19
       dw 23, 29, 31, 37, 41, 43, 47, 53
       dw 59, 61, 67, 71, 73, 79, 83, 89
       dw 97, 101, 103, 107, 109, 113, 127, 131
       dw 137, 139, 149, 151, 157, 163, 167, 173
       dw 179, 181, 191, 193, 197, 199, 211, 223 
       dw 227, 229, 233, 239, 241, 251, 257, 263
       dw 269, 271, 277, 281, 283, 293, 307, 311
primes_len equ $-primes

sha256x_end label dword

%echo Total size of code is @CatStr (%offset sha256x_end - offset sha256x_start) bytes
    
    end