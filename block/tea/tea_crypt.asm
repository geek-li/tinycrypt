;
;  Copyright © 2015 Odzhan. All Rights Reserved.
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

    bits 32
    
%define TEA_ROUNDS 32
%define TEA_DELTA  09e3779b9h

%ifndef BIN
  global TEA_Encryptx
  global _TEA_Encryptx
%endif
  
_TEA_Encryptx:
TEA_Encryptx:
    pushad
    lea    esi, [esp+32+4]
    lodsd
    xchg   eax, edx          ; edx = data 
    lodsd
    push   eax               ; save key
    lodsd
    xchg   eax, edx          ; edx = flag
    pop    esi               ; esi = key
    mov    ecx, esp 
    pushad                   ; allocate 16 bytes for key
    sub    ecx, esp          ; ecx = 16
    mov    edi, esp          ; move stack pointer into edi
    rep    movsb             ; move key onto stack
    mov    edi, esp          ; esi = key
    mov    esi, esp          ; edi = key
  
    neg    edx               ; negate input flag
    pushfd                   ; save cpu flags
    jz     load_data         ; if neg edx was zero, we're encrypting
  
    mov    cl, 2             ; else we're decrypting and need to 
    pushad                   ; rotate key right 64-bits
load_key:                    ; this is because of how key is loaded
    lodsd                    ; load 32-bits of key
    xchg   eax, [edi+8]      ; swap with upper 64-bits
    stosd                    ; save upper 32-bits
    loop   load_key
    popad
    ; ------------------------
load_data:
    popfd                    ; restore register flags
    xchg   eax, esi          ; eax now contains data to process
    push   esi               ; save pointer to it
    push   edx               ; save result of neg edx for add/sub
    lodsd                    ; eax=v[0]
    xchg   eax, ebx          ; store in ebx
    lodsd                    ; ebx=v[1]
    xchg   eax, ebx          ; store in ebx
    ; for (i=32; i>0; i--)
    mov    cl, TEA_ROUNDS    ; default is 32 rounds
    ; delta = 0x9e3779b9
    mov    ebp, 09e3779b9h   ; ebp = delta
    jz     t_el              ; neg edx == 0? skip shift for decryption
    shl    ebp, 5            ; *= 32 for decryption
    xchg   eax, ebx          ; and swap v0 + v1
t_el:
    clc                      ; clear carry flag   
    push   edi
t_ev:
    pushfd
    mov    edx, ebx          ; v << 4 + k
    shl    edx, 4
    add    edx, [edi]        ; add key from edi
    scasd                    ; advance edi 4 bytes
    mov    esi, ebx          ; v + sum
    add    esi, ebp
    xor    edx, esi
    mov    esi, ebx          ; v1 >> 5 + k1
    shr    esi, 5
    add    esi, [edi]        ; add key from edi
    scasd                    ; advance edi 4 bytes
    xor    edx, esi
    ; v += ((v << 4) + k) ^
    ;        (v + sum) ^
    ;       ((v >> 5) + k)
    xor    eax, [esp+8]
    add    eax, edx
    xor    eax, [esp+8]
  
    popfd                    ; restore flags
    xchg   eax, ebx          ; swap v0, v1
    cmc                      ; flip carry flag
    jc     t_ev              ; keep going if carry
    pop    edi               ; restore pointer to key
  
    ; sum += delta or sum -= delta
    ;
    ; the following performs addition or subtraction
    ; depending on flag which we expect to be 1 or 0
    ; the negate operation earlier will change it to -1 if true
    ; but remain 0 if false.
    xor    ebp, [esp]        ; ebp ^= -flag
    add    ebp, 09e3779b9h   ; performs normal addition
    xor    ebp, [esp]        ; ebp ^= -flag
  
    loop   t_el
save_r:
    pop    ecx               ; restore -flag
    inc    ecx               ; -flag++
    jnz    $+3               ; if not zero, it means encryption
    xchg   eax, ebx          ; else decryption so swap again
    pop    edi
    stosd                    ; v[0] = v0
    xchg   eax, ebx
    stosd                    ; v[1] = v1
    popad                    ; release stack
    popad
    ret
