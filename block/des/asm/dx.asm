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
; DES block cipher in x86 assembly
;
; http://csrc.nist.gov/publications/fips/fips46-3/fips46-3.pdf
;
; size: 1,038 bytes
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
  
  %ifndef BIN
    global _des_str2keyx
    global _des_setkeyx
    global _des_encx
  %endif
  
; esi = permutation table
; ebx = input
; edi = output
_permutex:
permutex:
    pushad
    
    xor    eax, eax
    push   edi
    stosd
    stosd
    pop    edi
    
    ; ob=*p++;
    lodsb
    xchg   ecx, eax
p_l1:
    push   ecx
    mov    cl, 8
p_l2:
    push   ecx
    ; x = *p++;
    lodsb
    ; replace (0x80 >> (x % 8)))
    ; with 1 << (7 - (x % 8))
    mov    cl, 7
    sub    cl, al
    and    cl, 7
    ; in[x/8];
    shr    al, 3
    xlatb
    ; if (in[x/8] & (0x80 >> (x % 8)))
    bt     eax, ecx
    ; t <<= 1;
    adc    dl, dl
    pop    ecx
    loop   p_l2
    xchg   eax, edx
    ; out->v8[i]=t;
    stosb
    pop    ecx
    loop   p_l1
    popad
    ret
  
; eax = input
; esi = key
; return result in eax
_des_fx:
des_fx:
    pushfd
    pushad
    cld

    ; put x in ebx
    xchg   ebx, eax
    ; put key in eax
    xchg   eax, esi
    
    ; allocate 32 bytes
    pushad

    mov    ebp, permutex
    
    ; permute (e_permtab, x, &t0);
    mov    edi, esp
    mov    esi, e_permtab
    call   ebp
    
    ; put key in esi
    xchg   eax, esi
    lodsd
    xor    [edi], eax
    lodsd
    xor    [edi+4], eax

    ; permute (splitin6bitword_permtab, &t0, &t1);
    mov    esi, splitin6bitword_permtab
    mov    ebx, esp
    scasd
    scasd
    call   ebp
    
    mov    esi, edi
    mov    ebx, sbox
    push   8
    pop    ecx
df_l2:
    lodsb
    shr    al, 1
    pushfd
    xlatb
    aam    16
    popfd
    jb     df_l3
    mov    al, ah
df_l3:
    shl    edx, 4
    or     dl, al
    add    ebx, 32
    loop   df_l2
    
    bswap  edx
    
    ; permute (p_permtab, &t, &t0);
    mov    ebx, edi
    mov    esi, p_permtab
    mov    edi, esp
    mov    [ebx], edx
    call   ebp
    mov    eax, [edi]
    mov    [esp+_eax+32], eax
    popad
    popad
    popfd
    ret
  
_des_setkeyx:
des_setkey:
    pushad
    mov    ebx, [esp+32+8]  ; input
    
    ; alloc space for k1, k2
    pushad
    
    mov    edx, permutex
    
    ; permute (pc1_permtab, input, &k1);
    mov    esi, pc1_permtab
    mov    edi, esp
    call   edx               ; permutex
    xor    ecx, ecx
    mov    edi, [esp+32+32+4]  ; ctx
sk_l1:
    ; permute (shiftkey_permtab, &k1, &k2);
    mov    ebx, esp          ; ebx=k1
    push   edi
    lea    edi, [ebx+8]      ; edi=k2
    add    esi, (shiftkey_permtab - pc1_permtab)
    ;mov    esi, shiftkey_permtab
    call   edx               ; permutex
    push   1
    pop    eax
    shl    eax, cl
    test   eax, 07EFCh
    xchg   ebx, edi          ; ebx=k2, edi=k1
    jz     sk_l2
    ; permute (shiftkey_permtab, &k2, &k1);
    call   edx               ; permutex
    mov    ebx, edi          ; ebx=k1
