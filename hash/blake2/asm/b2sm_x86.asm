;  Copyright © 2015 Odzhan, Peter Ferrie. All Rights Reserved.
;
;  Redistribution and use in source and binary forms, with or without
;  modification, are permitted provided that the following conditions are
;  met:
;
;  1. Redistributions of source code must retain the above copyright
;  notice, this list of conditions and the following disclaimer.
;
;  2. Redistributions in binary form must reproduce the above copyright
;  notice, this list of conditions and the following disclaimer in the
;  documentation and/or other materials provided with the distribution.
;
;  3. The name of the author may not be used to endorse or promote products
;  derived from this software without specific prior written permission.
;
;  THIS SOFTWARE IS PROVIDED BY AUTHORS "AS IS" AND ANY EXPRESS OR
;  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
;  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
;  DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
;  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
;  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
;  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
;  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
;  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
;  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
;  POSSIBILITY OF SUCH DAMAGE.

; -----------------------------------------------
; blake2s in x86 assembly
;
; size: 509 byte ptrs
;
; global calls use cdecl convention
;
; -----------------------------------------------

    .686
    .model flat, C
    
    ifndef BIN
      public b2s_initx
      public b2s_updatex
      public b2s_finalx
      
      public _b2s_initx
      public _b2s_updatex
      public _b2s_finalx
    endif
    
pushad_t struct
  _edi dd ?
  _esi dd ?
  _ebp dd ?
  _esp dd ?
  _ebx dd ?
  _edx dd ?
  _ecx dd ?
  _eax dd ?
pushad_t ends

b2s_ctx struct
  state  db 32 dup (?)
  buffer db 64 dup (?)
  len    dd 2 dup (?)
  index  dd ?
  outlen dd ?
  rounds dd ?
b2s_ctx ends

; masm uses base 10 by default for AAM
; throws error if supplied anything else
aamx macro b:req
  db 0d4h, b
endm

_a  equ eax
_b  equ edx
_c  equ esi
_d  equ edi
x   equ edi

t0  equ ebp

  .code
  
; void b2s_g(blake2_blk *blk, uint16_t index, 
;    uint32_t x0, uint32_t x1)
b2s_g:
    pushad
    push   _a
    xchg   ah, al
    aamx   16
    
    movzx  ebp, ah
    movzx  _c, al

    pop    _a
    aamx   16
    
    movzx  _b, ah
    movzx  _a, al
    
    lea    _a, [x+_a*4]
    lea    _b, [x+_b*4]
    lea    _c, [x+_c*4]
    lea    _d, [x+ebp*4]
    ; load ecx with rotate values
    ; 16, 12, 8, 7
    mov    ecx, 07080C10h
    ; x[a] = PLUS(x[a],x[b]) + x0; 
    mov    t0, [esp+pushad_t._ebx]    ; x0
    add    t0, [_b]
q_l1:
    mov    bl, 1
q_l2:
    ; also x[c] = PLUS(x[c],x[d]);
    add    t0, [_a]
    mov    [_a], t0
    ; x[d] = ROTATE(XOR(x[d],x[a]),cl);
    ; also x[b] = ROTATE(XOR(x[b],x[c]),cl);
    xor    t0, [_d]
    ror    t0, cl
    mov    [_d], t0
    xchg   cl, ch
    xchg   _c, _a
    xchg   _d, _b
    inc    ebx
    jpo    q_l2
    ; x[a] = PLUS(x[a],x[b]) + x1; 
    add    t0, [esp+pushad_t._ecx]     ; x1
    ; --------------------------------------------
    shr    ecx, 16
    jnz    q_l1

    popad
    ret
    
; void b2s_compress (b2s_ctx *ctx, int last)
_b2s_compressx:
    pushad
    mov    esi, edi
    mov    ebx, esi
    ; create space for v + m
    sub    esp, 124
    push   eax
    ; initialize v with state and chaining variables
    mov    edi, esp          ; edi = v
    mov    eax, b2s_iv 

b2t_l1:                      ; first is ctx->state
    push   32                ; move 32 byte ptrs
    pop    ecx
                             ; then b2s_iv
    rep    movsb
    cmc                      ; complement
    xchg   esi, eax
    jc     b2t_l1            ; continue if carry
    
    ; esi should now be ctx->buffer
    ; edi will be m
    ; copy buffer into m
    mov    cl, 64
    push   edi
    rep    movsb
    pop    edi
    
    ; esi now points to ctx->len
    ; xor v with current length
    lodsd
    xor    [edi+12*4-64], eax
    lodsd
    xor    [edi+13*4-64], eax
    
    ; if this is last block, invert word 14
    ; 1 becomes -1, 0 remains 0
    neg    edx
    xor    [edi+14*4-64], edx
    
    ; do minimum of 10 rounds
    mov    ecx, [esi+8]
b2t_l2:
    xor    esi, esi
b2t_l3:
    cmp    esi, 10
    je     b2t_l2
    xor    ebp, ebp
    mov    edx, [b2s_sigma64+8*esi+4]
    mov    eax, [b2s_sigma64+8*esi]
