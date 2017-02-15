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
      global _rc4
      global rc4
    %endif
    
; void rc4 (uint32_t data_len, void *data, RC4_CTX *ctx);
_rc4:
rc4:
    pushad
    lea    esi, [esp+4]
    lodsd
    xchg   eax, ecx
    jecxz  r4_l1
    lodsd
    xchg   eax, edi
    lodsd
    xchg   eax, esi
    push   esi               ; save pointer to ctx
    lodsd                    ; eax = x
    xchg   eax, ebx      
    lodsd                    ; ebx = y
    xchg   eax, ebx
    cdq
r4_l0:
    inc    al                ; x++
    
    mov    dl, [esi+eax]     ; dl = s[x]
    add    bl, dl            ; y += dl
    xchg   dl, [esi+ebx]     ; swap s[y], s[x]
    mov    [esi+eax], dl     ; s[x] = s[y]
    
    add    dl, [esi+ebx]     ; dl = s[x] + s[y]
    mov    dl, [esi+edx]     ; dl = s[ dl ]
    xor    [edi], dl         ; *p ^= (s[ s[x] + s[y] ])
    inc    edi               ; p++    
    loop   r4_l0
    pop    edi               ; edi = rc4key
    stosd                    ; save x
    xchg   eax, ebx 
    stosd                    ; save y
r4_l1:
    popad
    ret
    