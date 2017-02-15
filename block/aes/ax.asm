;
;  Copyright © 2015 Odzhan, Peter Ferrie. All Rights Reserved.
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
; AES-256 block cipher in x86 assembly
;
; http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
;
; size: 377 bytes (398 bytes side-channel resistant)
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
    pushad
    lea    esi, [esp+32+ 4]
    lodsd
    xchg   edi, eax         ; ctx
    lodsd
    xchg   ecx, eax
    lodsd
    xchg   ecx, eax         ; enc
    xchg   esi, eax         ; state
    call   ld_fn
; *********************************************** 
; void AddRoundKey (void *state, uint32_t w[], int rnd);
; ***********************************************
AddRoundKey:
    pushad
    mov    cl, 4          ; do 4 words
    shl    ebx, cl        ; multiply rnd by 16
ar_l1:
    mov    eax, [edi+ebx] ; w[i+rnd*16]
    xor    [esi], eax     ; state[i] ^= w[i+rnd*16]
    cmpsd
    loop   ar_l1
    popad
    ret
; ***********************************************     
; void ShiftRows (void *state, int enc)
; *********************************************** 
ShiftRows:
    pushad
    xor    ebx, ebx          ; i = 0
    mov    ebp, ecx          ; save enc flag in ebp
    mov    cl, 4             ; do 4 rows
sr_l1:
    pushad
    mov    edi, esi
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
    db     83h ;mask rol
sr_d:
    rol    edx, cl           ; rotate left for decryption
    mov    cl, 4
sr_l4:
    mov    [edi], dl
    ror    edx, 8
    scasd
    loop   sr_l4
    popad
    inc    ebx
    inc    esi
    loop   sr_l1
    popad
    ret

%define w0 eax
%define w1 ebx
%define w2 edx
%define w3 ebp

%define t eax
%define w ebp

gf_mul2:
    push   t
    mov    t, w
    and    t, 080808080h
    xor    w, t
    add    w, w
    shr    t, 7
    imul   t, t, 01bh
    xor    w, t
    pop    t
    ret
; ***********************************************
; void MixColumns (void *state, int enc)
; ***********************************************
MixColumns:
    pushad
    lea    edi, [ebp+(gf_mul2-ShiftRows)]
    test   ecx, ecx
    mov    cl, 4
mc_l1:
    pushfd
    lodsd                    ; w0 = state[i];
    jne    mc_l2
    mov    w3, w0
    ror    w3, 16
    xor    w3, w0
    call   edi               ; gf_mul2
    call   edi               ; gf_mul2
    xor    w0, w3
mc_l2:
    mov    w3, w0
    ror    w3, 8
    mov    w1, w3
    xor    w3, w0
    call   edi
    xor    w1, w3
    ror    w0, 16
    xor    w1, w0
    ror    w0, 8
    xor    w1, w0
    mov    [esi-4], w1
    popfd
    loop   mc_l1
    popad
    ret
    
ld_fn:
    pop    eax
    lea    ebp, [eax+(ShiftRows -AddRoundKey)]
    lea    edx, [ebp+(MixColumns-ShiftRows)]
    
    ; ************************************
    push   Nr
    pop    ebx
    jecxz  do_ark
    xor    ebx, ebx
do_ark:
    call   eax ; AddRoundKey
    push   eax
    jecxz  aes_edm
    xchg   edx, eax
    ;;jmp    aes_edm
    db     03dh ; mask call eax/call edx

aes_ed:
    call   eax ; MixColumns / AddRoundKey
    call   edx ; AddRoundKey / MixColumns

aes_edm:
    call   ebp ; ShiftRows + SubBytes
    dec    ebx
    jecxz  aes_edl
    inc    ebx
    inc    ebx
    cmp    ebx, Nr
aes_edl:
    jne    aes_ed
    pop    eax
    call   eax ; AddRoundKey
    popad
    ret
    
; *********************************************** 
; void aes_setkey (aes_ctx *ctx, void *key);
; *********************************************** 
_aes_setkeyx:
aes_setkeyx:
    pushad
    
    mov    edi, [esp+32+4]   ; ctx
    mov    esi, [esp+32+8]   ; key

    push   Nk
    pop    ecx
    rep    movsd

    push   1
    pop    edx                ; rcon = 1
sk_l1:
    mov    eax, [edi-4]       ; x=w[i-1];
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
    xor    eax, [edi-4*Nk]
    stosd
    inc    ecx
    cmp    ecx, Nb*Nr
    jne    sk_l1
    popad
    ret
; ***********************************************
; uint8_t SubByte (uint8_t x, int enc)
; *********************************************** 
SubByte:
    pushad
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
    mov    byte[esp+_eax], al
    popad
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
    push   ecx
    mov    cl, 4
sbw_l1:
    call   SubByte
    ror    eax, 8
    loop   sbw_l1
    pop    ecx
    ret
; ***********************************************    
; uint8_t gf_mulinv (uint8_t x);
; *********************************************** 
gf_mulinv:
    test   al, al
    xchg   edx, eax
    je     ret_y
    push   1
    pop    ebx          ; i=0, y=1
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
