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
; CRC16 in x86 assembly
;
; size: 48 bytes
;
; global calls use cdecl convention
;
; -----------------------------------------------

    bits 16
    
    %ifndef BIN
      global _xcrc16x
    %endif
    
xcrc16x:    
_xcrc16x:
    pusha
    mov    bp, sp
    lea    si, [bp+16+2]
    lodsw
    xchg   bx, ax          ; ebx = crc
    lodsw
    xchg   cx, ax          ; ecx = inlen
    lodsw
    xchg   si, ax          ; esi = in
    
    not    bx               ; ~crc
    jcxz   crc_l3
crc_l0:
    lodsb
    push   cx
    push   8
    pop    cx
    xor    bl, al
crc_l1:
    mov    ax, bx
    and    ax, 1
    imul   ax, -24575
    shr    bx, 1
    xor    bx, ax
crc_l2:    
    loop   crc_l1    
    pop    cx
    loop   crc_l0    
crc_l3:    
    not    bx
    mov    [bp+14], bx     ; return ~crc    
    popa
    ret
    