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
    bits 32
    
    %ifndef BIN
      global _rc4_setkey
      global rc4_setkey
    %endif
    
; void rc4_setkey (uint32_t key_len, void *key, RC4_CTX *ctx);
_rc4_setkey:
rc4_setkey:
    pushad
    lea    esi, [esp+4]
    lodsd
    xchg   eax, ebp
    lodsd
    push   eax
    lodsd
    xchg   eax, esi
    pop    esi
    xor    eax, eax               ; i=0
    xor    ecx, ecx
    cdq                           ; j=0
    stosd                         ; x=0
    stosd                         ; y=0
r4_l0:
    mov    [edi+eax], al          ; s[i] = i
    inc    al                     ; i++
    jnz    r4_l0
r4_l1:
    cmp    ecx, ebp               ; if (key_idx == key_len)
    jne    r4_l2
    xor    ecx, ecx               ; key_idx = 0
r4_l2:
    mov    bl, [edi+eax]          ; s[i]
    add    dl, bl                 ; j += s[i]
    add    dl, [esi+ecx]          ; j += key[key_idx]
    xchg   bl, [edi+edx]          ; s[j] = s[i]
    mov    [edi+eax], bl          ; s[i] = s[j]
    inc    ecx                    ; key_idx++
    inc    al                     ; i++
    jnz    r4_l1                  ;
    
    popad
    ret
