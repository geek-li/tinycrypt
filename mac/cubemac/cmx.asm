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
; size: 235 bytes for win64
;       228 bytes for linux (use -DNIX)
;
; -----------------------------------------------
    bits 64

    %ifndef BIN
      global cube_macx
    %endif
    
%ifdef NIX
  %define arg0 rdi
  %define arg1 rsi
  %define arg2 rdx 
  %define arg3 rcx ; r10 for syscalls
%else
  %define arg0 rcx
  %define arg1 rdx
  %define arg2 r8 
  %define arg3 r9
%endif

cube_macx:
      pop     r11       ; r11 = tag
      push    r11
      push    rbx
      push    rsi
      push    rdi
      push    rbp
      
      call    ld_pm
      ; ==================================
      ; permutation function
      ;
      ; edi = s
      sub     rsp, 64 
      push    rsp
      pop     rsi          ; rsi = y[16]
      ; for (n=16; n>0; n--)
      push    16
      pop     rcx
      ; for (k=7, j=2; j>0; k+=4, j--)
pm_l0:
      push    rcx               ; save n
      mov     cl, 16
      push    2
      pop     rbp               ; j=2
      push    7
      pop     rdx               ; k=7
pm_l1:
      ; **************************
      ; for (i=15; i>=0; --i)
      ;   s->w[i + 16] += s->w[i];
      ; **************************
      push    rdi
      mov     cl, 16      
pm_l2:
      mov     eax, [rdi]
      add     [rdi+64], eax
      scasd
      loop    pm_l2
      pop     rdi
      ; **************************
      ; for (i=15; i>=0; --i)
      ;   y[i ^ (j*4)] = s->w[i];
      ; **************************
      push    rbp
      mov     cl, 16      
      shl     ebp, 2
pm_l3:
      lea     ebx, [ecx-1]
      mov     eax, [rdi+rbx*4]
      xor     ebx, ebp
      mov     [rsi+rbx*4], eax
      loop    pm_l3
      pop     rbp
      ; **************************
      ; for (i=15; i>=0; --i)
      ;   s->w[i] = ROTL32(y[i], k);
      ; **************************
      push    rsi
      push    rdi
      push    rdx
      push    rdx
      pop     rcx
      mov     dl, 16      
pm_l4:
      lodsd
      rol     eax, cl
      stosd
      dec     rdx
      jnz     pm_l4
      pop     rdx
      pop     rdi
      pop     rsi
      ; **************************
      ; for (i=15; i>=0; --i)
      ;   s->w[i] ^= s->w[i + 16];
      ; **************************
      push    rdi
      mov     cl, 16      
pm_l5:
      mov     eax, [rdi]
      xor     eax, [rdi+64]
      stosd
      loop    pm_l5
      pop     rdi
      ; **************************
      ; for (i=15; i>=0; --i)
      ;   y[i ^ j] = s->w[i + 16];
      ; ************************** 
      mov     cl, 16      
pm_l6:
      lea     ebx, [ecx-1]
      mov     eax, [rdi+rbx*4+64]
      xor     ebx, ebp
      mov     [rsi+rbx*4], eax
      loop    pm_l6
      ; **************************
      ; for (i=15; i>=0; --i)
      ;   s->w[i + 16] = y[i];
      ; **************************
      mov     cl, 16      
      push    rdi
      push    rsi
      add     rdi, 64
      rep     movsd
      pop     rsi
      pop     rdi

      add     edx, 4          ; k += 4
      dec     rbp             ; j--
      jnz     pm_l1

      pop     rcx
      loop    pm_l0           ; will set CF to 0

      add     rsp, 64
      pop     rbx
      pop     rsi
      pop     rbp
      pop     rdi
      ret
      ; ==================================
ld_pm:
      pop     rbp           ; rbp = permute
      xor     eax, eax
      xor     ecx, ecx
      mov     cl, 128
      sub     rsp, rcx
      ; 1. initialize local state
      ; memset(&s, 0, sizeof(cube_state));
      push    rsp
      pop     rdi
      push    rsp
      pop     rsi      
      rep     stosb

      push    rsi
      pop     rdi
      ; s.w[0] = 16;
      mov     al, 16
      stosd
      ; s.w[1] = 32;
      mov     al, 32
      stosd
      ; s.w[2] = 16;
      mov     al, 16
      stosd
      ; permute(&s);
      call    rbp
      ; 2. absorb key
      ; 3. absorb message
      push    r10         ; save tag ptr
      push    r9          ; save msglen
      push    r8          ; save msg ptr
      push    rdx         ; save keylen
      push    rcx         ; save key ptr
      xor     ebx, ebx
cm_l0:
      pop     rdi         ; pop ptr
      pop     rcx         ; pop len
      ; ==================================
      ; absorb data into state
      ;
      ; rdi = data
      ; rcx = len
      ; rsi = s
absorb:
      xor     eax, eax      ; idx = 0
      jecxz   abs_l1        ; exit if len == 0
abs_l0:
      mov     dl, [rdi]     ; 
      scasb                 ; data++      
      xor     [rsi+rax], dl ; s->b[idx] ^= *data
      inc     al            ; idx++
      cmp     al, 32        ; absorbed block?
      loopne  abs_l0        ; while (al != 32 && ecx != 0)
      jne     abs_l1        ; if (al != 32 && ecx == 0) goto abs_l1
      call    rbp           ; permute(s)
      jmp     absorb        ; keep going
abs_l1:
      dec     rbx
      jp      cm_l0         ; loop twice
      
      ; 4. absorb end bit
      xor     byte[rsi+rax], 0x80
      call    rbp

      ; 5. absorb final bit
      xor     byte[rsi+31*4], 1
      call    rbp
      call    rbp

      ; 6. return 128-bit tag
      pop     rdi
      movsq
      movsq

      add     rsp, 128
      
      pop     rbp
      pop     rdi
      pop     rsi
      pop     rbx
      ret
