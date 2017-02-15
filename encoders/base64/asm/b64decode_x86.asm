
;
; Base64 encoder and decoder in x86 assembly
; Code here originally by Peter Ferrie
; I changed it a little, specifically adding d64_load code
; but 99% is work of Peter
;
; Odzhan

.586
.model flat, C

option prologue:none
option epilogue:none

.code

; *************************************************************
; void b64decode (void *dst, void *src, size_t len);
; *************************************************************
    public _b64decode
    public b64decode
_b64decode:
b64decode proc ; dst:ptr byte, src:ptr byte
    pushad
    mov    edi, [esp+32+4]
    mov    esi, [esp+32+8]
d64_outer:
    push   4
    pop    ecx
    xor    eax, eax
    cdq
d64_load:
    shl    eax, 8
    lodsb
    cmp    byte ptr[esi], dl
    loopne d64_load
    neg    ecx
    lea    ecx, [ecx+4]
    bswap  eax
    push   eax
d64_inner:
    rol    eax, 8
    cmp    al, '0'
    jnb    d64_testupr
    shr    al, 2 ;because '+' and '/' differ by only 1 bit
    xor    al, '0' ;concatenate numbers and '+' and '/'
d64_testupr:
    cmp    al, 'A'
    jnb    d64_testlwr
    add    al, ('z' + 1) - '0' ;concatenate lowercase and numbers
d64_testlwr:
    cmp    al, 'a'
    jb     d64_store
    sub    al, 'a' - ('Z' + 1) ;concatenate uppercase and lowercase
d64_store:
    sub    al, 'A'
    shrd   edx, eax, 6
    loop   d64_inner
    pop    eax
    bswap  edx
    xchg   edx, eax
    mov    cl, 3
d64_storei:
    stosb
    shr    eax, 8
    shr    edx, 8
    cmp    dh, '='
    loopne d64_storei
    je     d64decode_end
    cmp    byte ptr [esi], '+'
    jnb    d64_outer
d64decode_end:
    xor    al, al
    stosb
    popad
    ret
b64decode endp

    end