

; MD4 in x86 assembly
; Odzhan

.686
.model flat

option casemap:none
option prologue:none
option epilogue:none

include md4.inc

.code

; initialize MD4 context
  public MD4_Init
MD4_Init proc C
    pushad
    mov    edi, [esp+32+4]       ; context
    xor    eax, eax
    mov    [edi][MD4_CTX.len+0], eax
    mov    [edi][MD4_CTX.len+4], eax
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
    popad
    ret
MD4_Init endp
    
; update context
  public MD4_Update
MD4_Update proc C
    pushad
    mov    ecx, [esp+32+12]
    jecxz  exit_update
    xchg   eax, ecx
    
    ; get buffer index
    mov    ebx, [esp+32+4]    ; context
    mov    esi, [esp+32+8]    ; input

    ; idx = ctx->len & MD4_CBLOCK - 1;
    mov    edx, [ebx][MD4_CTX.len]
    and    edx, MD4_CBLOCK - 1
    
    ; limit of (2^32)-1 bytes each update
    ; ctx->len += len;
    add    [ebx][MD4_CTX.len+0], eax
    adc    [ebx][MD4_CTX.len+4], 0
    
    .while 1
      ; r = MIN(len, MD4_CBLOCK - idx);
      push   MD4_CBLOCK
      pop    ecx
      sub    ecx, edx
      cmp    ecx, eax
      cmovae ecx, eax
      ; memcpy ((void*)&ctx->buffer[idx], p, r);
      lea    edi, [ebx][MD4_CTX.buffer][edx]
      ; idx += r
      add    edx, ecx
      ; len -= r
      sub    eax, ecx
      rep    movsb
      ; if ((idx + r) < MD4_CBLOCK) break;
      .break .if edx < MD4_CBLOCK
      push   ebx
      call   MD4_Transform
      cdq
    .endw
exit_update:
    popad
    ret
MD4_Update endp

; finalize context
  public MD4_Final
MD4_Final proc C
    pushad
    mov    esi, [esp+32+8]   ; context
    mov    edi, [esp+32+4]   ; dgst
    
    ; uint64_t len=ctx->len & MD4_CBLOCK - 1;
    mov    ecx, [esi][MD4_CTX.len+0]
    and    ecx, MD4_CBLOCK - 1
    
    ; fill with zeros
    ; memset (&ctx->buffer.v8[len], 0, MD4_CBLOCK - len);
    pushad
    lea    edi, [esi][MD4_CTX.buffer][ecx]
    neg    ecx
    add    ecx, MD4_CBLOCK
    xor    eax, eax
    rep    stosb
    popad 
    
    ; ctx->buffer.v8[len] = 0x80;
    mov    byte ptr[esi][MD4_CTX.buffer][ecx], 80h
    
    ; if (len >= 56)
    cmp    ecx, 56
    .if !carry?
      push esi
      call MD4_Transform
      
      ; memset (ctx->buffer.v8, 0, MD4_CBLOCK);
      push edi
      lea  edi, [esi][MD4_CTX.buffer]
      push MD4_CBLOCK/4
      pop  ecx
      xor  eax, eax
      rep  stosd
      pop  edi
    .endif
    
    ; add total bits
    ; ctx->buffer.v64[7] = ctx->len * 8;
    mov    eax, [esi][MD4_CTX.len+0]
    mov    edx, [esi][MD4_CTX.len+4]
    shld   edx, eax, 3
    shl    eax, 3
    mov    dword ptr[esi][MD4_CTX.buffer+56], eax
    mov    dword ptr[esi][MD4_CTX.buffer+60], edx  
    
    ; compress
    ; MD4_Transform(ctx);
    push   esi    
    call   MD4_Transform
    
    ; copy digest to buffer
    ; memcpy (dgst, ctx->state.v8, MD4_DIGEST_LENGTH);
    push   MD4_LBLOCK
    pop    ecx
    rep    movsd
    popad
    ret    
MD4_Final endp

_a equ <eax>  ; don't change
_b equ <ebx>
_c equ <ebp>
_d equ <edx>

_i equ <esi>
t1 equ <edi>
t2 equ <ecx>

; update context with input
MD4_Transform proc stdcall
    pushad
    
    ; load context
    mov    esi, [esp+32+4]  ; ctx
    
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
    
    push   esi
    
    xor    _i, _i
    
    .repeat
      mov    t1, [esp]   
      movzx  t2, byte ptr idx[_i]     ; idx[i]
      add    _a, [t1+4*t2]            ; a += x[i]
      mov    t2, _i
      and    t2, 3                    ; i %= 4
      mov    t1, _c
      .if     _i < 16
        mov   cl, byte ptr rotf[t2]    ; rotf[i%4]
        xor   t1, _d
        and   t1, _b
        xor   t1, _d
      .elseif _i < 32
        mov   cl, byte ptr rotg[t2]    ; rotg[i%4]
        push  ecx
        mov   t2, _c
        or    t1, _d
        and   t1, _b
        and   t2, _d
        or    t1, t2
        pop   ecx
        add   t1, 05a827999h
      .else
        mov   cl, byte ptr roth[t2]    ; roth[i%4]
        xor   t1, _b
        xor   t1, _d
        add   t1, 06ed9eba1h
      .endif
      
      add    _a, t1
      rol    _a, cl
      
      mov    t1, _a
      mov    _a, _d
      mov    _d, _c
      mov    _c, _b
      mov    _b, t1
      
      inc    _i
    .until _i == 48
    
    pop    esi
    mov    edi, [esp+32+4]
    
    ; save context
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
    ; restore registers
    popad
    retn   4
MD4_Transform endp

rotf label dword
  db 3, 7, 11, 19
rotg label dword
  db 3, 5,  9, 13
roth label dword
  db 3, 9, 11, 15

idx  label dword
  db 0, 1, 2,  3, 4,  5, 6,  7, 8, 9, 10, 11, 12, 13, 14, 15
  db 0, 4, 8, 12, 1,  5, 9, 13, 2, 6, 10, 14,  3,  7, 11, 15
  db 0, 8, 4, 12, 2, 10, 6, 14, 1, 9,  5, 13,  3, 11,  7, 15
  
    end