b2t_l4:
    pushad
    aamx   16
    mov    cl, ah
    movzx  eax, al
    mov    ebx, [edi+4*eax]              ; m.v32[x0]
    mov    ecx, [edi+4*ecx]              ; m.v32[x1]
    mov    eax, dword ptr[ebp*2+b2s_idx16]   ; b2s_idx16[j]
    mov    edi, [edi+pushad_t._esp-96]
    call   b2s_g
    popad
    shrd   eax, edx, 8
    shr    edx, 8
    
    inc    ebp
    cmp    ebp, 8
    jnz    b2t_l4
    
    ; check rnds
    inc    esi
    loop   b2t_l3
    
    ; update state with work vector
b2t_l5:
    mov    eax, [edi+4*ebp-68]
    xor    eax, [edi+4*ebp+32-68]
    xor    [ebx+4*ebp-4], eax
    dec    ebp
    jnz    b2t_l5
    
    add    esp, 124
    pop    eax
    popad
    ret
    
; void b2s_init (b2s_ctx *ctx, uint32_t outlen, 
;   void *key, uint32_t keylen, uint32_t rnds)
_b2s_initx:
b2s_initx:
    pushad

    mov    ebp, esp    
    mov    edi, [ebp+32+ 4] ; ctx
    mov    ebx, [ebp+32+ 8] ; outlen
    mov    edx, [ebp+32+16] ; keylen
    
    ; initialize state
    mov    esi, b2s_iv
    lodsd
    ; eax ^= outlen
    xor    eax, ebx
    ; keylen << 8
    shl    edx, 8
    ; eax ^= keylen
    xor    eax, edx
    ; eax ^= 0x01010000
    xor    eax, 001010000h
    ; ctx->state.v32[0] = eax
    stosd
    push   7
    pop    ecx
    rep    movsd
    
    ; edi now points to ctx->buffer
    ; zero initialize
    ; also ctx->len.v64 = 0
    xor    eax, eax
    mov    cl, 19
    push   edi
    rep    stosd
    
    ; edi now points to index
    mov    ecx, [ebp+32+16] ; keylen
    jecxz  b2_kl
    mov    byte ptr [edi-4], 64
b2_kl:
    ; ctx->outlen = outlen
    xchg   eax, ebx
    stosd
    ; ctx->rounds = rnds
    mov    eax, [ebp+32+20]
    ; minimum of 10
    mov    bl, 10
    cmp    eax, ebx
    cmovb  eax, ebx
    stosd
    
    mov    esi, [ebp+32+12] ; key
    pop    edi
    rep    movsb

    popad
    ret
 
; void b2s_update (b2s_ctx *ctx, 
;   void *input, uint32_t len)
_b2s_updatex:
b2s_updatex:
    pushad
    lea    esi, [esp+32+4]
    lodsd
    xchg   edi, eax           ; ctx
    lodsd
    xchg   ecx, eax
    lodsd
    xchg   ecx, eax           ; len
    jecxz  ex_upd
    xchg   esi, eax           ; input
    mov    edx, [edi+b2s_ctx.index] ; index
b2_upd:
    cmp    edx, 64
    jnz    b2_ab
    ; ctx->len.v64 += index
    add    dword ptr[edi+b2s_ctx.len+0], edx   
    mov    dl, 0              ; last=0
    adc    dword ptr[edi+b2s_ctx.len+4], edx
    call   _b2s_compressx
b2_ab:
    lodsb
    mov    [edi+edx+b2s_ctx.buffer], al
    inc    edx
    loop   b2_upd
    mov    [edi+b2s_ctx.index], edx
ex_upd:
    popad
    ret 
    
; void b2s_final (void* out, b2s_ctx *ctx)
_b2s_finalx:
b2s_finalx:
    pushad
    mov    esi, [esp+32+4]    ; out
    mov    edx, [esp+32+8]    ; ctx
    mov    ecx, [edx+b2s_ctx.index]   ; index
    xor    eax, eax
    add    dword ptr[edx+b2s_ctx.len+0], ecx
    adc    dword ptr[edx+b2s_ctx.len+4], eax
    lea    edi, [edx+ecx+b2s_ctx.buffer]
    neg    ecx
    add    ecx, 64
    rep    stosb
    xchg   edx, eax
    xchg   edi, eax
    inc    edx                 ; last=1
    call   _b2s_compressx
    mov    ecx, [edi+b2s_ctx.outlen] ; outlen
    xchg   esi, edi
    rep    movsb
    popad
    ret
    
b2s_iv:
    dd 06A09E667h, 0BB67AE85h, 03C6EF372h, 0A54FF53Ah
    dd 0510E527Fh, 09B05688Ch, 01F83D9ABh, 05BE0CD19h

b2s_idx16:
    dw 0C840h, 0D951h, 0EA62h, 0FB73h
    dw 0FA50h, 0CB61h, 0D872h, 0E943h

b2s_sigma64:
    dq 0fedcba9876543210h, 0357b20c16df984aeh 
    dq 0491763eadf250c8bh, 08f04a562ebcd1397h 
    dq 0d386cb1efa427509h, 091ef57d438b0a6c2h 
    dq 0b8293670a4def15ch, 0a2684f05931ce7bdh 
    dq 05a417d2c803b9ef6h, 00dc3e9bf5167482ah

    end