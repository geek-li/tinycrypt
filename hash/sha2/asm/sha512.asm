

; SHA-512 in x86/SSE assembly
; Odzhan

.686
.xmm
.model flat, C

include sha2.inc

option casemap:none
option prologue:none
option epilogue:none

; n is wrapped in case of expression e.g 34-12
pror macro v:req, n:req
  movq   mm0, v
  psrlq  v, (n)
  psllq  mm0, 64-(n)
  por    v, mm0
endm


_a textequ <qword ptr[ebx+0*8]>
_b textequ <qword ptr[ebx+1*8]>
_c textequ <qword ptr[ebx+2*8]>
_d textequ <qword ptr[ebx+3*8]>
_e textequ <qword ptr[ebx+4*8]>
_f textequ <qword ptr[ebx+5*8]>
_g textequ <qword ptr[ebx+6*8]>
_h textequ <qword ptr[ebx+7*8]>

s0 equ <mm1>
s1 equ <mm2>
t1 equ <mm3>
t2 equ <mm4>
t3 equ <mm5>

i  equ <ecx>

%ifndef BIN
  global _SHA512_Transformx
%endif

_SHA512_Transformx:
    pushad
    
    mov    esi, [esp+32+4] ; ctx
    mov    edx, esi
    ; allocate work space, 80 words + 8 for state
    sub    esp, 128*8
    mov    ebx, esp
    
    ; load esi:state into edi:local buffer
    xor    i, i
load_state:
    movq   mm0, [esi+8*i]
    movq   [ebx+8*i], mm0
    inc    i
    cmp    i, 8
    jnz    load_state
    
    ; load data in big endian format
    lea    edi, [ebx+64]
    lea    esi, [esi+64]
    xor    i, i
load_data:
    mov    eax, [esi+8*i]
    mov    ebp, [esi+8*i+4]
    bswap  eax
    bswap  ebp
    mov    [edi+8*i], ebp
    mov    [edi+8*i+4], eax
    inc    i
    cmp    i, 16
    jnz    load_data

    ; expand data
expand:
    ; s0 := (w[i-15] rightrotate 1) xor (w[i-15] rightrotate 8) xor (w[i-15] rightshift 7)
    movq   s0, qword ptr[8*i+edi-15*8]
    movq   t1, s0
    movq   t2, s0
    pror   s0, 1
    pror   t1, 8
    psrlq  t2, 7
    pxor   s0, t1
    pxor   s0, t2
    ; s1 := (w[i-2] rightrotate 19) xor (w[i-2] rightrotate 61) xor (w[i-2] rightshift 6)
    movq   s1, qword ptr[8*i+edi-2*8]
    movq   t1, s1
    movq   t2, s1
    pror   s1, 19
    pror   t1, 61
    psrlq  t2, 6
    pxor   s1, t1
    pxor   s1, t2
    ; w[i] := w[i-16] + s0 + w[i-7] + s1
    paddq  s0, qword ptr[8*i+edi-16*8]
    paddq  s1, qword ptr[8*i+edi-7*8]
    paddq  s0, s1
    movq   qword ptr[edi+8*i], s0
    inc    i
    cmp    i, 80
    jnz    expand

    ; update context
    xor    i, i
    call   ld_k
    ; k values
    dq 0428a2f98d728ae22h, 07137449123ef65cdh, 0b5c0fbcfec4d3b2fh, 0e9b5dba58189dbbch 
    dq 03956c25bf348b538h, 059f111f1b605d019h, 0923f82a4af194f9bh, 0ab1c5ed5da6d8118h 
    dq 0d807aa98a3030242h, 012835b0145706fbeh, 0243185be4ee4b28ch, 0550c7dc3d5ffb4e2h 
    dq 072be5d74f27b896fh, 080deb1fe3b1696b1h, 09bdc06a725c71235h, 0c19bf174cf692694h 
    dq 0e49b69c19ef14ad2h, 0efbe4786384f25e3h, 00fc19dc68b8cd5b5h, 0240ca1cc77ac9c65h 
    dq 02de92c6f592b0275h, 04a7484aa6ea6e483h, 05cb0a9dcbd41fbd4h, 076f988da831153b5h 
    dq 0983e5152ee66dfabh, 0a831c66d2db43210h, 0b00327c898fb213fh, 0bf597fc7beef0ee4h 
    dq 0c6e00bf33da88fc2h, 0d5a79147930aa725h, 006ca6351e003826fh, 0142929670a0e6e70h 
    dq 027b70a8546d22ffch, 02e1b21385c26c926h, 04d2c6dfc5ac42aedh, 053380d139d95b3dfh 
    dq 0650a73548baf63deh, 0766a0abb3c77b2a8h, 081c2c92e47edaee6h, 092722c851482353bh 
    dq 0a2bfe8a14cf10364h, 0a81a664bbc423001h, 0c24b8b70d0f89791h, 0c76c51a30654be30h 
    dq 0d192e819d6ef5218h, 0d69906245565a910h, 0f40e35855771202ah, 0106aa07032bbd1b8h 
    dq 019a4c116b8d2d0c8h, 01e376c085141ab53h, 02748774cdf8eeb99h, 034b0bcb5e19b48a8h 
    dq 0391c0cb3c5c95a63h, 04ed8aa4ae3418acbh, 05b9cca4f7763e373h, 0682e6ff3d6b2b8a3h 
    dq 0748f82ee5defb2fch, 078a5636f43172f60h, 084c87814a1f0ab72h, 08cc702081a6439ech 
    dq 090befffa23631e28h, 0a4506cebde82bde9h, 0bef9a3f7b2c67915h, 0c67178f2e372532bh 
    dq 0ca273eceea26619ch, 0d186b8c721c0c207h, 0eada7dd6cde0eb1eh, 0f57d4f7fee6ed178h 
    dq 006f067aa72176fbah, 00a637dc5a2c898a6h, 0113f9804bef90daeh, 01b710b35131c471bh 
    dq 028db77f523047d84h, 032caab7b40c72493h, 03c9ebe0a15c9bebch, 0431d67c49c100d4ch 
    dq 04cc5d4becb3e42b6h, 0597f299cfc657e2ah, 05fcb6fab3ad6faech, 06c44198c4a475817h 
