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

; -----------------------------------------------
; rc5 in x86 assembly
;
; https://people.csail.mit.edu/rivest/Rivest-rc5rev.pdf
;
; size: 167 bytes
;
; global calls use cdecl convention
;
; -----------------------------------------------

  bits 32
  
%ifndef BIN
  global _rc5_setkeyx
  global _rc5_cryptx
%endif

%define RC5_ENCRYPT 1
%define RC5_DECRYPT 0

%define RC5_BLK_LEN 8
%define RC5_ROUNDS  12
%define RC5_KEYLEN  16

%define RC5_KR      (2*(RC5_ROUNDS+1))
%define RC5_P       0xB7E15163
%define RC5_Q       0x9E3779B9

_rc5_setkeyx:
rc5_setkey:
    pushad
    mov    eax, [esp+32+4] ; rc5 ctx
    mov    esi, [esp+32+8] ; key
    push   16
    pop    ecx
    sub    esp, ecx
    mov    edi, esp
    rep    movsb

    xchg   esi, eax
    mov    edi, esi
    
    mov    eax, RC5_P
    mov    cl, RC5_KR
sk_l1:
    stosd
    add    eax, RC5_Q
    loop   sk_l1
    
    xor    ebp, ebp    ; j=0
    xchg   ecx, eax    ; A=0
    cdq                ; B=0
    mov    ch, (-RC5_KR*3) & 255

sk_l2:
    xor    edi, edi    ; i=0
sk_l3:
    cmp    edi, RC5_KR
    je     sk_l2
    
    ; A = key->x[i] = ROTL32(key->x[i] + A+B, 3); 
    add    eax, edx
    add    eax, [esi+edi*4]
    rol    eax, 3
    mov    [esi+edi*4], eax
    ; B = L[j] = ROTL32(L[j] + A+B, A+B);
    add    edx, eax
    mov    cl, dl
    add    edx, [esp+4*ebp]
    rol    edx, cl
    mov    [esp+4*ebp], edx
    ; i++
    inc    edi 
    ; j++
    inc    ebp       
    ; j %= RC5_KEYLEN/4
    and    ebp, 3
sk_l4:
    inc    ch
    jnz    sk_l3
      
    add    esp, 16
    popad
    ret
 
%define A ebx
%define B ebp
 
_rc5_cryptx:
rc5_crypt:
    pushad
    lea    esi, [esp+32+4]   
    lodsd
    push   eax               ; save rc5 ctx
    lodsd
    push   eax               ; save input
    lodsd                    ; 
    xchg   eax, edi          ; output
    lodsd
    cdq
    xchg   eax, ecx          ; enc
    pop    esi               ; esi=input
    lodsd                    ; eax = A
    xchg   eax, A
    lodsd                    ; eax = B
    xchg   eax, B
    pop    esi               ; esi=rc5 ctx
    ; -------------          ; 
    mov    dl, RC5_KR-2
    jecxz  r5_l0             ; do decryption
    lodsd
    add    A, eax
    lodsd
    add    B, eax
    jmp    r5_l2
r5_l0:
    std                      ; move backwards
    lea    esi, [esi+4*edx+4]  ; advance key to end
r5_l2:
    push   ecx
    lodsd
    ; if ecx==0 do decryption
    jecxz r5_l3
    ; A = ROTL32(A ^ B, B) + key->x[i+2];
    xor   A, B
    mov   ecx, B
    rol   A, cl
    add   A, eax
    jmp   r5_l4
r5_l3:
    ; B = ROTR32(B - key->x[i+1], A) ^ A;
    sub   B, eax
    mov   ecx, A
    ror   B, cl
    xor   B, A
r5_l4:
    xchg  A, B
    dec   edx
    pop   ecx
    jnz   r5_l2
    ; -----------
    dec   ecx
    jz    r5_l5
    ; -----------
    lodsd
    sub   B, eax
    lodsd
    sub   A, eax
r5_l5:
    cld
    xchg  eax, A
    stosd
    xchg  eax, B
    stosd
    popad
    ret
