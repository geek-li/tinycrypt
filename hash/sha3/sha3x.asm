;
;  Copyright © 2015, 2016 Odzhan, Peter Ferrie. All Rights Reserved.
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
; SHA-3 in x86 assembly
;
; Derived from C code by Markku-Juhani O. Saarinen
;
; size: 458 bytes
;
; global calls use cdecl convention
;
; -----------------------------------------------
  bits 32

%define SHA3_ROUNDS    24  
%define SHA3_STATE_LEN 25

  %ifndef BIN
    global SHA3_Initx
    global _SHA3_Initx
    
    global SHA3_Updatex
    global _SHA3_Updatex
    
    global SHA3_Finalx
    global _SHA3_Finalx
  %endif
  
; ***********************************************
;
; SHA3_Init (&ctx, int);
;
; ***********************************************
SHA3_Initx:
_SHA3_Initx:
    pushad
    mov    edi, [esp+32+4]    ; ctx
    mov    eax, [esp+32+8]    ; mdlen
    stosd                     ; c->olen=mdlen
    add    eax, eax           ; *= 2
    push   (SHA3_STATE_LEN*8)/2
    pop    ecx
    add    ecx, ecx           ; ecx=200
    neg    eax                ; negate
    add    eax, ecx           ; add 200
    stosd                     ; blen = 200 - (2 * mdlen)
    xor    eax, eax
    stosd                     ; c->idx=0
    rep    stosb              ; zero the state buffer
    popad
    ret

; ***********************************************
;
; SHA3_Update (SHA3_CTX*, void*, uint32_t);
;
; ***********************************************
SHA3_Updatex:
_SHA3_Updatex:
    pushad
    lea    esi, [esp+32+4]
    lodsd
    push   eax               ; save ctx
    lodsd
    xchg   ebx, eax          ; ebx = input
    lodsd
    xchg   ecx, eax          ; ecx = len
    pop    esi               ; esi = ctx
    jecxz  upd_l2
    
    lodsd                    ; skip c->olen
    lodsd                    ;
    xchg   eax, ebp          ; ebp = c->blen
    push   esi               ; save ptr to ctx->idx
    lodsd                    ; eax = ctx->idx
upd_l0:
    mov    dl, [ebx]         ; absorb byte
    inc    ebx    
    xor    byte[esi+eax], dl
    inc    eax               ; c->idx++
    cmp    eax, ebp
    jnz    upd_l1
    call   SHA3_Transform    ; compress, expects c->state in esi
    xor    eax, eax          ; c->idx = 0
upd_l1:
    loop   upd_l0
    pop    edi
    stosd
upd_l2:
    popad
    ret
  
; ***********************************************
;
; SHA3_Final (void*, SHA3_CTX*);
;
; ***********************************************
SHA3_Finalx:
_SHA3_Finalx:
    pushad
    mov    esi, [esp+32+8]      ; esi=c
    lodsd
    xchg   ecx, eax             ; ecx=c->olen
    lodsd
    xchg   edx, eax             ; edx=c->blen
    lodsd                       ; eax=c->idx
    xor    byte[esi+eax], 6     ; ctx->state.v8[c->idx] ^= 6;
    xor    byte[esi+edx-1], 80h ; ctx->state.v8[c->blen-1] ^= 0x80;
    call   SHA3_Transform       ; SHA3_Transform (c->state);
    mov    edi, [esp+32+4]      ; edi=out
    rep    movsb                ; memcpy (out, c->state.v8, c->olen);
    popad
    ret

%define r    ebp
%define i    ecx
%define ih   ch
%define lfsr edx
%define j    ebx
%define t    mm0
%define _st  esi
%define _bc  edi

; ***********************************************
;
; SHA3_Transform (SHA3_CTX*);
;
; expects c->state in esi
; ***********************************************
SHA3_Transform:
    pushad
    
    ; set up workspace of 64-bytes
    pushad
    pushad
    mov    _bc, esp
    
    push   SHA3_ROUNDS
    pop    r
    
    push   1
    pop    lfsr

    call   s3_l2
sha3_mod5:
    db 0, 1, 2, 3, 4, 0, 1, 2, 3, 4
sha3_piln:
    db 10, 7,  11, 17, 18, 3, 5,  16, 8,  21, 24, 4 
    db 15, 23, 19, 13, 12, 2, 20, 14, 22, 9,  6,  1  
s3_l2:
    push   r
    push   lfsr
    ; Theta
    ; for (i = 0; i < 5; i++)     
    ;   bc[i] = st[i + 0 ] ^ 
    ;           st[i + 5 ] ^ 
    ;           st[i + 10] ^ 
    ;           st[i + 15] ^ 
    ;           st[i + 20]; 
    xor    eax, eax
s3_l3:
    movq    t, [_st+8*eax+20*8]
    pxor    t, [_st+8*eax+15*8]
    pxor    t, [_st+8*eax+10*8]
    pxor    t, [_st+8*eax+ 5*8]
    pxor    t, [_st+8*eax     ]
    movq    [_bc+8*eax        ], t
    inc     eax
    cmp     al, 5
    jnz     s3_l3
      
    ; for (i = 0; i < 5; i++) {
    ;   t = bc[(i + 4) % 5] ^ ROTL64(bc[(i+1)%5], 1);
    ;   for (j = 0; j < 25; j += 5)
    ;     st[j + i] ^= t;
    ; }
    ; ************************************
    ; for (i = 0; i < 5; i++)
    xor    i, i
