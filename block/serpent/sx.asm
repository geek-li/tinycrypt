;
;  Copyright © 2015 Odzhan, Peter Ferrie.
;
;  All Rights Reserved.
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
; Serpent-256 block cipher in x86 assembly
;
; size: 530 bytes
;
; global calls use cdecl convention
;
; -----------------------------------------------
  bits 32

GOLDEN_RATIO    equ 0x9e3779b9

SERPENT_ROUNDS  equ 32
SERPENT_BLK_LEN equ 16
SERPENT_KEY256  equ 32

SERPENT_ENCRYPT equ 0
SERPENT_DECRYPT equ 1

%ifndef BIN
    global _serpent_setkeyx
    global _serpent_encryptx
%endif

struc pushad_t
  _edi dd ?
  _esi dd ?
  _ebp dd ?
  _esp dd ?
  _ebx dd ?
  _edx dd ?
  _ecx dd ?
  _eax dd ?
  .size:
endstruc
    
_serpent_encryptx:
serpent_encryptx:
    pushad
    lea    esi, [esp+32+ 4]
    lodsd
    xchg   edi, eax         ; out
    lodsd
    xchg   ecx, eax
    lodsd
    xchg   ecx, eax         ; enc
    xchg   esi, eax         ; key
    call   ld_fn
    ; ****************************
blkxor:
    pushad
    mov    cl, 4
    shl    edx, cl
blk_l:
    mov    eax, [esi+edx]
    xor    [edi], eax
    cmpsd
    loop   blk_l
    popad
    ret
    
%define x0 eax
%define x1 ebx
%define x2 edx
%define x3 ebp
%define x4 esi

; ecx=0 for encrypt
; ecx=1 for decrypt
;
; edi = input
_serpent_ltx:
serpent_lt:
    pushad
    mov    esi, edi
    
    lodsd
    xchg   x0, x3
    lodsd
    xchg   x0, x1
    lodsd
    xchg   x0, x2
    lodsd
    xchg   x0, x3
    
    jecxz  lt_enc   ; if ecx=0, encryption
    
    rol    x2, 10
    ror    x0, 5
    mov    x4, x1
    shl    x4, 7
    xor    x2, x4
    xor    x2, x3
    xor    x0, x1
    xor    x0, x3
    ror    x3, 7
    ror    x1, 1
    mov    x4, x0
    shl    x4, 3
    xor    x3, x4
    xor    x3, x2
    xor    x1, x0
    xor    x1, x2
    ror    x2, 3
    ror    x0, 13
    jmp    lt_end
lt_enc:
    rol    x0, 13
    rol    x2, 3
    xor    x1, x0
    xor    x1, x2
    xor    x3, x2
    mov    x4, x0
    shl    x4, 3
    xor    x3, x4
    rol    x1, 1
    rol    x3, 7
    xor    x0, x1
    xor    x0, x3
    mov    x4, x1
    shl    x4, 7
    xor    x2, x3
    xor    x2, x4
    rol    x0, 5
    ror    x2, 10
lt_end:
    stosd
    xchg   x0, x1
    stosd
    xchg   x0, x2
    stosd
    xchg   x0, x3
    stosd
    popad
    ret
 
; CF=0 : encryption
; CF=1 : decryption
;
; edi = blk
; edx = i
_sbox128x:
sbox128:
    pushad
    call   init_sbox    
; sbox
  db 083h, 01Fh, 06Ah, 0B5h, 0DEh, 024h, 007h, 0C9h
  db 0CFh, 072h, 009h, 0A5h, 0B1h, 08Eh, 0D6h, 043h
  db 068h, 097h, 0C3h, 0FAh, 01Dh, 04Eh, 0B0h, 025h
  db 0F0h, 08Bh, 09Ch, 036h, 01Dh, 042h, 07Ah, 0E5h
  db 0F1h, 038h, 00Ch, 06Bh, 052h, 0A4h, 0E9h, 0D7h
  db 05Fh, 0B2h, 0A4h, 0C9h, 030h, 08Eh, 06Dh, 017h
  db 027h, 05Ch, 048h, 0B6h, 09Eh, 0F1h, 03Dh, 00Ah
  db 0D1h, 00Fh, 08Eh, 0B2h, 047h, 0ACh, 039h, 065h
; sbox_inv
  db 03Dh, 00Bh, 06Ah, 0C5h, 0E1h, 074h, 09Fh, 028h
  db 085h, 0E2h, 06Fh, 03Ch, 04Bh, 097h, 0D1h, 00Ah
  db 09Ch, 04Fh, 0EBh, 021h, 030h, 0D6h, 085h, 07Ah
  db 090h, 07Ah, 0EBh, 0D6h, 053h, 02Ch, 084h, 01Fh
  db 005h, 038h, 09Ah, 0E7h, 0C2h, 06Bh, 0F4h, 01Dh
  db 0F8h, 092h, 014h, 0EDh, 06Bh, 035h, 0C7h, 00Ah
  db 0AFh, 0D1h, 035h, 006h, 094h, 07Eh, 0C2h, 0B8h
  db 003h, 0D6h, 0E9h, 08Fh, 0C5h, 07Bh, 01Ah, 024h
  
