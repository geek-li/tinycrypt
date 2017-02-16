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
; SM4-128 block cipher in x86 assembly
;
; size: ???
;
; global calls use cdecl convention
;
; -----------------------------------------------

    bits 32
   
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

%define k0 ebx
%define k1 edx
%define k2 ebp
%define k3 esi

sm4_setkey:
_sm4_setkey:
    pushad
    mov    edi, [esp+32+4]  ; edi = ctx
    mov    esi, [esp+32+8]  ; esi = 128-bit key
    ; load the key
    lodsd
    bswap  eax
    xchg   eax, k0
    lodsd
    bswap  eax
    xchg   eax, k1
    lodsd
    bswap  eax
    xchg   eax, k2
    lodsd
    bswap  eax
    xchg   eax, k3
    
    ; xor FK values
    xor    k0, 0xa3b1bac6    
    xor    k1, 0x56aa3350    
    xor    k2, 0x677d9197    
    xor    k3, 0xb27022dc
    
    ; setup 4 sub keys
    call   T_function
    xor    eax, k0
    stosd
    call   T_function
    xor    eax, k1
    stosd
    call   T_function
    xor    eax, k2
    stosd
    call   T_function
    xor    eax, k3
    stosd
k_l0:
    mov    eax, ecx
    cdq
k_l1:
    shl    eax, 8
    imul   eax, ecx, 4
    add    eax, ecx
    imul   eax, eax, 7
    or     al, dl    
    inc    cl
    cmp    cl, 4
    jnz    k_l1
    
    xor    eax, [edi- 4] ; rk[i-1]
    xor    eax, [edi- 8] ; rk[i-2]
    xor    eax, [edi-12] ; rk[i-3]
    clc
    call   T_function  
    xor    eax, [edi-16] ; rk[i-4]
    stosd
    inc    ecx
    cmp    ecx, 32
    jnz    k_l0       
    popad
    ret
    
%define x0 ebx
%define x1 edx 
%define x2 ebp 
%define x3 esi
  
sm4_encrypt:
_sm4_encrypt:
    pushad
    mov    edi, [esp+32+4] ; edi = ctx
    mov    esi, [esp+32+8] ; esi = buf
    push   esi ; save buffer for later
    
    ; load data
    lodsd
    bswap  eax
    xchg   eax, x0
    lodsd
    bswap  eax    
    xchg   eax, x1
    lodsd
    bswap  eax    
    xchg   eax, x2
    lodsd
    bswap  eax    
    xchg   eax, x3
    
    ; do 32 rounds
    push   32
    pop    ecx
e_l0:
    ; apply F round
    mov    eax, x1
    xor    eax, x2
    xor    eax, x3
    xor    eax, [edi] ; rk[i]
    scasd
    stc
    call   T_function     
    xor    x0, eax
    xchg   x0, x1
    xchg   x1, x2
    xchg   x2, x3
    loop   e_l0
    
    ; save data
    pop    edi
    xchg   eax, x3
    bswap  eax    
    stosd
    xchg   eax, x2
    bswap  eax    
    stosd
    xchg   eax, x1
    bswap  eax    
    stosd
    xchg   eax, x0
    bswap  eax    
    stosd    
    popad
    ret
    
t_l0:
    pop    ebx
    ; apply non-linear substitution
    mov    cl, 4
t_l1:    
    xlatb
    ror    eax, 8
    loop   t_l1
    mov    ebx, eax
    mov    ecx, eax
    mov    edx, eax
    mov    ebp, eax
    ; apply linear substitution
    popfd
    jc     t_l2
    ; for key setup
    rol    ebx, 13
    rol    ecx, 23
    xor    eax, ebx
    xor    eax, ecx
    jmp    t_l3
t_l2:    
    ; for encryption
    rol    ebx, 2
    rol    ecx, 10
    rol    edx, 18
    rol    ebp, 24
    
    xor    eax, ebx
    xor    eax, ecx
    xor    eax, edx
    xor    eax, ebp
