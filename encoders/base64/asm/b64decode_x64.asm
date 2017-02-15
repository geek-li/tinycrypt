
;
; Base64 encoder and decoder in x86 assembly
; Code here originally by Peter Ferrie
; I changed it a little, specifically adding d64_load code
; but 99% is work of Peter
;
; Odzhan

ifdef __JWASM__
.x64
.model flat
endif

option casemap:none
option prologue:none
option epilogue:none

.code

; *************************************************************
; void b64decode (void *dst, void *src);
; *************************************************************
    public _b64decode
    public b64decode
_b64decode:
b64decode proc
    push   rsi
    push   rdi
    
    push   rcx
    pop    rdi
    push   rdx
    pop    rsi
d64_outer:
    push   4
    pop    rcx
    xor    eax, eax
    cdq
d64_load:
    shl    eax, 8
    lodsb
    cmp    byte ptr[rsi], dl
    loopne d64_load
    neg    ecx
    add    ecx, 4
    bswap  eax
    push   rax
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
    pop    rax
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
    cmp    byte ptr [rsi], '+'
    jnb    d64_outer
d64decode_end:
    xor    al, al
    stosb
    pop    rdi
    pop    rsi
    ret
b64decode endp

    end
