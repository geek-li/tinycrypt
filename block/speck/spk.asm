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
; Speck64/128 block cipher in x86 assembly
;
; size: 105 bytes
;
; global calls use cdecl convention
;
; -----------------------------------------------

    bits 32

%ifndef BIN
    global _speck_setkeyx
    global _speck_encryptx
%endif
    
%define SPECK_RNDS 27
    
%define k0 eax    
%define k1 ebx    
%define k2 ebp    
%define k3 edx
    
speck_setkeyx:
_speck_setkeyx:
    pushad
    mov    esi, [esp+32+4]   ; esi = in
    mov    edi, [esp+32+8]   ; edi = ks
    lodsd
    xchg   eax, k3
    lodsd
    xchg   eax, k1
    lodsd
    xchg   eax, k2
    lodsd
    xchg   eax, k3
    xor    ecx, ecx
spk_sk:
    ; ((uint32_t*)ks)[i] = k0;
    stosd
    ; k1 = (ROTR32(k1, 8) + k0) ^ i;
    ror    k1, 8
    add    k1, k0
    xor    k1, ecx
    ; k0 = ROTL32(k0, 3) ^ k1;
    rol    k0, 3
    xor    k0, k1
    ; rotate left 32-bits
    xchg   k3, k2
    xchg   k3, k1
    ; i++
    inc    ecx
    cmp    cl, SPECK_RNDS    
    jnz    spk_sk   
    popad
    ret

%define x0 eax    
%define x1 ebx
    
speck_encryptx:
_speck_encryptx:
    pushad
    lea    esi, [esp+32+4]
    lodsd
    xchg   edi, eax          ; edi = ks
    lodsd
    xchg   eax, ecx          ; ecx = enc
    lodsd
    xchg   eax, esi          ; esi = in
    push   esi
    lodsd    
    xchg   eax, x1
    lodsd
    xchg   eax, x1
    test   ecx, ecx
    mov    cl, SPECK_RNDS
    jz     spk_e0
spk_d0:
    ; x1 = ROTR32(x1 ^ x0, 3);
    xor    x1, x0
    ror    x1, 3
    ; x0 = ROTL32((x0 ^ ks[SPECK_RNDS-1-i]) - x1, 8);
    xor    x0, [edi+4*ecx-4]
    sub    x0, x1
    rol    x0, 8
    loop   spk_d0
    jmp    spk_end    
spk_e0:
    ; x0 = (ROTR32(x0, 8) + x1) ^ ks[i];
    ror    x0, 8
    add    x0, x1
    xor    x0, [edi]
    scasd
    ; x1 = ROTL32(x1, 3) ^ x0;
    rol    x1, 3
    xor    x1, x0
    loop   spk_e0
spk_end:
    pop    edi
    ; ((uint32_t*)in)[0] = x0;
    stosd
    xchg   eax, x1
    ; ((uint32_t*)in)[1] = x1;
    stosd    
    popad
    ret    
    
    
    
    