sk_l2:
    ; permute (pc2_permtab, k, &ctx->keys[rnd]);
    ;mov    esi, pc2_permtab
    add    esi, (pc2_permtab - shiftkey_permtab)
    pop    edi
    call   edx               ; permutex
    ; memcpy (k1.v8, k->v8, DES_BLK_LEN);
    fild   qword [ebx]
    fistp  qword [esp]
    scasd
    scasd
    sub    esi, (shiftkey_permtab - pc1_permtab) + \
                (pc2_permtab - shiftkey_permtab)
    ; rnd++
    inc    ecx
    cmp    ecx, 16
    jnz    sk_l1
    ; free stack
    popad
    ; restore registers
    popad
    ret
  
%define L ebp
%define R edx
  
_des_encx:
des_encx:
    pushad
    mov    ebp, esp
    
    mov    ebx, [ebp+32+ 8] ; in
    mov    ecx, [ebp+32+16] ; enc
    
    ; permute (ip_permtab, in, &t0);
    push   ecx
    push   ecx
    mov    edi, esp
    mov    esi, ip_permtab
    call   permutex
    
    mov    esi, [ebp+32+ 4] ; esi=ctx
    mov    ebx, esp
    
    mov    L, [edi]
    mov    R, [edi+4]
    
    jecxz  de_l1
    ; if decrypt, advance key and set direction
    add    esi, 15*8
    std
de_l1:
    mov    cl, 16

de_l2:
    ; L ^= des_f (&R, key);
    push   R
    mov    eax, esp
    call   des_fx
    
    xor    L, eax
    pop    eax
    ; swap
    xchg   L, R
    ; key += ofs;
    lodsd
    lodsd
    loop   de_l2
    cld
    
    ; permute (inv_ip_permtab, &t0, out);
    mov    dword[ebx], R
    mov    dword[ebx+4], L
    mov    edi, [esp+32+12+8] ; edi=out
    mov    esi, inv_ip_permtab
    call   permutex
    pop    eax
    pop    eax
    popad
    ret
    
_des_str2keyx:
des_str2keyx:
    pushad
    mov    esi, [esp+32+4] ; str
    mov    edi, [esp+32+8] ; key
    push   2
    pop    ecx
s2k_l1:
    push   ecx
    lodsd
    dec    esi
    bswap  eax
    dec    ecx
    mov    cl, 4    
    jnz    s2k_l3
    rol    eax, cl
s2k_l3:
    shld   edx, eax, 7
    add    edx, edx
    shl    eax, 7
    loop   s2k_l3
    
    xchg   eax, edx
    bswap  eax
    stosd
    pop    ecx
    loop   s2k_l1
    popad
    ret
