

; SHA-256 in x86 assembly
; Odzhan

.686
.model flat, C

;DYNAMIC equ <>

option casemap:none
option prologue:none
option epilogue:none

include sha2.inc

.code

s0 equ <eax>
s1 equ <ebx>
i  equ <ecx>
t1 equ <edx>
t2 equ <esi>
t3 equ <ebp>

; Initialize hash values:
; (first 32 bits of the fractional parts of the square 
; roots of the first 8 primes 2..19):
  public SHA256_Initx
SHA256_Initx proc
    pushad
ifdef DYNAMIC
    call   init_fpu
endif
    mov    edi, [esp+32+4]
    ; ctx->len = 0;
    xor    eax, eax
    mov    [edi][SHA256_CTX.len+0], eax
    mov    [edi][SHA256_CTX.len+4], eax

    xor    i, i
ifdef DYNAMIC
load_state:
    call   sqrt2int
    stosd
    inc    i
    cmp    i, 8
    jnz    load_state
else
    ; ctx->h[0] = 0x6a09e667
    mov    eax, 06a09e667h
    stosd
    ; ctx->h[1] = 0xbb67ae85
    mov    eax, 0bb67ae85h
    stosd
    ; ctx->h[2] = 0x3c6ef372
    mov    eax, 03c6ef372h
    stosd
    ; ctx->h[3] = 0xa54ff53a
    mov    eax, 0a54ff53ah
    stosd
    ; ctx->h[4] = 0x510e527f
    mov    eax, 0510e527fh
    stosd
    ; ctx->h[5] = 0x9b05688c
    mov    eax, 09b05688ch
    stosd
    ; ctx->h[6] = 0x1f83d9ab
    mov    eax, 01f83d9abh
    stosd
    ; ctx->h[7] = 0x5be0cd19
    mov    eax, 05be0cd19h
    stosd
endif
    popad
    ret
SHA256_Initx endp
    
; update context
  public SHA256_Updatex
SHA256_Updatex proc
    pushad
    ; get buffer index
    mov    ebx, [esp+32+4]
    mov    esi, [esp+32+8]

    ; idx = ctx->len & SHA256_CBLOCK - 1;
    mov    edx, [ebx][SHA256_CTX.len]
    and    edx, SHA256_CBLOCK - 1
    
    ; limit of (2^32)-1 bytes each update
    ; ctx->len += len;
    mov    eax, [esp+32+12]
    add    [ebx][SHA256_CTX.len+0], eax
    adc    [ebx][SHA256_CTX.len+4], 0
    
update_loop:
    push   ecx
    ; r = (len >= (SHA256_CBLOCK - idx)) ? SHA256_CBLOCK - idx : len;
    push   SHA256_CBLOCK
    pop    ecx
    sub    ecx, edx
    cmp    ecx, eax
    cmovae ecx, eax
    ; memcpy ((void*)&ctx->buffer[idx], p, r);
    lea    edi, [ebx][SHA256_CTX.buffer.v8][edx]
    ; idx += r
    add    edx, ecx
    ; len -= r
    sub    eax, ecx
    rep    movsb
    pop    ecx
    ; if ((idx + r) < SHA256_CBLOCK) break;
    cmp    edx, SHA256_CBLOCK
    jb     exit_update
    push   ebx
    call   SHA256_Transformx
    cdq
    jmp    update_loop
exit_update:
    popad
    ret
SHA256_Updatex endp

; finalize context
  public SHA256_Finalx
SHA256_Finalx proc
    pushad
    mov    esi, [esp+32+8] ; ctx
    ; uint64_t len=ctx->len & SHA256_CBLOCK - 1;
    mov    edx, [esi][SHA256_CTX.len+0]
    and    edx, SHA256_CBLOCK - 1
    ; memset (&ctx->buffer.v8[len], 0, SHA256_CBLOCK - len);
    lea    edi, [esi][SHA256_CTX.buffer.v8][edx]
    push   SHA256_CBLOCK
    pop    ecx
    sub    ecx, edx
    xor    eax, eax
    rep    stosb
    ; ctx->buffer.v8[len] = 0x80;
    mov    byte ptr[esi][SHA256_CTX.buffer.v8][edx], 80h
    ; if (len >= 56)
    cmp    edx, 56
    jb     calc_len
    ; SHA256_Transform (ctx);
    push   esi
    call   SHA256_Transformx
    ; memset (ctx->buffer.v8, 0, SHA256_CBLOCK);
    lea    edi, [esi][SHA256_CTX.buffer.v8]
    xor    eax, eax
    push   SHA256_CBLOCK/4
    pop    ecx
    rep    stosd
