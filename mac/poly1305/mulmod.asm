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
; Modular Multiplication in x86 assembly
;
; size: 99 bytes
;
; global calls use cdecl convention
;
; -----------------------------------------------
  bits 32
  
  %ifndef BIN
    global _mulmod
  %endif
    
struc pushad_t
  _edi dd ?
  _esi dd ?
  _ebp dd ?
  _esp dd ?
  _ebx dd ?
  _edx dd ?
  _ecx dd ?
  _eax dd ?
  .size:
endstruc
    
; ***************************************
;
; poly1305 addition
;
; al  = last 
; ecx = inlen
; edi = out
; esi = in
;
; ***************************************
_poly1305_addx:
poly1305_addx:
    pushad
    movzx  edx, al            ; edx = last
    xor    ebx, ebx           ; x = 0
    xor    ecx, ecx           ; ecx = 0    
    xor    ebp, ebp           ; i = 0
add_l0:
    movzx  eax, byte[esi]     ; c = *p 
    cmp    ebp, [esp+_ecx]    ; EFLAGS = (i - inlen)
    cmovz  eax, edx           ; c = (i == inlen) ? last : c;
    cmova  eax, ecx           ; c = (i  > inlen) ? 0    : c;
    adc    esi, ecx           ; p = (i  < inlen) ? p+1  : p;
    add    ebx, [edi]         ; x += out[i]
    add    ebx, eax           ; x += c
    mov    al, bl             ; al = x
    stosd                     ; out[i] = x & 255
    shr    ebx, 8             ; x >>= 8
    inc    ebp                ; i++
    cmp    ebp, 17 
    jb     add_l0
    popad
    ret
    
_poly1305_mulmodx:
poly1305_mulmodx:
    pushad
    
    ; 3 ffffffff ffffffff ffffffff fffffffb
    mov    eax, 0xfffffffb
    stosd
    or     eax, -1
    stosd
    stosd
    stosd
    push   3
    pop    eax
    stosd
    
    xor    eax, eax
    push   32
    pop    ecx
    cdq
    
    push   edi               ; save edi
    ; r=0
    sub    esp, ecx          ; create space for r and assign x
    mov    edi, esp
    pushad
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

cntbits:
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
    cmovnc edi, esi          ; if (cf==0) do t = xmod(t, t, m)
    mov    ebx, edi          ; else r = xmod(r, t, m)
addmod:
    clc
    pushad
am_l1:
    lodsb
    adc    al, [edi]
    stosb
    loop   am_l1
    popad
    mov    esi, ebp
    push   ecx
    dec    ecx
am_l2:
    mov    al, [edi+ecx]
    cmp    al, [esi+ecx]
    loope  am_l2
    pop    ecx
    jb     am_l4
am_l3:
    mov    al, [edi]
    sbb    al, [esi]
    stosb
    lodsb
    loop   am_l3
am_l4:
    popad
    ; ----------------
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
    
; ***************************************
;
; poly1305 mac function
;
; ***************************************
poly1305_macx:
_poly1305_macx:
    pushad

    lea    esi, [esp+32+4]
    xor    eax, eax          ; eax = 0
    cdq                      ; edx = 0
    push   64
    pop    ecx
    sub    esp, ecx
    mov    edi, esp
    mov    ebx, esp
    rep    stosb
    
    lodsd
    push   eax               ; save out
    
    lodsd
    push   eax               ; save in
    
    lodsd
    push   eax               ; save inlen
    
    lodsd
    xchg   esi, eax          ; esi = k
    
    xchg   eax, edx          ; eax = 0
    cdq                      ; edx = 0
    mov    cl,  16
    mov    edi, ebx          ; edi = r
    ; copy and clamp r
pm_l0:
    lodsd                    ; al = k[i]
    and    eax, 0xfc0fffff
    stosd                    ; r[i] = eax
    loop   pm_l0
    scasd                    ; skip 1
    
    pop    edx               ; edx = inlen
    pop    eax               ; eax = in
    push   esi               ; save &k[16]
    xchg   eax, esi          ; esi = in
poly_l0:
    test   edx, edx
    jz     pm_l5             ; break if inlen==0
    
    mov    cl, 16
    cmp    edx, ecx          ; 
    cmovb  ecx, edx          ; len = (inlen < 16) ? len : 16
    
    mov    al, 1
    call   poly1305_addx     ; poly1305_add (acc, in, len, 1);
    call   poly1305_mulmodx  ; poly1305_mulmod (acc, r);

    sub    edx, ecx          ; inlen -= len    
    add    esi, ecx          ; in += len
    
    jmp    poly_l0
    
pm_l5:
    mov    cl, 17
    pushad 
    mov    esi, edi          ; esi = acc
    mov    edi, ebx          ; edi = r    
    rep    movsb             ; memcpy (r, acc, sizeof(r));
    popad

    mov    al, 252           ;
    lea    esi, [edi+ecx]  ; esi = minusp
    mov    byte[esi], 5
    dec    ecx
    call   poly1305_addx     ; poly1305_add (acc, minusp, 16, 252);
    
    mov    esi, ebx          ; esi = r
    movzx  ebx, byte[edi+ecx]  ; neg = -(acc[16] >> 7);
    shr    bl, 7
    neg    bl
    pushad
pm_l6:                       ; acc[i] ^= neg & (r[i] ^ acc[i]);
    lodsb                    ; eax = r[i]
    xor    al, [edi]        ; eax ^= acc[i]
    mov    dl, bl          ; edx = neg
    and    dl, al          ; neg &= eax
    xor    [edi], dl        ; acc[i] ^= edx
    scasb                    ; edi += 4
    loop   pm_l6
    popad
    
    pop    esi               ; esi = &k[16]
    xor    al, al            ; 
    call   poly1305_addx     ; poly1305_add (acc, &k[16], 16, 0);

    ; return tag
    pop    esi               ; edi = out
    xchg   edi, esi
    rep    movsb
    
    popad
    popad
    popad                    ; restore registers
    ret
    