s3_l4:
    ; t = ROTL64(bc[(i + 1) % 5], 1)
    mov    eax, [esp+2*4]
    push   eax
    movzx  eax, byte [eax + i + 1]
    mov    edx, [_bc+8*eax]
    mov    ebp, [_bc+8*eax+4]
    add    edx, edx
    adc    ebp, ebp
    adc    dl, ih
    ; bc[(i + 4) % 5]
    pop    eax      ; keccakf_mod5
    movzx  eax, byte [eax + i + 4]
    xor    edx, [_bc+8*eax]
    xor    ebp, [_bc+8*eax+4]
    ; for (j = 0; j < 25; j += 5)
    xor    j, j
s3_l5:
    ; st[j + i] ^= t;
    lea    eax, [j+i]
    xor    [_st+8*eax], edx
    xor    [_st+8*eax+4], ebp
    add    j, 5
    cmp    j, 25
    jnz    s3_l5
    
    inc    i
    cmp    i, 5
    jnz    s3_l4
            
    ; // Rho Pi
    ; t = st[1];
    ; for (i = 0; i < 24; i++) {
    ;   j = keccakf_piln[i];
    ;   bc[0] = st[j];
    ;   st[j] = ROTL64(t, keccakf_rotc[i]);
    ;   t = bc[0];
    ; }
    ; *************************************
    ; t = st[1]
    mov    edx, [_st+8]
    mov    ebp, [_st+8+4]
    xor    i, i
    ; for (i = 0; i < 24; i++)
    push   i
s3_l6:
    push   ecx
    ; j = keccakf_piln[i];
    mov    eax, [esp+4*4]
    movzx  j, byte [eax + i + (sha3_piln - sha3_mod5)]
    mov    eax, [esp+4]
    lea    ecx, [eax+i+1]
    mov    [esp+4], ecx
    and    ecx, 63
    ;mov    cl, byte [eax + i + (sha3_rotc - sha3_piln)]
    ; st[j] = ROTL64(t, keccakf_rotc[i]);
s3_l06x:
    add    edx, edx
    adc    ebp, ebp
    adc    dl, ih
    loop   s3_l06x
    ; bc[0] = st[j];
    xchg   [_st+8*j], edx
    mov    [_bc], edx
    xchg   [_st+8*j+4], ebp
    mov    [_bc+4], ebp
    pop    ecx
    inc    i
    cmp    i, 24
    jnz    s3_l6
    pop    eax
      
    ; // Chi
    ; for (j = 0; j < 25; j += 5) {
    ;   for (i = 0; i < 5; i++)
    ;     bc[i] = st[j + i];
    ;   for (i = 0; i < 5; i++)
    ;     st[j + i] ^= (~bc[(i+1)%5]) & bc[(i+2)%5];
    ; }
    ; *********************************
    ; for (j=0; j<25; j+=5)
    xor    j, j
s3_l7:
    ; for (i=0; i<5; i++)
    xor    i, i
s3_l8:
    ; bc[i] = st[j + i];
    lea    eax, [j+i]
    movq   t, [_st+8*eax]
    movq   [_bc+8*i], t
    inc    i
    cmp    i, 5
    jnz    s3_l8
        
    ; for (i=0; i<5; i++)
    xor    i, i
s3_l9:
    ; st[j + i] ^= (~bc[(i+1)%5]) & bc[(i+2)%5];
    mov    eax, [esp+2*4] ; keccakf_mod5
    push   eax 
    movzx  eax, byte [eax + i + 1]
    movq   t, [_bc+8*eax]
    pop    eax            ; keccakf_mod5 
    movzx  eax, byte [eax + i + 2]
    pandn  t, [_bc+8*eax]
    lea    eax, [j+i]
    pxor   t, [_st+8*eax]
    movq   [_st+8*eax], t
    inc    i
    cmp    i, 5
    jnz    s3_l9
    
    add    j, 5
    cmp    j, 25
    jnz    s3_l7
           
    pop    lfsr

    ; // Iota
    ; st[0] ^= keccakf_rndc[round];
    movq    mm4, [_st]
;;    call    rc
rc:
    pxor   mm0, mm0        ; c=0
    pxor   mm1, mm1        ; 
    push   1
    pop    eax             ; i=1
    movd   mm1, eax
rc_l00:
    test   dl, 1           ; (t & 1)
    jz     rc_l01
    ; ecx = (i - 1)
    lea    ecx, [eax-1]
    movd   mm2, ecx
    movq   mm3, mm1
    ; 1ULL << (i - 1)
    psllq  mm3, mm2
    pxor   mm0, mm3        ; c ^= 1ULL << (i - 1)
rc_l01:
    add    dl, dl          ; t += t
    jnc    rc_l02
    xor    dl, 71h
rc_l02:
    add    al, al          ; i += i
    jns    rc_l00
;;    ret
  
    pxor    mm4, t
    movq    [_st], mm4
    
    pop     r
    dec     r
    jnz     s3_l2
    
    pop     eax
    popad
    popad
    popad
    ret
    