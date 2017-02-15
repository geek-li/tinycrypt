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

; -----------------------------------------------
; Noekeon block cipher in x86 assembly
;
; size: 292 bytes
;
; global calls use cdecl convention
;
; -----------------------------------------------

    bits 32
 
    %ifndef BIN
      global _Noekeonx
    %endif
    
struc pushad_t
  _edi resd 1
  _esi resd 1
  _ebp resd 1
  _esp resd 1
  _ebx resd 1
  _edx resd 1
  _ecx resd 1
  _eax resd 1
  .size:
endstruc
    
%define Nr 16
    
%define a0 eax 
%define a1 ebx 
%define a2 ecx 
%define a3 edx

%define t0 ebp
%define t1 esi

Thetax:
_Thetax:
    pushad
    push   esi
    lodsd
    xchg   a3, eax
    lodsd
    xchg   a1, eax
    lodsd
    xchg   a2, eax
    lodsd
    xchg   a3, eax
    ; t = a[0] ^ a[2];
    mov    t0, a0
    xor    t0, a2
    ; t ^= ROTR32(t, 8) ^ ROTL32(t, 8);
    mov    t1, t0
    rol    t1, 8
    xor    t0, t1    
    ror    t1, 16
    xor    t0, t1
    ; a[1] ^= t;    
    xor    a1, t0
    ; a[3] ^= t;
    xor    a3, t0
    ; a[0] ^= k[0];
    xor    a0, [edi]
    ; a[1] ^= k[1];
    xor    a1, [edi+ 4]
    ; a[2] ^= k[2];
    xor    a2, [edi+ 8]
    ; a[3] ^= k[3];
    xor    a3, [edi+12]
    ; t = a[1] ^ a[3];
    mov    t0, a1
    xor    t0, a3
    ; t ^= ROTR32(t, 8) ^ ROTL32(t, 8);
    mov    t1, t0
    rol    t1, 8
    xor    t0, t1    
    ror    t1, 16
    xor    t0, t1
    ; a[0] ^= t;    
    xor    a0, t0
    ; a[2] ^= t;
    xor    a2, t0
    pop    edi
    stosd
    xchg   eax, a1
    stosd
    xchg   eax, a2
    stosd
    xchg   eax, a3
    stosd
    popad
    ret
   
; esi = State
; edi = Key
; ecx = enc
; eax = rnd   
; CF  = end
Round:
    pushad
    pushfd
    call   nx_rc
    db     0x80
    db     0x1B, 0x36, 0x6C, 0xD8 
    db     0xAB, 0x4D, 0x9A, 0x2F 
    db     0x5E, 0xBC, 0x63, 0xC6 
    db     0x97, 0x35, 0x6A, 0xD4
nx_rc:
    pop    ebx
    xlatb
    jecxz  nxr_enc
    xchg   al, ah
nxr_enc:
    setne  cl
    ; State[0] ^= Constant1;
    xor    byte[esi], al
    ; Theta(State, Key);
    call   Thetax
    ; State[0] ^= Constant2;
    xor    byte[esi], ah
    jecxz  nxr_end
    mov    edi, esi    
    lodsd
    xchg   a3, eax
    lodsd
    xchg   a1, eax
    lodsd
    xchg   a2, eax
    lodsd
    xchg   a3, eax
Pi1:
    rol    a1, 1
    rol    a2, 5
    rol    a3, 2
Gamma:
    ; a[1] ^= ~((a[3]) | (a[2]));
    mov    ebp, a3
    or     ebp, a2
    not    ebp
    xor    a1, ebp
    ; a[0] ^=   a[2] & a[1];
    mov    ebp, a2
    and    ebp, a1
    xor    a0, ebp
    ; XCHG(a[0], a[3], t);
    xchg   a0, a3
    ; a[2] ^= a[0] ^ a[1] ^ a[3];
    xor    a2, a0
    xor    a2, a1
    xor    a2, a3
    ; a[1] ^= ~((a[3]) | (a[2]));
    mov    ebp, a3
    or     ebp, a2
    not    ebp
    xor    a1, ebp  
    ; a[0] ^=   a[2] & a[1];
    mov    ebp, a2
    and    ebp, a1
    xor    a0, ebp    
Pi2:
    ror    a1, 1
    ror    a2, 5
    ror    a3, 2
    stosd
    xchg   eax, a1
    stosd
    xchg   eax, a2
    stosd
    xchg   eax, a3
    stosd
nxr_end: 
    popfd   
    popad
    ret
    
nkncpy:
    pushad
    push   4
    pop    ecx
swp_l0:
    lodsd
    bswap  eax
    stosd
    loop   swp_l0
    mov    [esp+_edi], edi    
    popad
    ret
    
Noekeonx:
_Noekeonx:
    pushad
    lea    esi, [esp+32+4]
    pushad     ; allocate 32-bytes for state + key
    mov    edi, esp
    lodsd
    ; copy key to local buffer
    xchg   eax, esi
    call   nkncpy
    xchg   eax, esi
    lodsd
    ; copy data to local buffer
    xchg   eax, esi
    call   nkncpy
    xchg   eax, esi
    mov    ebp, eax
    lodsd
    cdq                      ; edx = 0
    xchg   ecx, eax          ; ecx = enc
    xchg   eax, edx          ; eax = 0
    mov    edi, esp          ; edi = Key
    lea    esi, [edi+16]     ; esi = State
    jecxz  nx_enc
    
    ; allocate a 16 null byte key
    pushad    
    push   eax
    push   eax
    push   eax
    push   eax
    mov    esi, edi          ; esi = Key
    mov    edi, esp          ; edi = NullVector 
    call   Thetax            ; Theta(Key, NullVector);
    add    esp, 16           ; release NullVector
    popad
    mov    al, Nr            ; i   = Nr    
nx_dec:
    test   eax, eax    
    call   Round
    dec    eax
    jns    nx_dec
    jmp    nx_xit    
nx_enc:
    cmp    al, Nr
    call   Round       ; Round(State, Key, enc, i, i==0);
    lea    eax, [eax+1]
    jnz    nx_enc
nx_xit:
    mov    edi, ebp
    call   nkncpy    
    popad
    popad
    ret

        
    
    