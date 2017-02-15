;
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
;
; -----------------------------------------------
; ChaCha20 stream cipher in x86 assembly
;
; size: 241 bytes
;
; global calls use cdecl convention
;
; -----------------------------------------------

    bits 32
    
    %ifndef BIN
      global _cc20_setkeyx
      global _cc20_encryptx
    %endif
    
struc cc20_ctx
  s    resb 64
  .size:
endstruc

; void cc_setkey(chacha_ctx *ctx, void *key, void *iv)
_cc20_setkeyx:
cc20_setkeyx:
    pushad
    mov    edi, [esp+32+4] ; ctx
    ; copy "expand 32-byte k" into state
    call   load_iv
    dd     061707865h, 03320646Eh
    dd     079622D32h, 06B206574h
load_iv:
    pop    esi
    push   4
    pop    ecx
    rep    movsd
    ; copy 256-bit key
    mov    esi, [esp+32+8] ; key
    mov    cl, 8
    rep    movsd
    ; set 64-bit block counter to zero
    xchg   ecx, eax
    stosd
    stosd
    ; store 64-bit nonce
    mov    esi, [esp+32+12] ; iv
    movsd
    movsd
    popad
    ret
    
%define a eax
%define b edx
%define c esi
%define d edi
%define x edi

%define t0 ebp

; void QUARTERROUND(chacha_blk *blk, uint16_t index)
QUARTERROUND:
    pushad
    push   a
    xchg   ah, al
    aam    16
    
    movzx  ebp, ah
    movzx  c, al

    pop    a
    aam    16
    
    movzx  b, ah
    movzx  a, al

    lea    a, [x+a*4]
    lea    b, [x+b*4]
    lea    c, [x+c*4]
    lea    d, [x+ebp*4]
    ; load ecx with rotate values
    ; 16, 12, 8, 7
    mov    ecx, 07080C10h
    mov    t0, [b]
q_l1:
    xor    ebx, ebx
q_l2:
    ; x[a] = PLUS(x[a],x[b]); 
    add    t0, [a]
    mov    [a], t0
    ; x[d] = ROTATE(XOR(x[d],x[a]),cl);
    ; also x[b] = ROTATE(XOR(x[b],x[c]),cl);
    xor    t0, [d]
    rol    t0, cl
    mov    [d], t0
    xchg   cl, ch
    xchg   c, a
    xchg   d, b
    dec    ebx
    jp     q_l2
    ; --------------------------------------------
    shr    ecx, 16
    jnz    q_l1
    
    popad
    ret
  
; void cc_corex (chacha_ctx *ctx, void *in, uint32_t len)
; do not call directly
; expects state in ebx, length in eax, input in edx
_cc20_corex:
cc20_corex:
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
e_l1:
    ; load indexes
    call   load_idx
    dw     0c840H, 0d951H
    dw     0ea62H, 0fb73H
    dw     0fa50H, 0cb61H
    dw     0d872H, 0e943H
load_idx:
    pop    esi  ; pointer to indexes
    mov    cl, 8
e_l2:
    lodsw
    call   QUARTERROUND
    loop   e_l2
    dec    ebp
    jnz    e_l1
    
    ; add state to x
    mov    cl, 16
add_state:
    mov    eax, [ebx+ecx*4-4]
    add    [edi+ecx*4-4], eax
    loop   add_state

    ; xor input with x
    pop    ecx               ; ecx=len
xor_input:
    mov    al, byte[edi+ecx-1]
    xor    byte[edx+ecx-1], al
    loop   xor_input
    
    ; update block counter
    stc
    adc    dword[ebx+12*4], ecx
    adc    dword[ebx+13*4], ecx
    
    ; free stack
    popad
    popad
    ; restore registers
    popad
    ret
    
_cc20_encryptx:
cc20_encryptx:
    pushad
    lea    esi, [esp+32+4]
    lodsd
    xchg   ebx, eax
    lodsd
    xchg   edx, eax
    lodsd
    xchg   ecx, eax
    jecxz  cc_l1             ; exit if len==0
    xor    eax, eax          ; r=0
cc_l0:
    mov    al, 64
    cmp    ecx, eax          ; eax=len>64:64:len
    cmovb  eax, ecx
    call   cc20_corex
    add    edx, eax          ; p += r;
    sub    ecx, eax          ; len -= r;
    jnz    cc_l0
cc_l1:
    popad
    ret
    