sbox:
  ; S-box 1
  db 0E4h, 0D1h, 02Fh, 0B8h, 03Ah, 06Ch, 059h, 007h
  db 00Fh, 074h, 0E2h, 0D1h, 0A6h, 0CBh, 095h, 038h
  db 041h, 0E8h, 0D6h, 02Bh, 0FCh, 097h, 03Ah, 050h
  db 0FCh, 082h, 049h, 017h, 05Bh, 03Eh, 0A0h, 06Dh
  ; S-box 2
  db 0F1h, 08Eh, 06Bh, 034h, 097h, 02Dh, 0C0h, 05Ah
  db 03Dh, 047h, 0F2h, 08Eh, 0C0h, 01Ah, 069h, 0B5h
  db 00Eh, 07Bh, 0A4h, 0D1h, 058h, 0C6h, 093h, 02Fh
  db 0D8h, 0A1h, 03Fh, 042h, 0B6h, 07Ch, 005h, 0E9h
  ; S-box 3
  db 0A0h, 09Eh, 063h, 0F5h, 01Dh, 0C7h, 0B4h, 028h
  db 0D7h, 009h, 034h, 06Ah, 028h, 05Eh, 0CBh, 0F1h
  db 0D6h, 049h, 08Fh, 030h, 0B1h, 02Ch, 05Ah, 0E7h
  db 01Ah, 0D0h, 069h, 087h, 04Fh, 0E3h, 0B5h, 02Ch
  ; S-box 4 
  db 07Dh, 0E3h, 006h, 09Ah, 012h, 085h, 0BCh, 04Fh
  db 0D8h, 0B5h, 06Fh, 003h, 047h, 02Ch, 01Ah, 0E9h
  db 0A6h, 090h, 0CBh, 07Dh, 0F1h, 03Eh, 052h, 084h
  db 03Fh, 006h, 0A1h, 0D8h, 094h, 05Bh, 0C7h, 02Eh
  ; S-box 5
  db 02Ch, 041h, 07Ah, 0B6h, 085h, 03Fh, 0D0h, 0E9h
  db 0EBh, 02Ch, 047h, 0D1h, 050h, 0FAh, 039h, 086h
  db 042h, 01Bh, 0ADh, 078h, 0F9h, 0C5h, 063h, 00Eh
  db 0B8h, 0C7h, 01Eh, 02Dh, 06Fh, 009h, 0A4h, 053h
  ; S-box 6
  db 0C1h, 0AFh, 092h, 068h, 00Dh, 034h, 0E7h, 05Bh
  db 0AFh, 042h, 07Ch, 095h, 061h, 0DEh, 00Bh, 038h
  db 09Eh, 0F5h, 028h, 0C3h, 070h, 04Ah, 01Dh, 0B6h
  db 043h, 02Ch, 095h, 0FAh, 0BEh, 017h, 060h, 08Dh
  ; S-box 7
  db 04Bh, 02Eh, 0F0h, 08Dh, 03Ch, 097h, 05Ah, 061h
  db 0D0h, 0B7h, 049h, 01Ah, 0E3h, 05Ch, 02Fh, 086h
  db 014h, 0BDh, 0C3h, 07Eh, 0AFh, 068h, 005h, 092h
  db 06Bh, 0D8h, 014h, 0A7h, 095h, 00Fh, 0E2h, 03Ch
  ; S-box 8
  db 0D2h, 084h, 06Fh, 0B1h, 0A9h, 03Eh, 050h, 0C7h
  db 01Fh, 0D8h, 0A3h, 074h, 0C5h, 06Bh, 00Eh, 092h
  db 07Bh, 041h, 09Ch, 0E2h, 006h, 0ADh, 0F3h, 058h
  db 021h, 0E7h, 04Ah, 08Dh, 0FCh, 090h, 035h, 06Bh

e_permtab:
  db  006h,
  db  01fh, 000h, 001h, 002h, 003h, 004h, 003h, 004h
  db  005h, 006h, 007h, 008h, 007h, 008h, 009h, 00ah
  db  00bh, 00ch, 00bh, 00ch, 00dh, 00eh, 00fh, 010h
  db  00fh, 010h, 011h, 012h, 013h, 014h, 013h, 014h
  db  015h, 016h, 017h, 018h, 017h, 018h, 019h, 01ah
  db  01bh, 01ch, 01bh, 01ch, 01dh, 01eh, 01fh, 000h
splitin6bitword_permtab:
  db  008h,
  db  03fh, 03fh, 000h, 005h, 001h, 002h, 003h, 004h
  db  03fh, 03fh, 006h, 00bh, 007h, 008h, 009h, 00ah
  db  03fh, 03fh, 00ch, 011h, 00dh, 00eh, 00fh, 010h
  db  03fh, 03fh, 012h, 017h, 013h, 014h, 015h, 016h
  db  03fh, 03fh, 018h, 01dh, 019h, 01ah, 01bh, 01ch
  db  03fh, 03fh, 01eh, 023h, 01fh, 020h, 021h, 022h
  db  03fh, 03fh, 024h, 029h, 025h, 026h, 027h, 028h
  db  03fh, 03fh, 02ah, 02fh, 02bh, 02ch, 02dh, 02eh
