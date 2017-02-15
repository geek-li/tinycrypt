
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
; void b64encode (void *dst, void *src, size_t len);
; *************************************************************
    public _b64encode
    public b64encode
_b64encode:
b64encode proc
    push   rsi
    push   rdi
    
    push   rdx
    pop    rsi
    push   rcx
    pop    rdi
    push   r8
    pop    rdx
e64_outer:
    lodsd
    dec    esi
    inc    edx
    bswap  eax
    push   4
    pop    rcx
e64_inner:
    rol    eax, 6
    and    al, 3fh
    cmp    al, 3eh
    jb     e64_testchar
    ; '+' and '/' differ by only 1 bit
    shl    al, 2         
    sub    al, ((3eh shl 2) + 'A' - '+') and 0ffh
e64_testchar:
    sub    al, 4
    cmp    al, '0'
    ; l not b because '/' is still < 0 here
    jnl    e64_store     
    add    al, 'A' + 4
    cmp    al, 'Z'
    jbe    e64_store
    add    al, 'a' - 'Z' - 1
e64_store:
    stosb
    dec    edx
    loopne e64_inner
    jne    e64_outer
    mov    al, '='
    rep    stosb
    xchg   eax, ecx
    stosb
    pop    rdi
    pop    rsi
    ret
b64encode endp

    end