t_l3:
    mov    [esp+_eax], eax    
    popad
    ret
    
; in:  eax
; out: eax  
T_function:
    pushad
    pushfd
    call   t_l0  ; pushes address of sbox on stack
    
    ; sbox for SM4
    db 0xd6, 0x90, 0xe9, 0xfe, 0xcc, 0xe1, 0x3d, 0xb7 
    db 0x16, 0xb6, 0x14, 0xc2, 0x28, 0xfb, 0x2c, 0x05
    db 0x2b, 0x67, 0x9a, 0x76, 0x2a, 0xbe, 0x04, 0xc3 
    db 0xaa, 0x44, 0x13, 0x26, 0x49, 0x86, 0x06, 0x99
    db 0x9c, 0x42, 0x50, 0xf4, 0x91, 0xef, 0x98, 0x7a 
    db 0x33, 0x54, 0x0b, 0x43, 0xed, 0xcf, 0xac, 0x62
    db 0xe4, 0xb3, 0x1c, 0xa9, 0xc9, 0x08, 0xe8, 0x95 
    db 0x80, 0xdf, 0x94, 0xfa, 0x75, 0x8f, 0x3f, 0xa6
    db 0x47, 0x07, 0xa7, 0xfc, 0xf3, 0x73, 0x17, 0xba 
    db 0x83, 0x59, 0x3c, 0x19, 0xe6, 0x85, 0x4f, 0xa8
    db 0x68, 0x6b, 0x81, 0xb2, 0x71, 0x64, 0xda, 0x8b 
    db 0xf8, 0xeb, 0x0f, 0x4b, 0x70, 0x56, 0x9d, 0x35
    db 0x1e, 0x24, 0x0e, 0x5e, 0x63, 0x58, 0xd1, 0xa2 
    db 0x25, 0x22, 0x7c, 0x3b, 0x01, 0x21, 0x78, 0x87
    db 0xd4, 0x00, 0x46, 0x57, 0x9f, 0xd3, 0x27, 0x52 
    db 0x4c, 0x36, 0x02, 0xe7, 0xa0, 0xc4, 0xc8, 0x9e
    db 0xea, 0xbf, 0x8a, 0xd2, 0x40, 0xc7, 0x38, 0xb5 
    db 0xa3, 0xf7, 0xf2, 0xce, 0xf9, 0x61, 0x15, 0xa1
    db 0xe0, 0xae, 0x5d, 0xa4, 0x9b, 0x34, 0x1a, 0x55 
    db 0xad, 0x93, 0x32, 0x30, 0xf5, 0x8c, 0xb1, 0xe3
    db 0x1d, 0xf6, 0xe2, 0x2e, 0x82, 0x66, 0xca, 0x60 
    db 0xc0, 0x29, 0x23, 0xab, 0x0d, 0x53, 0x4e, 0x6f
    db 0xd5, 0xdb, 0x37, 0x45, 0xde, 0xfd, 0x8e, 0x2f 
    db 0x03, 0xff, 0x6a, 0x72, 0x6d, 0x6c, 0x5b, 0x51
    db 0x8d, 0x1b, 0xaf, 0x92, 0xbb, 0xdd, 0xbc, 0x7f 
    db 0x11, 0xd9, 0x5c, 0x41, 0x1f, 0x10, 0x5a, 0xd8
    db 0x0a, 0xc1, 0x31, 0x88, 0xa5, 0xcd, 0x7b, 0xbd 
    db 0x2d, 0x74, 0xd0, 0x12, 0xb8, 0xe5, 0xb4, 0xb0
    db 0x89, 0x69, 0x97, 0x4a, 0x0c, 0x96, 0x77, 0x7e 
    db 0x65, 0xb9, 0xf1, 0x09, 0xc5, 0x6e, 0xc6, 0x84
    db 0x18, 0xf0, 0x7d, 0xec, 0x3a, 0xdc, 0x4d, 0x20 
    db 0x79, 0xee, 0x5f, 0x3e, 0xd7, 0xcb, 0x39, 0x48
      