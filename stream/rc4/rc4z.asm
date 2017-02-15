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
; Single call RC4 in x86 assembly (for 128-bit keys)
; Odzhan
;
; 64 bytes

;rc4_ctx struct
;  sbox db 256 dup (?)   ; uint8_t s[256];
;  key  db 16 dup (?)    ; uint8_t k[16];
;  len  dd ?             ; uint32_t len;
;  buf  db BUFSIZ dup(?) ; uint8_t buf[BUFSIZ];
;rc4_ctx ends
    
    bits 32
    
%ifndef BIN
    global RC4Z
    global _RC4Z
%endif

_RC4Z:
RC4Z:
    pushad
    mov    edi, [esp+32+4]   ; edi=rc4_key
    mov    esi, edi          ; esi=rc4_key
    ; x=0, y=0, i=0            
    xor    ecx, ecx          ;
    mul    ecx
; --------------------------------------------
; KSA. edi=s + key, esi=s, eax=i, edx=j
; --------------------------------------------
init_sbox:
    stosb                    ; s[i]=i
    inc    al                ; i++
    jnz    init_sbox
    ; edi now points to 128-bit key
init_key:
    movzx  ebx, al           ; ebx = k_idx
    and    bl, 15            ; %= 16
    
    add    dl, [edi+ebx]     ; j += k[k_idx % 16]
    jmp    swap
upd_idx:
    inc    al                ; i++
    jnz    init_key          ; i<256
    cdq                      ; y=0 
    mov    ecx, [edi+16]     ; ecx = len
; -----------------------------------------------
; PRNG. edi=buf, esi=s, eax=x, edx=y, ecx=len 
; -----------------------------------------------
crypt_data:
    inc    al                ; x++
swap:    
    mov    bl, [esi+eax]     ; bl = s[i] or bl=s[x]
    add    dl, bl            ; j += bl or y += bl 
    xchg   bl, [esi+edx]     ; XCHG(bl, s[j]) or XCHG(bl, s[y])
    mov    [esi+eax], bl
    jecxz  upd_idx           ; if ecx == 0 goto upd_idx
    
    add    bl, [esi+edx]     ; bl = s[x] + s[y]
    mov    bl, [esi+ebx]     ; bl = s[ bl ]
    xor    [edi+16+4], bl    ; *p ^= (s[ s[x] + s[y] ])
    inc    edi               ; p++     
    loop   crypt_data
    popad
    ret
