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
; AES-256 block cipher in x86-64 assembly
;
; Written by Odzhan and Peter Ferrie
;
; http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
;
; size: 441 bytes (462 bytes side-channel resistant)
;
; global calls use cdecl convention
;
; -----------------------------------------------

; The first four integer or pointer parameters are passed in the rcx, rdx, r8, and r9 registers. 
;
; rax, rcx, rdx, r8-r11 are volatile.
; rbx, rbp, rdi, rsi, r12-r15 are nonvolatile.

    bits 64

%define Nk 8     ; number of words for each sub key
%define Nr 14    ; number of rounds for 256-bit
%define Nb 4     ; number of words in each block

%ifndef BIN
    global aes_setkeyx
    global _aes_setkeyx
    
    global aes_encryptx
    global _aes_encryptx
%endif

; *********************************************** 
; void aes_encrypt (aes_ctx *ctx, void *state, int enc)
; *********************************************** 
_aes_encryptx:
aes_encryptx:
    ; pushad
    push   rbx
    push   rdi
    push   rsi
    push   rbp
    
    push   rcx
    pop    rdi              ; rdi=ctx
    
    push   rdx
    pop    rsi              ; rsi=state
    
    push   r8
    pop    rcx              ; rcx=enc
    
    call   ld_fn
; *********************************************** 
; void AddRoundKey (void *state, uint32_t w[], int rnd);
; ***********************************************
AddRoundKey:
    ; pushad
    push   rax
    push   rbx
    push   rcx
    push   rdi
    push   rsi
    
    mov    cl, 4          ; do 4 words
    shl    ebx, cl        ; multiply rnd by 16
ar_l1:
    mov    eax, [rdi+rbx] ; w[i+rnd*16]
    xor    [rsi], eax     ; state[i] ^= w[i+rnd*16]
    cmpsd
    loop   ar_l1
    
    pop    rsi
    pop    rdi
    pop    rcx
    pop    rbx
    pop    rax
    ; popad
    ret
; ***********************************************     
; void ShiftRows (void *state, int enc)
; *********************************************** 
ShiftRows:
    ; pushad
    push   rax
    push   rbx
    push   rcx
    push   rdx
    push   rsi
    push   rdi
    push   rbp
    
    xor    ebx, ebx          ; i = 0
    mov    ebp, ecx          ; save enc flag in ebp
    mov    cl, 4             ; do 4 rows
sr_l1:
    ; pushad
    push   rsi
    push   rcx
    
    push   rsi
    pop    rdi
    mov    cl, 4             
sr_l2:                       ; get row
    lodsd
    call   SubByte
    shrd   edx, eax, 8
    loop   sr_l2
    ; ---------------------
    lea    ecx, [ecx+ebx*8]
    test   ebp, ebp
    jz     sr_d
    ror    edx, cl           ; rotate right for encryption
    jmp    sr_l4
sr_d:
    rol    edx, cl           ; rotate left for decryption
sr_l4:
    mov    cl, 4
sr_l5:
    mov    [rdi], dl
    scasd
    ror    edx, 8
    loop   sr_l5
    
    pop    rcx
    pop    rsi
    
    ; popad
    inc    ebx
    inc    rsi
    loop   sr_l1
    
    pop    rbp
    pop    rdi
    pop    rsi
    pop    rdx
    pop    rcx
    pop    rbx
    pop    rax
    ; popad
    ret

%define w0 eax
%define w1 ebx
%define w2 edx
%define w3 ebp

%define t  eax
%define w  ebp

gf_mul2:
    push   rax
    mov    t, w
    and    t, 080808080h
    xor    w, t
    add    w, w
    shr    t, 7
    imul   t, t, 01bh
    xor    w, t
    pop    rax
    ret
; ***********************************************
; void MixColumns (void *state, int enc)
; ***********************************************
MixColumns:
    ; pushad
    push   rax
    push   rbx
    push   rcx
    push   rdx
    push   rdi
    push   rsi
    push   rbp
    
    lea    rdi, [rbp+(gf_mul2-ShiftRows)]
    test   ecx, ecx
    mov    cl, 4
mc_l1:
    pushfq
    lodsd                    ; w0 = state[i];
    jne    mc_l2
    mov    w3, w0
    ror    w3, 16
    xor    w3, w0
    call   rdi               ; gf_mul2
    call   rdi               ; gf_mul2
    xor    w0, w3
mc_l2:
    mov    w3, w0
    ror    w3, 8
    mov    w1, w3
    xor    w3, w0
    call   rdi
    xor    w1, w3
    ror    w0, 16
    xor    w1, w0
    ror    w0, 8
    xor    w1, w0
    mov    [rsi-4], w1
    popfq
    loop   mc_l1
    
    pop    rbp
    pop    rsi
    pop    rdi
    pop    rdx
    pop    rcx
    pop    rbx
    pop    rax
    
    ; popad
    ret
    
ld_fn:
    pop    rax
    push   rax
    pop    rbp
    add    rbp, (ShiftRows - AddRoundKey)
    push   rbp
    pop    rdx
    add    rdx, (MixColumns - ShiftRows)
    
    ;lea    rbp, [rax+(ShiftRows -AddRoundKey)]
    ;lea    rdx, [rbp+(MixColumns-ShiftRows)]
    
    ; ************************************
    push   Nr
    pop    rbx
    jecxz  do_ark
    xor    ebx, ebx
