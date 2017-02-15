;
;  Copyright © 2016 Odzhan, Peter Ferrie. All Rights Reserved.
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
;
; -----------------------------------------------
; Salsa20 stream cipher in x86 assembly
;
; size: 245 bytes
;
; global calls use cdecl convention
;
; -----------------------------------------------

    bits 32
    
    %ifndef BIN
      global s20_setkeyx
      global _s20_setkeyx
      
      global s20_cryptx
      global _s20_cryptx
    %endif
    
struc s20_ctx
  state    resb 64
  .size:
endstruc

; void s20_setkey(s20_ctx *ctx, void *key, void *iv)
_s20_setkeyx:
s20_setkeyx:
    pushad
    lea    esi, [esp+32+4]
    lodsd
    xchg   edi, eax
    lodsd
    push   eax
    lodsd
    xchg   ebx, eax
    pop    esi
    mov    eax, 061707865h
    cdq
    stosd
    ; copy 16 bytes of 256-bit key
    movsd    
    movsd    
    movsd    
    movsd    
    mov    eax, 03320646Eh
    stosd
    ; copy 64-bit iv to 6,7
    xchg   esi, ebx
    movsd
    movsd
    ; zero 64-bit counter at 8,9
    xchg   eax, edx
    stosd
    stosd
    ; store 32-bits at 10
    mov    eax, 079622D32h
    stosd
    ; store remainder of key at 11-14
    xchg   esi, ebx
    movsd    
    movsd    
    movsd    
    movsd
    ; store last 32-bits
    mov    eax, 06B206574h
    stosd
    popad
    ret
    
%define a eax
%define b ebx
%define c edx
%define d esi
%define x edi

%define t0 ebp

; void QUARTERROUND(s20_blk *blk, uint16_t index)
; expects edi points to x array
QUARTERROUND:
    pushad
    push   a                ; save eax
    xchg   ah, al
    aam    16
    
    movzx  d, ah
    movzx  c, al

    pop    a
    aam    16
    
    movzx  b, ah
    movzx  a, al

    lea    a, [x+a*4]
    lea    b, [x+b*4]
    lea    c, [x+c*4]
    lea    d, [x+d*4]

    ; load ecx with rotate values
    ; 16, 12, 8, 7
    mov    ecx, 0120D0907h
s20_q0:
    ; x[b] ^= ROTL32((x[a] + x[d]), 7);
    mov    t0, [a]           ; ebp=x[a]
    add    t0, [d]
    rol    t0, cl
    xor    [b], t0
    
    xchg   cl, ch
    
    mov    t0, [b]           ; ebp=x[b]
    add    t0, [a]
    rol    t0, cl
    xor    [c], t0
    
    xchg   a, c
    xchg   b, d
    
    shr    ecx, 16
    jnz    s20_q0
    
    popad
    ret
  
; void s20_corex (s20_ctx *ctx, void *in, uint32_t len)
; do not call directly
; expects state in ebx, length in eax, input in edx
_s20_corex:
s20_corex:
    pushad

    ; allocate 64-bytes local space, x
    push   64
    pop    ecx
    sub    esp, ecx
    
    ; copy state to x
    mov    edi, esp
    mov    esi, ebx
    rep    movsb

    ; move x into edi
    mov    edi, esp
    push   eax
    push   20/2  ; 20 rounds
    pop    ebp
s20_c0:
    ; load indexes
    call   s20_c1
    dw 0C840h, 01D95h, 062EAh, 0B73Fh
    dw 03210h, 04765h, 098BAh, 0EDCFh
s20_c1:
    pop    esi  ; pointer to indexes
    mov    cl, 8
s20_c2:
    lodsw
    call   QUARTERROUND
    loop   s20_c2
    dec    ebp
    jnz    s20_c0
    
    ; add state to x
    mov    cl, 16
s20_c3:
    mov    eax, [ebx+ecx*4-4]
    add    [edi+ecx*4-4], eax
    loop   s20_c3

    ; xor input with x
    pop    ecx               ; ecx=len
s20_c4:
    mov    al, byte[edi+ecx-1]
    xor    byte[edx+ecx-1], al
    loop   s20_c4
    
    ; update block counter
    stc
    adc    dword[ebx+8*4], ecx
    adc    dword[ebx+9*4], ecx
    
    ; free stack
    popad
    popad
    ; restore registers
    popad
    ret
    
_s20_cryptx:
s20_cryptx:
    pushad
    lea    esi, [esp+32+4]
    lodsd
    xchg   ebx, eax
    lodsd
    xchg   edx, eax
    lodsd
    xchg   ecx, eax
    jecxz  s20_l1            ; exit if len==0
    xor    eax, eax          ; r=0
s20_l0:
    mov    al, 64
    cmp    ecx, eax          ; eax=len < 64 ? len : 64
    cmovb  eax, ecx
    call   s20_corex
    add    edx, eax          ; p += r;
    sub    ecx, eax          ; len -= r;
    jnz    s20_l0
s20_l1:
    popad
    ret
    