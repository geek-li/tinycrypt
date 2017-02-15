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
; CubeMAC128 with b=32, r=16
;
; size: 197 bytes
;
; global calls use cdecl convention
;
; -----------------------------------------------
    bits 32

    %ifndef BIN
      global cube_macx
      global _cube_macx
    %endif

cube_macx:
_cube_macx:
      pushad
      call   ld_pm
      ; ==================================
      ; permutation function
      ;
      ; edi = s
      pushad                   ; save registers
      pushad                   ; allocate 32-bytes
      pushad                   ; allocate 32-bytes
      mov    esi, esp          ; esi = y[16]
      ; for (n=16; n>0; n--)
      push   16
      pop    ecx
      ; for (k=7, j=2; j>0; k+=4, j--)
pm_l0:
      push   ecx               ; save n
      mov    cl, 16
      push   2
      pop    ebp               ; j=2
      push   7
      pop    edx               ; k=7
pm_l1:
      ; **************************
      ; for (i=15; i>=0; --i)
      ;   s->w[i + 16] += s->w[i];
      ; **************************
      pushad
pm_l2:
      mov    eax, [edi]
      add    [edi+64], eax
      scasd
      loop   pm_l2
      popad
      ; **************************
      ; for (i=15; i>=0; --i)
      ;   y[i ^ (j*4)] = s->w[i];
      ; **************************
      pushad
      shl    ebp, 2
pm_l3:
      lea    ebx, [ecx-1]
      mov    eax, [edi+ebx*4]
      xor    ebx, ebp
      mov    [esi+ebx*4], eax
      loop   pm_l3
      popad
      ; **************************
      ; for (i=15; i>=0; --i)
      ;   s->w[i] = ROTL32(y[i], k);
      ; **************************
      pushad
      xchg   ecx, edx
pm_l4:
      lodsd
      rol    eax, cl
      stosd
      dec    edx
      jnz    pm_l4
      popad
      ; **************************
      ; for (i=15; i>=0; --i)
      ;   s->w[i] ^= s->w[i + 16];
      ; **************************
      pushad
pm_l5:
      mov    eax, [edi]
      xor    eax, [edi+64]
      stosd
      loop   pm_l5
      popad
      ; **************************
      ; for (i=15; i>=0; --i)
      ;   y[i ^ j] = s->w[i + 16];
      ; **************************
      pushad
pm_l6:
      lea    ebx, [ecx-1]
      mov    eax, [edi+ebx*4+64]
      xor    ebx, ebp
      mov    [esi+ebx*4], eax
      loop   pm_l6
      popad
      ; **************************
      ; for (i=15; i>=0; --i)
      ;   s->w[i + 16] = y[i];
      ; **************************
      pushad
      add    edi, 64
      rep    movsd
      popad

      add    edx, 4          ; k += 4
      dec    ebp             ; j--
      jnz    pm_l1

      pop    ecx
      loop   pm_l0           ; will set CF to 0

      popad                  ; release 32-bytes
      popad                  ; release 32-bytes
      popad                  ; restore registers
      ret
      ; ==================================
ld_pm:
      pop    ebp           ; ebp = permute
      lea    esi, [esp+32+4]
      xor    eax, eax
      xor    ecx, ecx
      mov    cl, 128
      sub    esp, ecx
      ; 1. initialize local state
      ; memset(&s, 0, sizeof(cube_state));
      mov    edi, esp
      rep    stosb

      mov    edi, esp
      push   edi
      ; s.w[0] = 16;
      mov    al, 16
      stosd
      ; s.w[1] = 32;
      mov    al, 32
      stosd
      ; s.w[2] = 16;
      mov    al, 16
      stosd
      pop    edi
      ; permute(&s);
      call   ebp
      ; 2. absorb key
      ; 3. absorb message
cm_l0:
      pushfd      
      lodsd
      xchg   ebx, eax        ; ebx = data
      lodsd
      xchg   ecx, eax        ; ecx = len
      ; ==================================
      ; absorb data into state
      ;
      ; ebx = data
      ; ecx = len
      ; edi = s
absorb:
      xor    eax, eax      ; idx = 0
      jecxz  abs_l1        ; exit if len == 0
abs_l0:
      mov    dl, [ebx]
      xor    [edi+eax], dl ; s->b[idx] ^= *data
      inc    eax           ; idx++
      inc    ebx           ; data++
      cmp    al, 32        ; absorbed block?
      loopne abs_l0        ; while (al != 32 && ecx != 0)
      jne    abs_l1        ; if (al != 32 && ecx == 0) goto abs_l1
      call   ebp           ; permute(s)
      jmp    absorb        ; keep going
abs_l1:
      popfd
      cmc                  ; CF = !CF 
      jc     cm_l0         ; loop twice
      
      ; 4. absorb end bit
      xor    byte[edi+eax], 0x80
      call   ebp

      ; 5. absorb final bit
      xor    byte[edi+31*4], 1
      call   ebp
      call   ebp

      ; 6. return 128-bit tag
      lodsd
      xchg   eax, edi        ; edi = tag
      xchg   eax, esi        ; esi = s
      mov    cl, 16
      rep    movsb

      ; release stack
      pop    eax
      add    esp, 124
      popad
      ret
