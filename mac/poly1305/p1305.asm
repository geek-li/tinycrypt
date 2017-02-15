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
; Poly1305 MAC in x86 assembly
;
; size: 339 bytes
;
; global calls use cdecl convention
;
; -----------------------------------------------

    bits 32
    
%ifndef BIN
    global _poly1305_macx
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

; ***************************************
;
; poly1305 modular multiplication
;
; "P" is 2^130-5 or 3fffffffffffffffffffffffffffffffb
;
; esi = acc 
; ebx = r
;
; ***************************************
poly1305_mulmodx:
_poly1305_mulmodx:
    pushad
    ; memcpy ((uint8_t*)hr, (uint8_t*)acc, 17*4); 
    push   17*4
    pop    ecx
    sub    esp, ecx
    mov    esi, edi            ; esi = acc
    mov    edi, esp            ; edi = hr
    push   esi                 ; save acc
    rep    movsb
    pop    edi                 ; edi = acc
    
    ; multiply
    mov    esi, ebx            ; esi = r
    xor    ebx, ebx            ; i = 0
mm_l0:
    xor    ecx, ecx            ; j = 0
    xor    edx, edx            ; u = 0
mm_l1:
    mov    eax, [esp+ecx*4]    ; eax = hr[j]
    mov    ebp, ebx            ; ebp = i
    sub    ebp, ecx            ; ebp = i - j
    imul   eax, [esi+ebp*4]    ; eax *= r[i - j]
    add    edx, eax            ; u += eax
    inc    ecx                 ; j++
    cmp    ecx, ebx            ; j <= i
    jbe    mm_l1
mm_l2:
    push   17
    pop    ebp
    
    cmp    ecx, ebp            ; i < 17
    jae    mm_l3

    sub    ebp, ecx            ; ebp = 17 - j          
    add    ebp, ebx            ; ebp += i
    
    push   64
    pop    eax
    inc    ah                  ; eax = 320
    imul   eax, [esi+ebp*4]    ; eax *= r[i+17-j]
    imul   eax, [esp+ecx*4]    ; eax *= hr[j]
    add    edx, eax            ; u += eax 
    inc    ecx                 ; j++
    jmp    mm_l2
mm_l3:
    mov    [edi+ebx*4], edx    ; acc[i] = u
    inc    ebx
    cmp    ebx, 17
    jb     mm_l0
    
    ; reduce
    xor    ebx, ebx
    xor    edx, edx            ; u = 0
f_lx:    
    mov    cl, 16
    mov    esi, edi            ; esi = acc
    push   edi    
f_l0:
    lodsd                      ; eax = acc[i]
    add    edx, eax            ; u += acc[i]
    movzx  eax, dl             ; eax = u & 255
    stosd                      ; acc[i] = eax
    shr    edx, 8              ; u >>= 8
    loop   f_l0
    dec    ebx                 ;  
    jnp    f_l1    
    ; ---------------
    lodsd                      ; eax = acc[16]
    add    edx, eax            ; u += eax
    movzx  eax, dl             ; 
    and    al, 3               ; al &= 3
    stosd                      ; acc[16] = eax
    shr    edx, 2              ; u >>= 2
    lea    edx, [edx+edx*4]    ; u = (u >> 2) * 5
    pop    edi
    jmp    f_lx
f_l1:    
    add    [edi], edx          ; acc[16] += u;
    add    esp, 17*4 + 4
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
    xor    ecx, ecx          ; ecx = 0 
    mov    cl, 196           ; ecx = 196
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
    ; copy r
pm_l0:
    lodsb                    ; al = k[i]
    stosd                    ; r[i] = eax
    loop   pm_l0
    scasd                    ; skip 1

    ; clamp r
    mov    al, 15
    and    [ebx+ 3*4], eax   ; r[ 3] &= 15;
    and    [ebx+ 7*4], eax   ; r[ 7] &= 15;
    and    [ebx+11*4], eax   ; r[11] &= 15;
    and    [ebx+15*4], eax   ; r[15] &= 15;
    
    mov    al, 252
    and    [ebx+ 4*4], eax   ; r[ 4] &= 252;
    and    [ebx+ 8*4], eax   ; r[ 8] &= 252;
    and    [ebx+12*4], eax   ; r[12] &= 252;
    and    [ebx+16*4], edx   ; r[16]  = 0;
    
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
    rep    movsd             ; memcpy (r, acc, sizeof(r));
    popad

    mov    al, 252           ;
    lea    esi, [edi+ecx*4]  ; esi = minusp
    mov    byte[esi], 5
    dec    ecx
    call   poly1305_addx     ; poly1305_add (acc, minusp, 16, 252);
    
    mov    esi, ebx          ; esi = r
    mov    ebx, [edi+ecx*4]  ; neg = -(acc[16] >> 7);
    shr    ebx, 7
    neg    ebx
    pushad
pm_l6:                       ; acc[i] ^= neg & (r[i] ^ acc[i]);
    lodsd                    ; eax = r[i]
    xor    eax, [edi]        ; eax ^= acc[i]
    mov    edx, ebx          ; edx = neg
    and    edx, eax          ; neg &= eax
    xor    [edi], edx        ; acc[i] ^= edx
    scasd                    ; edi += 4
    loop   pm_l6
    popad
    
    pop    esi               ; esi = &k[16]
    xor    al, al            ; 
    call   poly1305_addx     ; poly1305_add (acc, &k[16], 16, 0);

    ; return tag
    pop    esi               ; edi = out
    xchg   edi, esi
pm_l7:
    lodsd                    ; eax = acc[i]
    stosb                    ; out[i] = al
    loop   pm_l7
    
    mov    cl, 196
    add    esp, ecx          ; release stack
    popad                    ; restore registers
    ret
    