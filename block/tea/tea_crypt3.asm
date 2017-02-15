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
    
%define TEA_ROUNDS 32
%define TEA_DELTA  09e3779b9h

%ifndef BIN
  global TEA_Encryptx
  global _TEA_Encryptx
%endif

_TEA_Encryptx:
TEA_Encryptx:
  pushad
  lea    esi, [esp+32+4]
  lodsd
  push   eax
  lodsd
  xchg   eax, edi
  lodsd
  xchg   eax, ecx
  pop    esi
  push   esi
  ; eax=v[0]
  lodsd
  xchg   eax, ebx            
  ; ebx=v[1]
  lodsd
  xchg   eax, ebx
  test   ecx, ecx
  ; for (i=32; i>0; i--)
  mov    cl, 32
  jnz    tea_dec
  
  ; sum = 0
  xor    ebp, ebp
t_el:
  ; sum += delta
  add    ebp, TEA_DELTA
  clc
  push   edi
t_ev:
  pushfd
  ; v1 << 4 + k0
  mov    edx, ebx
  shl    edx, 4
  add    edx, [edi]
  scasd
  ; v1 + sum
  mov    esi, ebx
  add    esi, ebp
  xor    edx, esi
  ; v1 >> 5 + k1
  mov    esi, ebx
  shr    esi, 5
  add    esi, [edi]
  scasd
  xor    edx, esi
  ; v0 += ((v1<<4) + k0) ^
  ;        (v1+sum) ^
  ;       ((v1>>5) + k1)
  add    eax, edx
  popfd
  xchg   eax, ebx
  cmc
  jc     t_ev
  pop    edi
  loop   t_el
save_r:
  pop    edi
  ; v[0] = v0
  stosd
  xchg   eax, ebx
  ; v[1] = v1
  stosd
  popad
  ret

tea_dec:
  ; sum = delta << 5
  mov    ebp, (TEA_DELTA << 5) & 0xFFFFFFFF
t_dl:
  clc
  std
  push   edi
  add    edi, 12
t_dv:
  pushfd
  ; v0 << 4 + k2
  mov    edx, eax
  shl    edx, 4
  scasd
  add    edx, [edi]
  ; v0 + sum
  mov    esi, eax
  add    esi, ebp
  xor    edx, esi
  ; v0 >> 5 + k3
  mov    esi, eax
  shr    esi, 5
  add    esi, [edi+4]
  scasd
  xor    edx, esi
  ; v1 -= ((v0<<4) + k2) ^
  ;        (v0+sum) ^
  ;       ((v0>>5) + k3)
  sub    ebx, edx
  xchg   eax, ebx
  popfd
  cmc
  jc     t_dv
  ; sum -= delta
  sub    ebp, TEA_DELTA
  pop    edi
  cld
  loop   t_dl
  jmp    save_r
