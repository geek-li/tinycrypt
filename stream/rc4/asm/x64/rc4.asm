

; RC4 in x64 assembly
; Odzhan

ifdef __JWASM__
.x64
.model flat
endif

option casemap:none
option prologue:none
option epilogue:none

include rc4.inc

.code

; void rc4 (size_t data_len, void *data, RC4_CTX *ctx);
    public _rc4
    public rc4
_rc4:
rc4 proc
    push   rsi ; save rsi
    push   rdi ; save rdi
    push   rbx ; save rdi
    
    ; rsi=&ctx
    push   r8
    pop    rsi
    
    ; rdi=input
    push   rdx
    pop    rdi
    
    push   rsi               ; save pointer to ctx
    lodsd                    ; eax = x
    xchg   eax, ebx      
    lodsd                    ; ebx = y
    xchg   eax, ebx
    cdq
crypt_loop:
    inc    al                ; x++
    
    mov    dl, [rsi+rax]     ; dl = s[x]
    add    bl, dl            ; y += dl
    xchg   dl, [rsi+rbx]     ; swap s[y], s[x]
    mov    [rsi+rax], dl     ; s[x] = s[y]
    
    add    dl, [rsi+rbx]     ; dl = s[x] + s[y]
    mov    dl, [rsi+rdx]     ; dl = s[ dl ]
    xor    byte ptr[rdi], dl ; *p ^= (s[ s[x] + s[y] ])
    inc    rdi               ; p++    
    loop   crypt_loop
    pop    rdi               ; edi = &ctx
    stosd                    ; save x
    xchg   eax, ebx 
    stosd                    ; save y
    
    pop    rbx
    pop    rdi ; restore
    pop    rsi
    ret
rc4 endp

    end
