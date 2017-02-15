;
;  Copyright © 2017 Odzhan, Peter Ferrie. All Rights Reserved.
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
; Threefish-256 block cipher in x86 assembly
;
; size: 371 bytes
;
; global calls use cdecl convention
;
; -----------------------------------------------

    bits 32

    %ifndef BIN
      global _threefish_setkeyx
      global _threefish_encryptx
    %endif
    
struc pushad_t
  _edi dd ?
  _esi dd ?
  _ebp dd ?
  _esp dd ?
  _ebx dd ?
  _edx dd ?
  _ecx dd ?
  _eax dd ?
  .size:
endstruc
    
%define t0 mm0
%define t1 mm1
    
%define x0 mm0
%define x1 mm1
%define x2 mm2
%define x3 mm3
%define x4 mm4
%define x5 mm5
%define x6 mm6
%define x7 mm7
    
threefish_setkeyx:
_threefish_setkeyx:
    pushad
    lea    esi, [esp+32+4]
    lodsd
    xchg   eax, edi          ; edi = ctx   
    lodsd
    xchg   eax, esi          ; esi = key
    push   edi               ; save ctx 
    
    ; memcpy((void*)c->k, key,   32);
    push   8
    pop    ecx
    rep    movsd
    scasd
    scasd
    
    ; memcpy((void*)c->t, tweak, 16);
    xchg   eax, esi
    lodsd
    xchg   eax, esi
    mov    cl, 4
    rep    movsd

    pop    edi 
    ; c->k[4] = 0x1BD11BDAA9FC1A22ULL;
    mov    eax, 0xA9FC1A22
    mov    ebx, 0x1BD11BDA
    mov    cl, 4    
tf_sk:
    xor    eax, [edi]
    scasd
    xor    ebx, [edi]
    scasd
    loop   tf_sk
    
    stosd
    xchg   eax, ebx
    stosd
    movq   t0, [edi]
    pxor   t0, [edi+8]
    movq   [edi+16], t0
    popad
    ret

rotl64:
    movd   x5, edi
    neg    edi
    add    edi, 64    
    movd   x6, edi
    movq   x7, x1
    psllq  x7, x5
    psrlq  x1, x6
    por    x1, x7
    ret
    
; void mix(
;    void *data, 
;    uint8_t rc[], 
;    int rnd, int enc)
mix:
    pushad
    xor    eax, eax
m_l0:    
    movq   x0, [esi+eax*8]
    movq   x1, [esi+eax*8+8]
    
    ; r = rc[(rnd & 7) + (i << 2)];
    mov    edi, edx
    and    edi, 7
    lea    edi, [edi+eax*4] 
    movzx  edi, byte[ebp+edi]
    jecxz  m_l1
    
    sub    edi, 64
    neg    edi
    
    pxor   x1, x0
    call   rotl64
    psubq  x0, x1
    jmp    m_l2
m_l1    
    paddq  x0, x1
    call   rotl64
    pxor   x1, x0
m_l2:
    movq   [esi+eax*8  ], x0    
    movq   [esi+eax*8+8], x1    
    add    al, 2
    cmp    al, 4
    jnz    m_l0
    popad
    ret
    
; void permute(void *data)    
permute:
    movq   x0, [esi+1*8]
    movq   x1, [esi+3*8]

    movq   [esi+1*8], x1 
    movq   [esi+3*8], x0 
    ret
    
; void addkey(const threefish_ctx_t *c, 
;    void *data, uint8_t s, 
;    uint64_t enc)
addkey:
    pushad

    pxor    x3, x3           ; x3 = 0 
    jecxz   ak_l0
    
    pcmpeqb x3, x3           ; x3 = ~0
    dec     ecx              ; ecx = 0
ak_l0:  
    push    5
    pop     ebp  
    
    ; x0 = x[i];
    movq    x0, [esi+ecx*8]   
    
    ; x1 = c->k[(s + i) % 5];
    lea     eax, [ebx+ecx]    
    cdq    
    idiv    ebp        
    movq    x1, [edi+edx*8]
    
    dec     ebp
    dec     ebp    
    
    pxor    x2, x2    
    jecxz   ak_lx
    
    ; if (i==3) x2 = s;
    movd    x2, ebx
    cmp     cl, 3
    je      ak_lx

    mov     eax, ebx
    cdq
    cmp     cl, 1
    je      ak_lxx
    inc     eax
ak_lxx:    
    idiv    ebp
    
    ; if (i==1) x2 = c->t[s % 3];
    ; if (i==2) x2 = c->t[(s+1) % 3];    
    movq    x2, [edi+8*edx+40]
ak_lx:    
    ; x[i] = ((x0 ^ -enc) + x1 + x2) ^ -enc;
    pxor    x0, x3
    paddq   x0, x1
    paddq   x0, x2
    pxor    x0, x3
    
    movq    [esi+ecx*8], x0
    
    inc     ecx
    cmp     cl, 4
    jnz     ak_l0    
    popad
    ret
    
; void threefish_encrypt(
;    const threefish_ctx_t *c, 
;    void *data, 
;    uint32_t enc)
threefish_encryptx:
_threefish_encryptx
    pushad
    lea    esi, [esp+32+4]
    lodsd
    xchg   edi, eax    
    lodsd
    push   eax
    lodsd
    cdq
    xchg   eax, ecx
    xor    ebx, ebx
    pop    esi
    push   1
    pop    eax
    ; load rotation constants
    push   0x20160C21
    push   0x25283910
    push   0x203A2E19
    push   0x0517340E
    mov    ebp, esp
    jecxz  tf_l1
    
    neg    eax               ; ofs = -1
    mov    bl, 18            ; s   = 18
    pushad
    mov    esi, ebp
    mov    edi, ebp
tf_l0:    
    lodsd
    xchg   eax, ebx
    lodsd
    bswap  eax
    stosd
    xchg   eax, ebx
    bswap  eax
    stosd
    dec    edx
    jp     tf_l0
    popad
    ; apply 72 rounds
tf_l1:    
    ; add key every 4 rounds
    test  dl, 3
    jnz   tf_l2
    call  addkey
    add   ebx, eax
tf_l2:    
    ; permute if decrypting
    jecxz tf_l3    
    call  permute
tf_l3:    
    ; apply mixing function
    call  mix
    ; permute if encrypting
    test  ecx, ecx
    jnz   tf_l4
    call  permute
tf_l4:    
    inc   edx                 
    cmp   dl, 72
    jnz   tf_l1 
    ; add key
    call  addkey
    add   esp, 16
    popad
    ret

    
    
    