ld_k:
    pop    esi
update_loop:
    ; S1 := EP1(e)
    ; S1 := (e rightrotate 14) xor (e rightrotate 18) xor (e rightrotate 41)
    movq   s1, _e
    pror   s1, 41-18
    pxor   s1, _e
    pror   s1, 18-14
    pxor   s1, _e
    pror   s1, 14
    ; CH(e, f, g)
    ; ch := (e and f) xor ((not e) and g)
    movq   t1, _f
    pxor   t1, _g
    pand   t1, _e
    pxor   t1, _g
    ; temp1 := h + S1 + ch + k[i] + w[i]
    paddq  t1, s1
    paddq  t1, _h
    paddq  t1, qword ptr [esi+8*i] ; _k[i]
    paddq  t1, qword ptr [edi+8*i] ; w[i]
    ; S0 := EP0(a)
    ; S0 := (a rightrotate 28) xor (a rightrotate 34) xor (a rightrotate 39)
    movq   s0, _a
    pror   s0, 39-34
    pxor   s0, _a
    pror   s0, 34-28
    pxor   s0, _a
    pror   s0, 28
    ; MAJ(a, b, c)
    ; maj := (a and b) xor (a and c) xor (b and c)
    movq   t2, _a
    movq   t3, _a
    por    t2, _b
    pand   t2, _c
    pand   t3, _b
    por    t2, t3
    ; temp2 := S0 + maj
    paddq  t2, s0
    ; d = d + temp1
    movq   t3, t1
    paddq  t3, _d
    movq   _d, t3
    ; h = temp1 + temp2
    paddq  t1, t2
    movq   _h, t1
    ; shift state
    ; a becomes b, b becomes c, c becomes d and so on
    movq   mm0, _a
    movq   mm1, _b
    movq   mm2, _c
    movq   mm3, _d
    movq   mm4, _e
    movq   mm5, _f
    movq   mm6, _g
    movq   mm7, _h

    movq   _a, mm7
    movq   _b, mm0
    movq   _c, mm1
    movq   _d, mm2
    movq   _e, mm3
    movq   _f, mm4
    movq   _g, mm5
    movq   _h, mm6
    
    ; i++
    inc    i
    cmp    i, 80
    jne    update_loop
    
    ; Produce the final hash value (big-endian):
    ; digest := hash := h0 append h1 append h2 append h3 append 
    ; h4 append h5 append h6 append h7
    xor    i, i
save_state:    
    movq   mm1, [ebx+8*i]
    paddq  mm1, [edx+8*i]
    movq   [edx+8*i], mm1
    inc    i
    cmp    i, 8
    jnz    save_state
    
    add    esp, 128*8
    ; restore registers
    popad
    ret
