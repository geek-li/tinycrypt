;
;  Copyright © 2016 Odzhan, Peter Ferrie. All Rights Reserved.
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
; Modular Exponetiation in x86 assembly
;
; size: 175 bytes
;
; global calls use cdecl convention
;
; -----------------------------------------------

  bits 32

  %ifndef BIN
    global _modexp
  %endif
  
_modexp:
modexp:
    pushad

    lea    esi, [esp+32+ 4]
    lodsd
    cdq
    xchg   ecx, eax         ; maxbits
    lodsd
    push   eax
    lodsd
    xchg   ebx, eax         ; exponent
    lodsd
    xchg   ebp, eax         ; modulus
    lodsd                   ; result
    pop    esi              ; base

    ; bits should be aligned by 32-bits
    shr    ecx, 3
    jne    me_l1

mulmod:
    pushad

    ; if (cf==1) do mulmod (r, b, m) else do mulmod (b, b, m)
    xchg   edi, eax ; eax = r
    cmovnc eax, esi ; eax = b
    push   eax      ; where to store result
    xor    edx, edx
    inc    edx

me_l1:

    ; b = base
    sub    esp, ecx
    mov    edi, esp
    push   ecx
    rep    movsb
    pop    ecx
    dec    edx
    mov    esi, esp

    ; e = exponent
    sub    esp, ecx
    mov    edi, esp
    inc    edx
    pushad
    xchg   esi, eax
    cmove  esi, ebx
    rep    movsb
    popad
    mov    ebx, esp
    je     me_l2
    xchg   ebx, esi
    dec    edx
    sub    esp, ecx
    mov    eax, esp
    sub    esp, ecx ;we need non-Z, so no lahf/sahf

me_l2:
    pushfd
    ; result = 1
    xchg   eax, edi
    mov    al, 0 ;preserve flags
    pushad
    rep    stosb
    popad
    call   me_l3

addmod:
    pushad
    cmovnc edi, esi
    shr    ecx, 2
    clc
    push   ecx
    push   edi
am_l1:
    lodsd
    adc    eax, [edi]
    stosd
    loop   am_l1
    pop    edi
    pop    ecx
    mov    esi, ebp
    push   ecx
    dec    ecx
am_l2:
    mov    eax, [edi+4*ecx]
    cmp    eax, [esi+4*ecx]
    loope  am_l2
    pop    ecx
    jb     am_l4
am_l3:
    mov    eax, [edi]
    sbb    eax, [esi]
    stosd
    lodsd
    loop   am_l3
am_l4:
    popad
    ret

me_l3:
    jne    me_l4
    inc    byte [edi]
    sub    dword [esp], addmod-mulmod

me_l4:
    ; n = bits(e)
;;    call   cntbits

; 
; Find the most significant bit in big number
; Return value in eax
;
; in: edx = 0, 
;     ecx = max size of memory in bytes, 
;     ebx = pointer to memory
;
; out: eax = last bit in array
;
cntbits:
    lea   eax, [edx+8*ecx]
cnt_l1:
    dec   eax
    jz    cnt_l2
    bt    [ebx], eax
    jnc   cnt_l1
cnt_l2:
;;    ret

me_l5:
    ; e & 1
    bt     [ebx], edx
me_l6:
    pushfd
    call   [esp+4]
    popfd
    cmc
    jnc   me_l6
    
    inc   edx
    dec   eax
    jns   me_l5

    pop   eax
    popfd
    lea   esp, [esp+2*ecx]
    je     me_l7
    lea   esp, [esp+2*ecx]

    ; return r
    mov   esi, edi
    pop   edi
    rep   movsb

me_l7:
    popad
    ret