p_permtab:
  db  004h,
  db  00fh, 006h, 013h, 014h, 01ch, 00bh, 01bh, 010h
  db  000h, 00eh, 016h, 019h, 004h, 011h, 01eh, 009h
  db  001h, 007h, 017h, 00dh, 01fh, 01ah, 002h, 008h
  db  012h, 00ch, 01dh, 005h, 015h, 00ah, 003h, 018h
ip_permtab:
  db  008h,
  db  039h, 031h, 029h, 021h, 019h, 011h, 009h, 001h
  db  03bh, 033h, 02bh, 023h, 01bh, 013h, 00bh, 003h
  db  03dh, 035h, 02dh, 025h, 01dh, 015h, 00dh, 005h
  db  03fh, 037h, 02fh, 027h, 01fh, 017h, 00fh, 007h
  db  038h, 030h, 028h, 020h, 018h, 010h, 008h, 000h
  db  03ah, 032h, 02ah, 022h, 01ah, 012h, 00ah, 002h
  db  03ch, 034h, 02ch, 024h, 01ch, 014h, 00ch, 004h
  db  03eh, 036h, 02eh, 026h, 01eh, 016h, 00eh, 006h
inv_ip_permtab:
  db  008h,
  db  027h, 007h, 02fh, 00fh, 037h, 017h, 03fh, 01fh
  db  026h, 006h, 02eh, 00eh, 036h, 016h, 03eh, 01eh
  db  025h, 005h, 02dh, 00dh, 035h, 015h, 03dh, 01dh
  db  024h, 004h, 02ch, 00ch, 034h, 014h, 03ch, 01ch
  db  023h, 003h, 02bh, 00bh, 033h, 013h, 03bh, 01bh
  db  022h, 002h, 02ah, 00ah, 032h, 012h, 03ah, 01ah
  db  021h, 001h, 029h, 009h, 031h, 011h, 039h, 019h
  db  020h, 000h, 028h, 008h, 030h, 010h, 038h, 018h
pc1_permtab:
  db  007h,
  db  038h, 030h, 028h, 020h, 018h, 010h, 008h, 000h
  db  039h, 031h, 029h, 021h, 019h, 011h, 009h, 001h
  db  03ah, 032h, 02ah, 022h, 01ah, 012h, 00ah, 002h
  db  03bh, 033h, 02bh, 023h, 03eh, 036h, 02eh, 026h
  db  01eh, 016h, 00eh, 006h, 03dh, 035h, 02dh, 025h
  db  01dh, 015h, 00dh, 005h, 03ch, 034h, 02ch, 024h
  db  01ch, 014h, 00ch, 004h, 01bh, 013h, 00bh, 003h
shiftkey_permtab:
  db  007h,
  db  001h, 002h, 003h, 004h, 005h, 006h, 007h, 008h
  db  009h, 00ah, 00bh, 00ch, 00dh, 00eh, 00fh, 010h
  db  011h, 012h, 013h, 014h, 015h, 016h, 017h, 018h
  db  019h, 01ah, 01bh, 000h, 01dh, 01eh, 01fh, 020h
  db  021h, 022h, 023h, 024h, 025h, 026h, 027h, 028h
  db  029h, 02ah, 02bh, 02ch, 02dh, 02eh, 02fh, 030h
  db  031h, 032h, 033h, 034h, 035h, 036h, 037h, 01ch
pc2_permtab:
  db  006h,
  db  00dh, 010h, 00ah, 017h, 000h, 004h, 002h, 01bh
  db  00eh, 005h, 014h, 009h, 016h, 012h, 00bh, 003h
  db  019h, 007h, 00fh, 006h, 01ah, 013h, 00ch, 001h
  db  028h, 033h, 01eh, 024h, 02eh, 036h, 01dh, 027h
  db  032h, 02ch, 020h, 02fh, 02bh, 030h, 026h, 037h
  db  021h, 034h, 02dh, 029h, 031h, 023h, 01ch, 01fh