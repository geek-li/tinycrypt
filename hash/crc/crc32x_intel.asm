;
;  Copyright © 2017 Odzhan. All Rights Reserved.
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
; Emulation of INTEL CRC32 instruction using x86 assembly
;
; size: 46 bytes
;
; global calls use cdecl convention
;
; -----------------------------------------------

    bits 32
    
    %ifndef BIN
      global _icrc32x
    %endif
    
icrc32x:    
_icrc32x:
    pushad
    lea    esi, [esp+32+4]
    lodsd
    xchg   ebx, eax          ; edx = crc
    lodsd
    xchg   ecx, eax          ; ecx = inlen
    lodsd
    xchg   esi, eax          ; esi = in
    jecxz  crc_l2            ; if (inlen==0) return crc;
crc_l0:
    lodsb                    ; c = *in++
    push   ecx
    push   8
    pop    ecx
    xor    bl, al            ; crc ^= c
crc_l1:
    mov    eax, ebx          ; eax = crc
    and    eax, 1            ; &= 1
    imul   eax, -2097792136  ; eax * 0x82F63B78
    shr    ebx, 1            ; crc >>= 1
    xor    ebx, eax          ; crc ^= eax 
    loop   crc_l1    
    pop    ecx
    loop   crc_l0    
crc_l2:    
    mov    [esp+28], ebx     ; return crc    
    popad
    ret
    