do_ark:
    call   rax ; AddRoundKey
    push   rax
    jecxz  aes_edm
    xchg   rdx, rax
    ; jmp    aes_edm
    db     03dh ; mask call eax/call edx

aes_ed:
    call   rax ; MixColumns / AddRoundKey
    call   rdx ; AddRoundKey / MixColumns

aes_edm:
    call   rbp ; ShiftRows + SubBytes
    dec    ebx
    jecxz  aes_edl
    inc    ebx
    inc    ebx
    cmp    ebx, Nr
aes_edl:
    jne    aes_ed
    pop    rax
    call   rax ; AddRoundKey
    
    pop    rbp
    pop    rsi
    pop    rdi
    pop    rbx
    
    ; popad
    ret
    
; *********************************************** 
; void aes_setkey (aes_ctx *ctx, void *key);
; *********************************************** 
_aes_setkeyx:
aes_setkeyx:
    ; pushad
    ;int3
    push   rbx
    push   rdi
    push   rsi
    push   rbp
    
    push   rcx
    pop    rdi  ; rdi=ctx
    
    push   rdx
    pop    rsi  ; rsi=key
    
    push   Nk
    pop    rcx
    rep    movsd

    push   1
    pop    rdx                ; rcon = 1
    
    push   rdx
    pop    rbp                ; rbp=AES_ENCRYPT
sk_l1:
    mov    eax, [rdi-4]       ; x=w[i-1];
    test   cl, Nk-1           ; (i % Nk)==0
    jnz    sk_ei
    
    ror    eax, 8             ; x = RotWord(x);
    call   SubWord            ; x = SubWord(x)
    xor    eax, edx           ; x ^= rcon;
    add    dl, dl
%ifdef SCR
    sbb    bl, bl
    and    bl, 1bh
    xor    dl, bl
%else
    jnc    sk_sw
    xor    dl, 1bh
%endif
    jmp    sk_sw
sk_ei:
    test   cl, 3              ; (i % 4)==0
    jne    sk_sw
    call   SubWord
sk_sw:
    xor    eax, [rdi-4*Nk]
    stosd
    inc    ecx
    cmp    ecx, Nb*Nr
    jne    sk_l1
    
    pop    rbp
    pop    rsi
    pop    rdi
    pop    rbx
    
    ; popad
    ret
; ***********************************************
; uint8_t SubByte (uint8_t x, int enc)
; *********************************************** 
SubByte:
    ; pushad
    push   rax
    push   rbx
    push   rcx
    push   rdx
    push   rbp
    
    test   ebp, ebp
    mov    cl, 4
    jz     sb_inv
    call   gf_mulinv
sb_l1:
    rol    dl, 1
    xor    al, dl
    loop   sb_l1
sb_inv:
    xor    al, 63h
    jecxz  xit_sb
    rol    al, 1
    mov    dl, al
    rol    dl, 2
    xor    al, dl
    rol    dl, 3
    xor    al, dl
    call   gf_mulinv
xit_sb:
    pop    rbp
    pop    rdx
    pop    rcx
    pop    rbx
    mov    byte[rsp], al
    pop    rax
    ; popad
    ret
; ***********************************************
; uint32_t SubWord (uint32_t x)
; *********************************************** 
SubWord:
    ; for (i=0; i<4; i++) {
    ;   r |= SubByte(x & 0xFF, AES_ENCRYPT);
    ;   r  =_rotr(r, 8);
    ;   x >>= 8;
    ; }
    push   rcx
    mov    cl, 4
sbw_l1:
    call   SubByte
    ror    eax, 8
    loop   sbw_l1
    pop    rcx
    ret
; ***********************************************    
; uint8_t gf_mulinv (uint8_t x);
; *********************************************** 
gf_mulinv: 
    test   al, al
    xchg   edx, eax
    je     ret_y
    push   1
    pop    rbx          ; i=0, y=1
%ifdef SCR
    xor    esi, esi
    xor    edi, edi
%endif
gf_l1:
    mov    ebp, ebx     ;
    call   gf_mul2
    xchg   ebp, eax
    xor    bl, al       ; y ^= gf_mul2 (y);
    inc    bh           ; i++
    cmp    dl, bl       ; if (y==x) break;
%ifdef SCR
    cmove  esi, ebx     ; assign esi on every match
    test   edi, edi
    cmove  edi, esi     ; assign edi on first match
    test   bh, bh
    jne    gf_l1
    mov    bl, 1        ; y=1
%else
    jne    gf_l1
    mov    dl, 1        ; y=1
%endif
    je     gf_l3
gf_l2:    
    xchg   ebp, eax
    call   gf_mul2
    xchg   ebp, eax
    xor    dl, al       ; y ^= gf_mul2(y);
gf_l3:    
%ifdef SCR
    cmp    ebx, edi
    cmovbe edx, ebx
%endif
    inc    bh           ; i++
ret_y:
    mov    al, dl
    jne    gf_l2
    ret
