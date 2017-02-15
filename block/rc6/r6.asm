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
; RC6 block cipher in x86 assembly
;
; https://people.csail.mit.edu/rivest/pubs/RRSY98.pdf
;
; size: 247 bytes
;
; global calls use cdecl convention
;
; -----------------------------------------------

    bits 32
  
%define RC6_ROUNDS 20
%define RC6_KR     (2*(RC6_ROUNDS+2))
%define RC6_P      0b7e15163h
%define RC6_Q      09e3779b9h

struc RC6_KEY
  x resd RC6_KR
endstruc

  %ifndef BIN
    global _rc6_setkeyx
    global _rc6_cryptx
  %endif
  
_rc6_setkeyx:
rc6_setkey:
    pushad
    lea    esi, [esp+32+ 4]
    lodsd
    xchg   edx, eax            ; rc6key
    lodsd
    xchg   ecx, eax
    lodsd
    xchg   ecx, eax            ; keylen
    xchg   esi, eax            ; key
    
    sub    esp, ecx
    mov    edi, esp
    
    shr    ecx, 2              ; /= 4
    mov    ebx, ecx            ; save keylen/4
    ; copy key to local buffer
    rep    movsd

    mov    eax, RC6_P
    mov    esi, edx
    mov    edi, edx
    mov    cl, RC6_KR
init_key:
    stosd
    add    eax, RC6_Q
    loop   init_key
    
    xor    edi, edi    ; i=0
    xor    ebp, ebp    ; j=0
    xchg   ecx, eax    ; A=0
    cdq                ; B=0
    mov    ch, (-RC6_KR*3) & 255

sk_l1:
    ; A = key->S[i] = ROTL(key->S[i] + A+B, 3); 
    add    eax, edx
    add    eax, [esi+edi*4]
    rol    eax, 3
    mov    [esi+edi*4], eax
    ; B = L[j] = ROTL(L[j] + A+B, A+B);
    add    edx, eax
    mov    cl, dl
    add    edx, [esp+4*ebp]
    rol    edx, cl
    mov    [esp+4*ebp], edx

    ; i++
    inc    edi          
    ; i %= (RC6_ROUNDS*2)+4
    cmp    edi, RC6_KR  
    jb     sk_l2
    xor    edi, edi

sk_l2:    
    ; j++
    inc    ebp       
    ; j %= RC6_KEYLEN/4
    cmp    ebp, ebx
    jb     sk_l3
    xor    ebp, ebp

sk_l3:
    inc    ch
    jnz    sk_l1
      
    lea    esp, [esp+ebx*4]
    popad
    ret

%define A esi
%define B ebx
%define C edx
%define D ebp

_rc6_cryptx:
rc6_crypt:
    pushad

    mov    edi, [esp+32+4] ; rc6 key
    mov    esi, [esp+32+8] ; input
    
    ; load ciphertext
    lodsd
    xchg   eax, D
    lodsd
    xchg   eax, B
    lodsd
    xchg   eax, C
    lodsd
    xchg   eax, D
    xchg   eax, A
    
    push   RC6_ROUNDS
    pop    eax
    mov    ecx, [esp+32+16] ; enc
    jecxz  r6c_l1
    
    ; B += key->x[0];
    add    B, [edi]
    scasd
    ; D += key->x[1];
    add    D, [edi]
    jmp    r6c_l2

r6c_l1:
    ; move to end of key
    lea    edi, [edi+eax*8+12]
    ; load backwards
    std
    
    ; C -= key->x[43];
    sub    C, [edi]
    ; A -= key->x[42];
    scasd
    sub    A, [edi]
    xchg   D, A
    xchg   C, B

r6c_l2:
    scasd
r6c_l3:
    push   eax
    push   ecx
    dec    ecx
    pushfd
    
    ; T0 = ROTL(B * (2 * B + 1), 5);
    lea    eax, [B+B+1]
    imul   eax, B
    rol    eax, 5
    ; T1 = ROTL(D * (2 * D + 1), 5);
    lea    ecx, [D+D+1]
    imul   ecx, D
    rol    ecx, 5
    popfd
    jnz    r6c_l4

    ; A = ROTL(A ^ T0, T1) + key->x[i];
    xor    A, eax
    rol    A, cl
    add    A, [edi]  ; key->x[i]
    scasd
    ; C = ROTL(C ^ T1, T0) + key->x[i+1];
    xor    C, ecx
    xchg   eax, ecx
    rol    C, cl
    add    C, [edi]  ; key->x[i+1]
    jmp    r6c_l5
r6c_l4:    
    ; B = ROTR(B - key->x[i + 1], t) ^ u;
    sub    C, [edi]
    scasd
    ror    C, cl   ; t
    xor    C, eax  ; u
    ; D = ROTR(D - key->x[i], u) ^ t;
    xchg   eax, ecx ; swap u and t
    sub    A, [edi]
    ror    A, cl   ; u
    xor    A, eax  ; t
r6c_l5:
    scasd
    ; swap
    xchg   D, eax
    xchg   C, eax
    xchg   B, eax
    xchg   A, eax
    xchg   D, eax
    ; decrease counter
    pop    ecx
    pop    eax
    dec    eax    ; _I--
    jnz    r6c_l3

    jecxz  r6c_l6
    ; out[0] += key->x[42];
    add    A, [edi]
    ; out[2] += key->x[43];
    add    C, [edi+4]
    jmp    r6c_l7
r6c_l6:
    xchg   D, A
    xchg   C, B
    ; out[3] -= key->x[1];
    sub    D, [edi]
    ; out[1] -= key->x[0];
    sub    B, [edi-4]
    cld
    
r6c_l7:
    ; save ciphertext
    mov    edi, [esp+32+12] ; output
    xchg   eax, A
    stosd
    xchg   eax, B
    stosd
    xchg   eax, C
    stosd
    xchg   eax, D
    stosd
    popad
    ret
    