calc_len:
    ; ctx->buffer.v64[7] = SWAP64(ctx->len * 8);
    mov    eax, [esi][SHA256_CTX.len+0]
    mov    edx, [esi][SHA256_CTX.len+4]
    shld   edx, eax, 3
    shl    eax, 3
    bswap  eax
    bswap  edx
    mov    dword ptr[esi][SHA256_CTX.buffer.v64+7*8+0], edx
    mov    dword ptr[esi][SHA256_CTX.buffer.v64+7*8+4], eax
    ; SHA256_Transform(ctx);
    push   esi
    call   SHA256_Transformx
    ; for (i=0; i<SHA256_LBLOCK; i++) {
    ;   ctx->state.v32[i] = SWAP32(ctx->state.v32[i]);
    ; }
    ;
    ; memcpy (dgst, ctx->state.v8, SHA256_DIGEST_LENGTH);
    push   SHA256_LBLOCK
    pop    ecx
    mov    edi, [esp+32+4] ; dgst
swap_words:
    lodsd
    bswap  eax
    stosd
    dec    ecx
    jnz    swap_words
    popad
    ret    
SHA256_Finalx endp

sha256_ws struct
  h dd  8 dup (?)
  w dd 64 dup (?)
sha256_ws ends

_a textequ <dword ptr[esp][sha256_ws.h[0*4]]>
_b textequ <dword ptr[esp][sha256_ws.h[1*4]]>
_c textequ <dword ptr[esp][sha256_ws.h[2*4]]>
_d textequ <dword ptr[esp][sha256_ws.h[3*4]]>
_e textequ <dword ptr[esp][sha256_ws.h[4*4]]>
_f textequ <dword ptr[esp][sha256_ws.h[5*4]]>
_g textequ <dword ptr[esp][sha256_ws.h[6*4]]>
_h textequ <dword ptr[esp][sha256_ws.h[7*4]]>

SHA256_Transformx proc
    pushad
    
    mov    ebx, [esp+32+4] ; ctx
    push   ebx
    
    ; allocate work space
    sub    esp, sizeof sha256_ws
    
    ; Initialize working variables to current hash value:
    mov    edi, esp
    mov    esi, ebx
    push   8
    pop    ecx
    rep    movsd
    
    ; convert 16 words to big endian
    mov    cl, 16
swap_bytes:
    lodsd
    bswap  eax
    stosd
    loop   swap_bytes

    ; Extend the first 16 words into the remaining 48 words w[16..63] of 
    ; the message schedule array:
    mov    cl, 48
expand:
    ; s0 := (w[i-15] rightrotate 7) xor (w[i-15] rightrotate 18) xor (w[i-15] rightshift 3)
    mov    s0, [edi - 15*4]
    mov    t1, s0
    mov    t2, s0
    ror    s0, 7
    ror    t1, 18
    shr    t2, 3
    xor    s0, t1
    xor    s0, t2
    ; s1 := (w[i-2] rightrotate 17) xor (w[i-2] rightrotate 19) xor (w[i-2] rightshift 10)
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
    ; S1 := (e rightrotate 6) xor (e rightrotate 11) xor (e rightrotate 25)
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
ifdef DYNAMIC
    call   cbr2int
    add    t1, eax
else
    call   ld_k
ld_k:
    pop    t2
    lea    t2, [t2+(_k-ld_k)]
    add    t1, dword ptr [t2+4*i]