init_sbox:
    pop    esi               ; esi=sbox
    jc     sb_and
    add    esi, 64           ; eax=sbox_inv
sb_and:
    and    edx, 7            ; %= 8
    lea    esi, [esi+8*edx]  ; esi = &sbox[i*8]
    mov    edx, edi
    call   ld_perm
    
; void permute (out, in);
; edi = out
; esi = in
; CF = type
_permutex:
permute:
    pushad
    xchg   eax, ecx    ; ecx should be zero
    push   16
    pop    ecx
    pushad
    rep    stosb
    popad
    cdq            ; m=0
    jnc    do_fp
    ; initial permutation
ip_m_l:
    mov    eax, edx
    and    eax, 3
    shr    dword[esi+4*eax], 1
    rcr    byte[edi], 1
    inc    edx
    test   dl, 7
    jne    ip_m_l
    inc    edi
    loop   ip_m_l
xit_perm:
    popad
    ret
    ; final permutation
do_fp:
    mov    cl, 4    ; n=4
fp_m_l:
    mov    eax, edx
    and    eax, 3
    shr    dword[esi], 1
    rcr    dword[edi+4*eax], 1
    inc    edx
    test   dl, 31
    jne    fp_m_l
    lodsd
    loop   fp_m_l
    popad
    ret
    
ld_perm:
    pop    ebp
    
    pushad          ; create 2 16-byte blocks
    mov    ebx, esp          ; ebx=sb
    mov    edi, esp          ; edi=sb
    mov    cl, 8             ; SERPENT_BLK_LEN/2
sb_l1:
    lodsb                    ; t = sbp[i/2];
    aam    16
    stosw
    loop   sb_l1
    
    ; permute (&tmp_blk, blk, SERPENT_IP);
    mov    esi, edx
    stc
    call   ebp ;permute
    mov    esi, edi
    mov    cl, SERPENT_BLK_LEN
    push   esi
sb_l2:
    lodsb
    aam    16
    xchg   ah, al
    xlatb
    xchg   ah, al
    xlatb
    aad    16 
    stosb
    loop   sb_l2
    
    pop    esi
    mov    edi, edx
    ; permute (blk, &tmp_blk, SERPENT_FP);
    call   ebp ;permute 
    popad
    popad
    ret
    
ld_fn:
    ; *********************************
    pop    ebp
    lea    ebx, [ebp+(serpent_lt-blkxor)]
    lea    eax, [ebx+(sbox128-serpent_lt)]
    xor    edx, edx
    jecxz  se_e
    mov    dl, SERPENT_ROUNDS
    ; blkxor (out, key, i);
    call   ebp ; blkxor
    jmp    sd_e
sd_l:
    ; serpent_lt (out, SERPENT_DECRYPT);
    call   ebx ; serpent_lt
sd_e:
    dec    edx               ; --i
    ; sbox128 (out, i, SERPENT_DECRYPT);
    clc
    call   eax ; sbox128
    ; blkxor (out, key, i);
    call   ebp ; blkxor
    test   edx, edx
    jnz    sd_l
    popad
    ret
se_l:
    ; serpent_lt (out, SERPENT_ENCRYPT);
    call   ebx ; serpent_lt
se_e:
    ; blkxor (out, key, i);
    call   ebp ; blkxor
    ; sbox128 (out, i, SERPENT_ENCRYPT);
    stc
    call   eax ; sbox128
    inc    edx
    cmp    edx, SERPENT_ROUNDS
    jne    se_l
    call   ebp ; blkxor
    popad
    ret
    ; *********************************
    
_serpent_setkeyx:  
serpent_setkey:  
    pushad
    
    ; copy key into local memory
    mov    ecx, esp
    pushad                  ; allocate 32-bytes
    sub    ecx, esp         ; ecx = 32
    mov    edi, esp
    mov    esi, [edi+64+8]  ; esi = input
    rep    movsb
    
    mov    esi, esp          ; esi = local key bytes
    mov    edi, [edi+32+4]   ; edi = key ctx
    ; ecx will be i which is now 0
skey_l1:
    xor    edx, edx           ; j=0
skey_l2:
    ; serpent_gen_w (s_ws.v32, j+4*i);
    mov    eax, [esi]
    xor    eax, [esi+3*4]
    xor    eax, [esi+5*4]
    xor    eax, [esi+7*4]
    xor    eax, GOLDEN_RATIO
    lea    ebx, [edx+4*ecx]
    xor    eax, ebx
    rol    eax, 11
    mov    [edi+4*edx], eax
    
    ; rotate workspace left 32 bits
    pushad
    push   esi
    cmpsd
    pop    edi
    mov    cl, 7
    rep    movsd
    stosd
    popad
    
    ; j++
    inc    edx
    cmp    edx, 4
    jne    skey_l2
    
    ; apply sbox
    dec    edx    
    sub    edx, ecx          ; 3 - i
    stc                      ; CF=1 for encrypt
    call   sbox128
    
    ; advance key
    add    edi, 16
    
    ; i++
    inc    ecx
    cmp    ecx, 32
    jbe    skey_l1
    popad                    ; release 32-bytes of memory
    popad                    ; restore registers
    ret

