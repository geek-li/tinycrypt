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

    bits 32
    
MAX_BITS equ 256
MAX_BYTE equ MAX_BITS / 8
MAX_WORD equ MAX_BITS / 32
   
; uint32_t bn_num_bits (void *a);   
bn_num_bits:    
_bn_num_bits:
    pushad    
    mov    eax, [esp+32+4]  ; eax = a
    push   MAX_BITS
    pop    ecx
scan_loop:
    bt     [eax], ecx
    jc     exit_scan
    dec    ecx
    jns    scan_loop
exit_scan:
    lea    eax, [ecx+1]
    mov    [esp+28], eax
    popad
    ret
    
; void bn_copy (void *a, void *b);
bn_copy:
_bn_copy:
    pushad
    mov    esi, [esp+32+4] ; esi = a
    mov    edi, [esp+32+8] ; edi = b
    push   MAX_WORD
    pop    ecx  
    rep    movsd
    popad
    ret

; void bn_zero (void *a);    
bn_zero:    
_bn_zero:    
    pushad
    mov    edi, [esp+32+4] ; edi = a
    xor    eax, eax
    push   MAX_WORD
    pop    ecx
    rep    stosd
    popad
    ret
    
; void bn_add(void *r, void *a, void *b);    
bn_add:    
_bn_add:    
    pushad
    lea    esi, [esp+32+4]
    lodsd
    xchg   edi, eax    ; edi = r
    lodsd
    xchg   ebx, eax    ; ebx = a
    lodsd
    xchg   esi, eax    ; esi = b 
    push   MAX_WORD
    pop    ecx
    clc                ; clear carry flag
a_l0:
    lodsd              ; eax = a[i]
    adc    eax, [ebx]  ; eax = a[i] + b[i] + cy
    stosd              ; r[i] = a[i] + b[i]
    lea    ebx, [ebx+4]
    loop   a_l0
    popad
    ret
    
; void bn_add(void *r, void *a, void *b);     
bn_sub:    
_bn_sub:    
    pushad
    lea    esi, [esp+32+4]
    lodsd
    xchg   edi, eax    ; edi = r
    lodsd
    xchg   ebx, eax    ; ebx = a
    lodsd
    xchg   esi, eax    ; esi = b 
    push   MAX_WORD
    pop    ecx
    clc                ; clear carry flag
s_l0:
    lodsd              ; eax = a[i]
    sbb    eax, [ebx]  ; eax = a[i] + b[i] + cy
    stosd              ; r[i] = a[i] + b[i]
    lea    ebx, [ebx+4]
    loop   s_l0
    popad
    ret    
    
; void bn_div (void *q, void *r, void *a, void *b);
bn_div:              
_bn_div:
    pushad
    lea    esi, [esp+32+4]         
    lodsd
    push   eax          ; save q
    lodsd
    xchg   eax, edi     ; edi = r
    lodsd
    xchg   eax, ebx     ; ebx = a
    mov    ecx, ebx     ; ecx = a
    lodsd
    xchg   eax, edx     ; edx = b
    
    call   bn_num_bits
    test   eax, eax
    jz     div_l2

    xchg   eax, ebp
    
    mov    ecx, esi     ; q = 0
    call   bn_zero
    
    mov    ecx, edi     ; r = 0
    call   bn_zero
div_l0:    
    clc
    mov    ecx, esi     ; q = q + q
    call   bn_add
    
    bt     [ebx], ebp   ; a & 1
    mov    ecx, edi     ; r = r + r
    call   bn_add  
    
    call   bn_cmp       ; if (r >= b) r -= b;
    jb     div_l1
    
    call bn_sub
    or     byte[esi], 1 ; q |= 1
div_l1:    
    dec    ebp
    jns    div_l0
div_l2:    
    popad
    ret
    
bn_mul:
_bn_mul:
    pushad    
    mov    esi, [esp+32+4]  ; esi = 
    mov    ebx, [esp+32+8]  ; ebx = 

    push   MAX_WORD * 2
    pop    ecx
    sub    esp, ecx 
    
    ; t = 0
    mov    edi, esp
    xor    eax, eax
    rep    stosd
    
    mov    cl, MAX_WORD
set_j:
    push   ecx
    push   ebx
    push   esi
    push   edi
    mov    cl, MAX_WORD
load_x:
    mov    eax, [esi]         ; eax      = a[i]
    mul    dword[ebx]     ; eax:edx  = a[i] * b[j]
    push   edi
    add    eax, [edi]
    stosd                     ; r[i+j]   = eax
    xchg   eax, edx
    adc    eax, [edi]         ; r[i+j+1] = edx
    stosd
add_carry:
    adc    dword[edi], 0
    lea    edi, [edi+4]
    jc     add_carry
    pop    edi
    add    ebx, 4
    scasd
    loop   load_x
    pop    edi
    pop    esi
    pop    ebx
    pop    ecx
    scasd
    lodsd 
    loop   set_j
    pop    edx    ; t
    pop    ecx    ; r
    ; r = t
    call   bn_copy
    popad
    ret
    
; *************************************************
; Compare a with b, set flags accordingly.
;
; int bn_cmp (void *a, void *b);
;
; a < b  : eax = -1, JL for signed, JB for unsigned
; a > b  : eax = 1, JG for signed, JA for unsigned
; a == b : eax = 0, JE for equal
; *************************************************   
bn_cmp:
_bn_cmp:
    pushad  
    mov    esi, [esp+32+4]  ; esi = a
    mov    edi, [esp+32+8]  ; edi = b
    push   MAX_WORD
    pop    ecx
    xor    edx, edx
    std                     ; set direction
    lodsd                   ; subtract 4
    scasd
    lea    esi, [esi+4*ecx]
    lea    edi, [edi+4*ecx]
    repz   cmpsd            ; repeat while equal
    cld
    pushfd                  ; save efl
    seta   dl               ; set dl if above
    sbb    eax, eax
    xor    eax, edx
    popfd
    popad
    ret
    
    