endif
    add    t1, dword ptr [esp][sha256_ws.w[4*i]]
    ; S0 := EP0(a)
    ; S0 := (a rightrotate 2) xor (a rightrotate 13) xor (a rightrotate 22)
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
    xchg   eax, [edi]       ; a becomes b, eax then has b to swap with c and so on
    loop   shift_state
    pop    edi
    stosd
    pop    i
    
    ; i++
    inc    i
    cmp    i, 64
    jne    update_loop
    
    ; Produce the final hash value (big-endian):
    ; digest := hash := h0 append h1 append h2 append h3 append 
    ; h4 append h5 append h6 append h7
    mov    esi, esp
    add    esp, sizeof sha256_ws
    pop    edi
    mov    cl, 8
save_state:
    lodsd
    add    eax, [edi]
    stosd
    loop   save_state
    ; restore registers
    popad
    ret    4
SHA256_Transformx endp

ifdef DYNAMIC
    ; get cubic root of number and return 32-bit fractional part as integer
cbr2int:
    pushad
    call   cbr_primes
cbr_primes:
    pop    esi
    lea    esi, [esi+(primes-cbr_primes)]
    push   1
    push   0
    fild   qword ptr[esp]   ; load 2^32
    push   1
    fild   dword ptr[esp]
    push   3
    fild   dword ptr[esp]
    fdivp                   ; 1.0/3.0
    movzx  eax, word ptr[esi+2*i]
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
    mov    [esp+1ch], eax
    popad
    ret

init_fpu:
    ; round numbers down
    push   eax
    fstcw  [esp]            ; store control word
    pop    eax
    or     ax, 00400H       ; set round down bit
    and    ax, 0F7FFH       ; clear bit
    push   eax
    fldcw  [esp]            ; load control word
    pop    eax
    ret
    
sqrt2int:
    ; get square root of number in eax 
    ; return 32-bit fractional part as integer
    pushad
    call   sqr_primes
sqr_primes:
    pop    esi
    lea    esi, [esi+(primes-sqr_primes)]
    push   1
    push   0
    fild   qword ptr[esp]   ; load 2^32
    movzx  eax, word ptr[esi+2*i]
    push   eax
    fild   dword ptr[esp]   ; load integer
    push   eax
    fsqrt
    fld1                    ; load 1 
    fsub                    ; subtract to get fractional part
    fmul                    ; multiply fractional part by 2^32
    frndint
    fistp  qword ptr[esp]
    pop    eax 
    add    esp, 3*4         ; release 2^32 on stack
    mov    [esp+1ch], eax
    popad
    ret

primes dw 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53
       dw 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131
       dw 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223 
       dw 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311
primes_len equ $-primes

else

_k dd 0428a2f98h, 071374491h, 0b5c0fbcfh, 0e9b5dba5h
   dd 03956c25bh, 059f111f1h, 0923f82a4h, 0ab1c5ed5h
   dd 0d807aa98h, 012835b01h, 0243185beh, 0550c7dc3h
   dd 072be5d74h, 080deb1feh, 09bdc06a7h, 0c19bf174h
   dd 0e49b69c1h, 0efbe4786h, 00fc19dc6h, 0240ca1cch
   dd 02de92c6fh, 04a7484aah, 05cb0a9dch, 076f988dah 
   dd 0983e5152h, 0a831c66dh, 0b00327c8h, 0bf597fc7h
   dd 0c6e00bf3h, 0d5a79147h, 006ca6351h, 014292967h 
   dd 027b70a85h, 02e1b2138h, 04d2c6dfch, 053380d13h
   dd 0650a7354h, 0766a0abbh, 081c2c92eh, 092722c85h 
   dd 0a2bfe8a1h, 0a81a664bh, 0c24b8b70h, 0c76c51a3h
   dd 0d192e819h, 0d6990624h, 0f40e3585h, 0106aa070h 
   dd 019a4c116h, 01e376c08h, 02748774ch, 034b0bcb5h
   dd 0391c0cb3h, 04ed8aa4ah, 05b9cca4fh, 0682e6ff3h 
   dd 0748f82eeh, 078a5636fh, 084c87814h, 08cc70208h
   dd 090befffah, 0a4506cebh, 0bef9a3f7h, 0c67178f2h
   
endif

    end