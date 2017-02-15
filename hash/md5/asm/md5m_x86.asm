

; MD5 in x86 assembly
; Odzhan

.686
.model flat

option casemap:none
option prologue:none
option epilogue:none

include md5.inc

.code

; initialize MD5 context
  public MD5_Init
MD5_Init proc C
    pushad
    mov    edi, [esp+32+4]     ; context
    xor    eax, eax
    mov    [edi][MD5_CTX.len+0], eax
    mov    [edi][MD5_CTX.len+4], eax
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
MD5_Init endp
    
; update context
  public MD5_Update
MD5_Update proc C
    pushad
    ; get buffer index
    mov    ebx, [esp+32+4]    ; context
    mov    esi, [esp+32+8]    ; input

    ; idx = ctx->len & MD5_CBLOCK - 1;
    mov    edx, [ebx][MD5_CTX.len]
    and    edx, MD5_CBLOCK - 1
    
    ; limit of (2^32)-1 bytes each update
    ; ctx->len += len;
    mov    eax, [esp+32+12]
    add    [ebx][MD5_CTX.len+0], eax
    adc    [ebx][MD5_CTX.len+4], 0
    
    .while 1
      ; r = MIN(len, MD5_CBLOCK - idx);
      push   MD5_CBLOCK
      pop    ecx
      sub    ecx, edx
      cmp    ecx, eax
      cmovae ecx, eax
      ; memcpy ((void*)&ctx->buffer[idx], p, r);
      lea    edi, [ebx][MD5_CTX.buffer][edx]
      ; idx += r
      add    edx, ecx
      ; len -= r
      sub    eax, ecx
      rep    movsb
      ; if ((idx + r) < MD5_CBLOCK) break;
      .break .if edx < MD5_CBLOCK
      push   ebx
      call   MD5_Transform
      xor    edx, edx
    .endw
    popad
    ret
MD5_Update endp

; finalize context
  public MD5_Final
MD5_Final proc C
    pushad
    mov    esi, [esp+32+8]   ; context
    mov    edi, [esp+32+4]   ; dgst
    
    ; uint64_t len=ctx->len & MD5_CBLOCK - 1;
    mov    ecx, [esi][MD5_CTX.len+0]
    and    ecx, MD5_CBLOCK - 1
    
    ; fill with zeros
    ; memset (&ctx->buffer.v8[len], 0, MD5_CBLOCK - len);
    pushad
    lea    edi, [esi][MD5_CTX.buffer][ecx]
    neg    ecx
    add    ecx, MD5_CBLOCK
    xor    eax, eax
    rep    stosb
    popad 
    
    ; ctx->buffer.v8[len] = 0x80;
    mov    byte ptr[esi][MD5_CTX.buffer][ecx], 80h
    
    ; if (len >= 56)
    cmp    ecx, 56
    .if !carry?
      push esi
      call MD5_Transform
      
      ; memset (ctx->buffer.v8, 0, MD5_CBLOCK);
      push edi
      lea  edi, [esi][MD5_CTX.buffer]
      push MD5_CBLOCK/4
      pop  ecx
      xor  eax, eax
      rep  stosd
      pop  edi
    .endif
    
    ; add total bits
    ; ctx->buffer.v64[7] = ctx->len * 8;
    mov    eax, [esi][MD5_CTX.len+0]
    mov    edx, [esi][MD5_CTX.len+4]
    shld   edx, eax, 3
    shl    eax, 3
    mov    dword ptr[esi][MD5_CTX.buffer+56], eax
    mov    dword ptr[esi][MD5_CTX.buffer+60], edx  
    
    ; compress
    ; MD5_Transform(ctx);
    push   esi    
    call   MD5_Transform
    
    ; copy digest to buffer
    ; memcpy (dgst, ctx->state.v8, MD5_DIGEST_LENGTH);
    push   MD5_LBLOCK
    pop    ecx
    rep    movsd
    popad
    ret  
MD5_Final endp

_a equ <eax>  ; don't change
_b equ <ebx>
_c equ <ebp>
_d equ <edx>

_i equ <esi>
t1 equ <edi>
t2 equ <ecx>

rotf label dword
  db 7,12,17,22
rotg label dword
  db 5, 9,14,20
roth label dword
  db 4,11,16,23
roti label dword
  db 6,10,15,21
idx  label dword
  db 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
  db 1,6,11,0,5,10,15,4,9,14,3,8,13,2,7,12
  db 5,8,11,14,1,4,7,10,13,0,3,6,9,12,15,2
  db 0,7,14,5,12,3,10,1,8,15,6,13,4,11,2,9

ifndef DYNAMIC

tc label dword
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
    push   eax
    fstcw  [esp]            ; store control word
    pop    eax
    or     ax, 00400H       ; set round down bit
    and    ax, 0F7FFH       ; clear bit
    push   eax
    fldcw  [esp]            ; load control word
    pop    eax
    ret
init_fpu endp

; get sine of i+1 and return 32-bit fractional part as integer
sin2int proc
    push   1
    push   0
    fild   qword ptr[esp]   ; load 2^32 or 4294967296UL
    inc    _i
    push   _i
    dec    _i
    fild   dword ptr[esp]
    fsin              ; fsin (integer)
    fabs
    fmul              ; fabs(fsin (integer)) * 4294967296UL
    fistp  qword ptr[esp]
    pop    t1
    add    esp, 2*4
    ret
sin2int endp

endif

; update context with input
MD5_Transform proc
    pushad
ifdef DYNAMIC
    ; set control word to round down numbers
    call   init_fpu
endif
    ; load context
    mov    esi, [esp+32+4]  ; ctx
    
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
    
    push   esi         ; x
    
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
        xor   t1, _b
        and   t1, _d
        xor   t1, _c
      .elseif _i < 48
        mov   cl, byte ptr roth[t2]    ; roth[i%4]
        xor   t1, _d
        xor   t1, _b
      .else
        mov   cl, byte ptr roti[t2]
        mov   t1, _d
        xor   t1, -1
        or    t1, _b
        xor   t1, _c
      .endif
      
      add    _a, t1
ifdef DYNAMIC
      call   sin2int
else
      mov    t1, dword ptr tc[_i*4]
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
    .until _i == MD5_CBLOCK
    
    pop    esi
    
    ; restore registers
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
MD5_Transform endp

    end
