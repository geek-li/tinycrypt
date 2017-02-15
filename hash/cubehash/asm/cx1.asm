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
; CubeHash-256 with b=32, r=32
;
; size: 192 bytes
;
; global calls use cdecl convention
;
; -----------------------------------------------

  bits 32

  %ifndef BIN
    global cube_initx
    global _cube_initx
    
    global cube_updatex
    global _cube_updatex
    
    global cube_finalx
    global _cube_finalx
  %endif
    
; void cube_init(cube_ctx *c);
_cube_initx:
cube_initx:
    pushad
    mov    edi, [esp+32+4]   ; c
    mov    ebx, edi
    ; zero initialize state
    push   32
    pop    ecx
    xor    eax, eax
    pushad
    rep    stosd
    popad
    xchg   eax, ecx
    stosd                    ; c->x[0]=32
    stosd                    ; c->x[1]=32
    stosd                    ; c->x[2]=32
    db     0b0h              ; mov al, 0x60 to mask pushad
;
; expects ecx to hold loop count and ebx to hold context
;
transform:
    pushad
    pushad
    pushad                   ; create y[16]
    mov    edi, ebx          ; edi=c
    mov    esi, esp          ; esi=y
    imul   ebx, ecx, 10      ; *= 10
    mov    cl,  16
tx_l0:
    push   2
    pop    ebp               ; j=2
    push   7 
    pop    edx               ; k=7
    ; for (i=0; i<16; i++) 
    ;   c->x[i + 16] += c->x[i];
tx_l1:
    pushad
tx_l2:
    mov    eax, [edi]        ;
    add    [edi+64], eax     ; c->x[i+16] += c->x[i]
    scasd
    loop   tx_l2
    popad
    ; ---------------
    ; for (i=15; i>=0; --i) 
    ;   y[i ^ (j*4)] = c->x[i];
    pushad
    shl    ebp, 2              ; l = j << 2;
tx_l4:
    lea    ebx, [ecx-1]        ; ebx=i-1
    mov    eax, [edi+ebx*4]    ; eax=x[(i-1)]
    xor    ebx, ebp            ; 
    mov    [esi+ebx*4], eax    ; y[(i-1) ^ l] = *x
    loop   tx_l4
    popad
    ; ---------------
    ; for (x=c->x, y=t, i=16; i>0; --i, x++, y++) {
    ;   *x = ROTL32(*y, k);
    ; }
    pushad
    xchg   ecx, edx          ; edx=i, ecx=k
tx_l5:
    lodsd                    ; eax=*y
    rol    eax, cl           ; eax=ROTL32(*y, k)
    stosd                    ; *x=eax
    dec    edx               ; i--
    jnz    tx_l5
    popad
    ; for (i=0; i<16; i++) {
    ;   c->x[i] ^= c->x[i+16];
    ;}
    pushad
tx_l6:
    mov    eax, [edi]        ; eax = c->x[i]
    xor    eax, [edi+64]     ; eax ^= c->x[i+16]
    stosd                    ; c->x[i] = eax
    loop   tx_l6
    popad
    ; --------------
    ;  for (c=c->x, i=16; i>0; --i, x++) {
    ;    y[i ^ j] = x[16];
    ;  }
    pushad
tx_l7:
    lea    ebx, [ecx-1]
    mov    eax, [edi+ebx*4+64] ; eax = c->x[(i-1)+16]
    xor    ebx, ebp          ; ebx ^= j
    mov    [esi+ebx*4], eax  ; y[(i-1) ^ j] = eax
    loop   tx_l7
    popad
    ; --------------
    ; for (i=15; i>=0; --i) 
    ;   c->x[i + 16] = y[i];
    pushad
    add    edi, 64
    rep    movsd
    popad
    
    add    edx, 4            ; k += 4
    dec    ebp               ; j--
    jnz    tx_l1
    
    dec    ebx               ; n--
    jnz    tx_l0             ; while(n>0)
    
    popad           ; release y[16]
    popad
    popad
    ret
    
;void cube_update(cube_ctx *c, uint32_t inlen, 
; const void *in)
_cube_updatex:
cube_updatex:
    pushad
    lea    esi, [esp+32+ 4]
    lodsd
    xchg   eax, ebx          ; ebx=c
    lodsd
    push   eax               ; save in
    lodsd
    test   eax, eax
    pop    esi               ; esi=in
    jz     cb_l2             ; exit if no input
    xchg   edx, eax          ; edx=inlen
    push   10
    pop    ecx
cb_l1:
    lodsb    
    xor    [ebx], al         ; c->x[0] ^
    call   transform         ; transform(c, 1);
    dec    edx
    jnz    cb_l1             ; 
cb_l2:
    popad
    ret
    
; void cube_final(void *out, cube_ctx *c)
_cube_finalx:
cube_finalx:
    pushad
    mov    edi, [esp+32+4]   ; out
    mov    ebx, [esp+32+8]   ; c
    xor    byte[ebx], 80h        ; c->x[0] ^= 0x80            
    call   transform         ; transform(c, 1);
    xor    byte[ebx+31*4], 1   ; c->x[31] ^= 1;
    mov    cl, 10
    call   transform         ; transform(c, 10*32);
    ; save 32-bytes
    mov    cl, 8
    mov    esi, ebx
    rep    movsd             ; memcpy (out, c->v8, 32);
    popad
    ret
    