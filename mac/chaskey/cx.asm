;
;  Copyright © 2017 Odzhan, Peter Ferrie. All Rights Reserved.
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
; Chaskey Message Authentication Code
;
; size: 234 bytes
;
; global calls use cdecl convention
;
; -----------------------------------------------

    bits 32

%ifndef BIN
  global chas_setkeyx   
  global _chas_setkeyx   
  
  global chas_macx   
  global _chas_macx   
%endif
    
%define k0 ebp
%define k1 ebx
%define k2 ecx
%define k3 edx
    
chas_setkeyx:
_chas_setkeyx:
    pushad
    mov    edi, [esp+32+4]   ; edi = out
    mov    esi, [esp+32+8]   ; esi = in
    push   edi
    movsd
    movsd
    movsd
    movsd
    pop    esi
    clc
sk_l0:
    pushfd

    lodsd
    xchg   eax, k0
    lodsd
    xchg   eax, k1
    lodsd
    xchg   eax, k2
    lodsd
    xchg   eax, k3
    push   esi
    
    mov    esi, k3
    mov    eax, k0
    shl    eax, 1
    rol    k3, 1
    sbb    dl, dl
    and    dl, 0x87
    xor    al, dl   
    mov    k3, esi
    stosd
    
    mov    eax, k1
    shl    eax, 1
    shr    k0, 31
    or     eax, k0
    stosd
    
    mov    eax, k2
    shl    eax, 1
    shr    k1, 31
    or     eax, k1
    stosd
    
    xchg   eax, k3
    shl    eax, 1
    shr    k2, 31
    or     eax, k2
    stosd
    
    pop    esi
    popfd
    cmc
    jc     sk_l0
    
    popad
    ret
    
%define v0 eax    
%define v1 edx    
%define v2 ebp    
%define v3 ebx
    
; ecx = 16    
; edi = v
chas_permute:
    pushad
    mov    cl, 12
    mov    esi, edi
    lodsd
    xchg   eax, v3
    lodsd
    xchg   eax, v1
    lodsd
    xchg   eax, v2
    lodsd
    xchg   eax, v3
cp_l0:
    add    v0, v1            ; v[0] += v[1];
    rol    v1, 5             ; v[1] = ROTL(v[1], 5);
    xor    v1, v0            ; v[1] ^= v[0];
    rol    v0, 16            ; v[0] = ROTL(v[0], 16);
    add    v2, v3            ; v[2] += v[3]; 
    rol    v3, 8             ; v[3] = ROTL(v[3], 8); 
    xor    v3, v2            ; v[3] ^= v[2]; 
    add    v0, v3            ; v[0] += v[3];
    rol    v3, 13            ; v[3] = ROTL(v[3], 13);
    xor    v3, v0            ; v[3] ^= v[0];
    add    v2, v1            ; v[2] += v[1];
    rol    v1, 7             ; v[1] = ROTL(v[1],  7);
    xor    v1, v2            ; v[1] ^= v[2];
    rol    v2, 16            ; v[2] = ROTL(v[2], 16);
    loop   cp_l0
    stosd
    xchg   eax, v1
    stosd
    xchg   eax, v2
    stosd
    xchg   eax, v3
    stosd
    popad   
    ret
    
; ecx = length
; esi = input
; edi = v   
chas_xor:
    pushad
    jecxz  cx_l1
cx_l0:    
    mov    al, [esi]
    xor    [edi], al
    cmpsb
    loop   cx_l0
cx_l1:    
    popad
    ret    
    
; chaskey    
chas_macx:
_chas_macx:
    pushad
    lea    esi, [esp+32+4]
    pushad                   ; allocate 32 bytes
    mov    edi, esp          ; edi = v
    lodsd
    xchg   eax, ebp          ; ebp = tag ptr
    lodsd
    xchg   eax, ebx          ; ebx = msg ptr
    lodsd
    xchg   edx, eax          ; edx = msglen
    lodsd
    xchg   eax, esi          ; esi = key

    ; memcpy(v, &key[0], 16);
    push   16
    pop    ecx
    push   edi               ; save v
    rep    movsb
    pop    edi               ; restore v
    push   esi               ; save &key[16]
    mov    esi, ebx          ; esi = msg    
    ; absorb message
cm_l0:
    mov    cl, 16
    ; len = (msglen < 16) ? msglen : 16;
    cmp    edx, ecx
    cmovb  ecx, edx
    
    ; chas_xor(&v, msg, len);
    call   chas_xor
    mov    cl, 16
    
    ; if (msglen <= 16)
    cmp    edx, ecx
    jbe    cm_l2
    
    call   chas_permute

    ; msglen -= 16
    sub    edx, ecx
    ; msg += 16
    add    esi, ecx
    
    jmp    cm_l0
cm_l2:    
    pop    esi
    ; if (msglen < 16)
    sete   al  
    ; v.b[msglen] ^= 0x01;
    xor    byte[edi+edx], al
    ; load key + 32
    add    esi, ecx
    ; chas_xor(v, key, 16);
    call   chas_xor
    ; chas_permute(v);
    call   chas_permute
    ; chas_xor(v, key, 16);
    call   chas_xor
    
    ; memcpy(tag, v, 16);
    mov    esi, edi
    mov    edi, ebp
    rep    movsb
        
    popad
    popad
    ret
    
    