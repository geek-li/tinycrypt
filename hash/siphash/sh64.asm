

  bits 64
  
; void siphash24(uint8_t const* key, void const* data, size_t size, uint8_t* out)

  global siphash24
  global _siphash24
  
%define v0 mm0
%define v1 mm1
%define v2 mm2
%define v3 mm3

%define k0 mm4
%define k1 mm5

%define x  mm4
%define t  mm5

; rotate mm0 left by bits in eax
; uses mm2, mm3 and mm4
rotl64:
    movq   mm2, mm0
    movd   mm3, eax    ; move count into mm2
    sub    eax, 64     ; calculate how much to rotate right
    neg    eax         ; 64  eax
    movd   mm4, eax
    psllq  mm0, mm3    ; shift left by n
    psrlq  mm2, mm4    ; shift right by 64n
    por    mm0, mm2    ; mm0 has the result
    ret
    
siphash24:
_siphash24:
    pushad
    lea    esi, [esp+32+4]
    lodsd
    xchg   edi, eax            ; edi=out
    lodsd
    push   eax                 ;
    lodsd
    xchg   eax, ecx            ; ecx=inlen
    lodsd                      ; eax=key
    pop    esi                 ; esi=in
    
    movq   k0, [eax]
    movq   k1, [eax+8]
    
    call   load_iv
    dq     0736F6D6570736575h;
    dq     0646F72616E646F6Dh;
    dq     06C7967656E657261h;
    dq     07465646279746573h;
load_iv
    pop    eax
    movq   v0, [eax]
    movq   v1, [eax+ 8]
    movq   v2, [eax+16]
    movq   v3, [eax+24]
    
    pxor   v0, k0
    pxor   v1, k1
    pxor   v2, k0
    pxor   v3, k1
    
sh_l0:
    pxor   x, x                ; x=0
    cmp    r, 8
    cmovb  r, ecx
    jecxz  sh_l2
sh_l1:
    lodsb                      ; al=*in++
    
    popad
    ret