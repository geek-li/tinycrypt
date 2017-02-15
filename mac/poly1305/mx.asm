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
; size: 140 bytes or 143 for slightly faster version
;
; global calls use cdecl convention
;
; -----------------------------------------------

; %define BYTES 1

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
    xchg   ecx, eax          ; ecx = max bytes
    lodsd
    push   eax               ; save base
    lodsd
    xchg   ebx, eax          ; ebx = exponent
    lodsd
    xchg   ebp, eax          ; ebp = modulus
    lodsd                   
    xchg   edi, eax          ; edi = result
    pop    esi               ; esi = base
    
    inc    edx               ; edx = x=1
    stc                      ; r   = xmod (b, e, m, 1)
    db     0b0h              ; mov al, 0x60 to mask pushad
mulmod:
    pushad                   ; save registers
mm_l1:
; cf=1 : r = mulmod (r, t, m);
; cf=0 : t = mulmod (t, t, m);
    push   edi               ; save edi
    ; r=x
    sub    esp, ecx          ; create space for r and assign x
    mov    edi, esp
    pushad
    dec    ecx               ; skip 1
    xchg   eax, edx          ; r=x
    stosb
    xor    al, al            ; zero remainder of buffer
    rep    stosb
    popad
    ; *************
    ; t=b
    sub    esp, ecx          ; create space for t and assign b
    mov    eax, esp
    pushad
    xchg   eax, edi
    rep    movsb
    popad
    xchg   eax, esi          
    call   ld_fn
    
; cf=1 : r = addmod (r, t, m);
; cf=0 : t = addmod (t, t, m);

; ebp  : m
; esi  : t
; edi  : r or t
; ecx  : size in bytes
;
addmod:
    pushad
%ifndef BYTES
    shr    ecx, 2            ; /= 4
%endif
    clc
    pushad
am_l1:
%ifndef BYTES
    lodsd
    adc    eax, [edi]
    stosd
%else
    lodsb
    adc    al, [edi]
    stosb
%endif
    loop   am_l1
    popad
    mov    esi, ebp
    push   ecx
    dec    ecx
am_l2:
%ifndef BYTES
    mov    eax, [edi+ecx*4]
    cmp    eax, [esi+ecx*4]
%else
    mov    al, [edi+ecx]
    cmp    al, [esi+ecx]
%endif
    loope  am_l2
    pop    ecx
    jb     am_l4
am_l3:
%ifndef BYTES
    mov    eax, [edi]
    sbb    eax, [esi]
    stosd
    lodsd
%else
    mov    al, [edi]
    sbb    al, [esi]
    stosb
    lodsb
%endif
    loop   am_l3
am_l4:
    popad
    ret
    ; -----------------------------
ld_fn:
    dec    edx
    js     cntbits
    sub    dword[esp], addmod - mulmod
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
    xor    edx, edx
    lea    eax, [edx+ecx*8]
cnt_l1:
    dec    eax
    jz     xm_l1
    bt     [ebx], eax
    jnc    cnt_l1
xm_l1:
    ; if (e & 1)
    bt     [ebx], edx
xm_l2:
    pushfd
    pushad
    cdq
    cmovnc edi, esi          ; if (cf==0) do t = xmod(t, t, m)
    mov    ebx, edi          ; else r = xmod(r, t, m)
    call   dword[esp+32+4]   ; invoke mulmod or addmod
    popad
    popfd
    cmc
    jnc    xm_l2
    
    inc    edx
    dec    eax
    jns    xm_l1

    ; return r
    mov    esi, edi
    lea    esp, [esp+ecx*2+4]
    pop    edi
    rep    movsb
    popad
    ret
