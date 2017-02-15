

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

; void rc4_set_key (size_t key_len, void *key, RC4_CTX *ctx);
    public _rc4_setkey
    public rc4_setkey
_rc4_setkey:
rc4_setkey proc
    push   rsi
    push   rdi
    push   rbp
    
    ; rdi = ctx
    push   r8
    pop    rdi
    ; rbp = key_len
    push   rcx
    pop    rbp
    ; rsi = key
    push   rdx
    pop    rsi
    xor    eax, eax               ; i=0
    cdq                           ; j=0
    stosd                         ; x=0
    stosd                         ; y=0
init_sbox:
    mov    byte ptr[rdi+rax], al  ; s[i] = i
    inc    al                     ; i++
    jnz    init_sbox
init_key:
    cmp    ecx, ebp               ; if (key_idx == key_len)
    jne    skip_reset
    xor    ecx, ecx               ; key_idx = 0
skip_reset:
    mov    bl, [rdi+rax]          ; s[i]
    add    dl, bl                 ; j += s[i]
    add    dl, [rsi+rcx]          ; j += key[key_idx]
    xchg   bl, [rdi+rdx]          ; s[j] = s[i]
    mov    [rdi+rax], bl          ; s[i] = s[j]
    inc    ecx                    ; key_idx++
    inc    al                     ; i++
    jnz    init_key               ;
    
    pop    rbp
    pop    rdi
    pop    rsi
    ret
rc4_setkey endp

    end
