

; SHA-1 in x86 assembly
; Odzhan

.686
.model flat

option casemap:none
option prologue:none
option epilogue:none

include sha.inc

.code

; initialize SHA-1 context
  public SHA1_Init
SHA1_Init proc C
    pushad
    mov    edi, [esp+32+4]
    xor    eax, eax
    mov    [edi][SHA_CTX.len+0], eax
    mov    [edi][SHA_CTX.len+4], eax
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
    popad
    ret
SHA1_Init endp
    
; update context
  public SHA1_Update
SHA1_Update proc C
    pushad
    ; get buffer index
    mov    ebx, [esp+32+4] ; context
    mov    esi, [esp+32+8] ; data

    ; idx = ctx->len & SHA_CBLOCK - 1;
    mov    edx, [ebx][SHA_CTX.len+0]
    and    edx, SHA_CBLOCK - 1
    
    ; limit of (2^32)-1 bytes each update
    ; ctx->len += len;
    mov    eax, [esp+32+12]   ; len
    add    [ebx][SHA_CTX.len+0], eax
    adc    [ebx][SHA_CTX.len+4], 0
    
    .while 1
      push   ecx
      ; r = MIN(len, SHA_CBLOCK - idx);
      push   SHA_CBLOCK
      pop    ecx
      sub    ecx, edx
      cmp    ecx, eax
      cmovae ecx, eax
      ; memcpy ((void*)&ctx->buffer[idx], p, r);
      lea    edi, [ebx][SHA_CTX.buffer][edx]
      ; idx += r
      add    edx, ecx
      ; len -= r
      sub    eax, ecx
      rep    movsb
      pop    ecx
      ; if ((idx + r) < SHA_CBLOCK) break;
      .break .if edx < SHA_CBLOCK
      push   ebx
      call   SHA1_Transform
      xor    edx, edx
    .endw
    popad
    ret
SHA1_Update endp

; finalize context
  public SHA1_Final
SHA1_Final proc C
    pushad
    mov    esi, [esp+32+8]    ; context
    mov    edi, [esp+32+4]    ; out
    
    ; uint64_t len=ctx->len & SHA_CBLOCK - 1;
    mov    eax, [esi][SHA_CTX.len+0]
    mov    edx, [esi][SHA_CTX.len+4]
    and    eax, SHA_CBLOCK - 1
    
    ; fill with zeros
    ; memset (&ctx->buffer.v8[len], 0, SHA1_CBLOCK - len);
    pushad
    xchg   eax, ecx
    lea    edi, [esi][SHA_CTX.buffer][ecx]
    neg    ecx
    add    ecx, SHA_CBLOCK
    xor    eax, eax
    rep    stosb
    popad
    
    ; ctx->buffer[len] = 0x80;
    mov    byte ptr[esi][SHA_CTX.buffer][eax], 80h
    ; if (len >= 56)
    cmp    eax, 56
    .if !carry?
      push esi
      call SHA1_Transform
      
      ; memset (ctx->buffer.v8, 0, SHA1_CBLOCK);
      push edi
      lea  edi, [esi][SHA_CTX.buffer]
      push SHA_CBLOCK/4
      pop  ecx
      xor  eax, eax
      rep  stosd
      pop  edi
    .endif
    
    ; ctx->bitlen = SWAP64(ctx->len * 8);
    mov    eax, [esi][SHA_CTX.len+0]
    mov    edx, [esi][SHA_CTX.len+4]
    shld   edx, eax, 3
    shl    eax, 3
    bswap  eax
    bswap  edx
    mov    dword ptr[esi][SHA_CTX.buffer+56], edx
    mov    dword ptr[esi][SHA_CTX.buffer+60], eax
    
    push   esi
    call   SHA1_Transform
    
    push   SHA_LBLOCK
    pop    ecx
swap_words:
    lodsd
    bswap  eax
    stosd
    loop   swap_words
    popad
    ret    
SHA1_Final endp

; workspace for transform
SHA1_WS struct
  w dword 80 dup (?)
  i dword ?
SHA1_WS ends

_a equ <eax>  ; don't change
_b equ <ebx>
_c equ <ebp>
_d equ <edx>
_e equ <edi>

_i textequ <[esp][SHA1_WS.i]>
_w textequ <[esp][SHA1_WS.w]>

t1 equ <esi>
t2 equ <ecx>

; update context with input
  public SHA1_Transform
SHA1_Transform proc
    pushad
    
    mov    ebx, [esp+32+4]
    lea    esi, [ebx][SHA_CTX.buffer]
    
    ; allocate space for work
    sub    esp, sizeof SHA1_WS
    mov    edi, esp
    
    ; convert 16 words to big endian
    push   16
    pop    ecx
swap_bytes:
    lodsd
    bswap  eax
    stosd
    loop   swap_bytes
    
    mov    _i, ecx
    
    ; expand 16 words to 80
    mov    cl, 64
expand:
    mov    eax, [edi -  3*4]
    xor    eax, [edi -  8*4]
    xor    eax, [edi - 14*4]
    xor    eax, [edi - 16*4]
    rol    eax, 1
    stosd
    loop   expand
    
    ; load context
    mov    esi, ebx
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
    
    ; update context
    .repeat
        mov	 t1, _c
      .if (_i < 20)
        xor	 t1, _d
        and	 t1, _b
        xor	 t1, _d
        add  t1, 05a827999h
      .elseif (_i < 40)
        xor	 t1, _d
        xor	 t1, _b
        add  t1, 06ed9eba1h
      .elseif (_i < 60)
        mov  t2, _c
        or   t1, _d
        and  t1, _b
        and  t2, _d
        or   t1, t2
        add  t1, 08f1bbcdch
      .else
        xor	 t1, _d
        xor	 t1, _b
        add  t1, 0ca62c1d6h
      .endif
      ; t += ROL32(a, 5) + e
      mov   t2, _a
      rol   t2, 5
      add   t2, _e
      add   t1, t2
      mov   t2, _i
      shl   t2, 2
      add   t1, [esp][SHA1_WS.w+t2]
      
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
    .until  _i == 80
    
    add    esp, sizeof SHA1_WS
    
    mov    esi, _e
    mov    edi, [esp+32+4]
    
    ; ctx->state.v32[0] += a;
    add    [edi], _a
    scasd
    ; ctx->state.v32[1] += b;
    add    [edi], _b
    scasd
    ; ctx->state.v32[2] += c;
    add    [edi], _c
    scasd
    ; ctx->state.v32[3] += d;
    add    [edi], _d
    scasd
    ; ctx->state.v32[4] += e;
    add    [edi], esi
    ; restore registers
    popad
    retn   4
SHA1_Transform endp

    end
