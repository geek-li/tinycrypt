
; *********************************************************
; Spritz - a spongy RC4-like stream cipher and hash function
; Designed by Ron Rivest and Jacob Schuldt
;
; Odzhan
; *********************************************************
;
; 369 bytes

.686
.model flat

option epilogue:none
option prologue:none

N equ 256

STATE struct
    a byte ?
    i byte ?
    j byte ?
    k byte ?
    z byte ?
    w byte ?
    s byte N dup (?)
STATE ends

.code
    
; *********************************************************
    public _spritz_init
_spritz_init:
spritz_init proc fastcall state:dword
    pushad
    mov    edi, state
    xor    eax, eax
    stosb                  ; a = 0
    stosb                  ; i = 0
    stosb                  ; j = 0
    stosb                  ; k = 0
    stosb                  ; z = 0
    inc    eax
    stosb                  ; w = 1
    dec    eax
; for v = 0 to N - 1
;   s[v] = v
init_sbox:                 ; fixed for 256 bytes
    stosb
    inc    al
    jnz    init_sbox
    popad
    ret
spritz_init endp
spritz_initLen equ $ - spritz_init

; *********************************************************
    public _spritz_update
_spritz_update:
spritz_update proc fastcall state:dword
    pushad
    movzx  eax, byte ptr[state][STATE.i]
    movzx  ebx, byte ptr[state][STATE.j]
    mov    dh, [state][STATE.k]
        
    ; i = i + w
    add    al, [state][STATE.w]
    
    ; j = k + S[j + S[i]]
    mov    dl, [state][STATE.s][eax]
    add    bl, dl
    mov    bl, [state][STATE.s][ebx]
    add    bl, dh
    
    ; Swap(S[i], S[j])
    xchg   dl, [state][STATE.s][ebx]
    mov    [state][STATE.s][eax], dl
    
    ; k = i + k + S[j]
    add    dl, al
    add    dl, dh
    
    mov    [state][STATE.i], al
    mov    [state][STATE.j], bl
    mov    [state][STATE.k], dl
    popad
    ret
spritz_update endp
spritz_updateLen equ $ - spritz_update

; *********************************************************
    public _spritz_whip
_spritz_whip:
spritz_whip proc fastcall state:dword
    pushad
    ; for v = 0 to r - 1
    ;   spritz_update()
    xor    ebx, ebx
    mov    bh, (2*N) shr 8
whip_loop:
    call   spritz_update
    dec    ebx
    jnz    whip_loop
    ; w = w + 1
    add    [state][STATE.w], 2    ; when N is power of 2
    popad
    ret
spritz_whip endp
spritz_whipLen equ $ - spritz_whip

; *********************************************************
    public _spritz_crush
_spritz_crush:
spritz_crush proc fastcall state:dword
    pushad
    ; for v = 0 to [N/2] - 1
    xor    eax, eax    
    cdq    
crush_loop:
    ; if S[v] > S[N - 1 - v]
    mov    dl, N - 1
    sub    dl, al
    mov    bl, [state][STATE.s][eax]  ; s[v]
    mov    bh, [state][STATE.s][edx]  ; s[N - 1 - v]
    
    .if bl > bh
      ; Swap (S[v], S[N - 1 - v])
      mov  [state][STATE.s][eax], bh
      mov  [state][STATE.s][edx], bl
    .endif
    
    inc    al             ; N/2
    jns    crush_loop
    
    popad
    ret
spritz_crush endp
spritz_crushLen equ $ - spritz_crush

; *********************************************************
    public _spritz_shuffle
_spritz_shuffle:
spritz_shuffle proc fastcall state:dword
    call   spritz_whip
    call   spritz_crush
    
    call   spritz_whip
    call   spritz_crush
    
    call   spritz_whip
    
    ; a = 0
    and    [state][STATE.a], 0
    ret
spritz_shuffle endp
spritz_shuffleLen equ $ - spritz_shuffle

; *********************************************************
    public _spritz_shuffle_absorb
_spritz_shuffle_absorb:
spritz_shuffle_absorb proc fastcall state:dword
    .if [state][STATE.a] == N/2
      call  spritz_shuffle
    .endif
    ret
spritz_shuffle_absorb endp
spritz_shuffle_absorbLen equ $ - spritz_shuffle_absorb

; *********************************************************
spritz_absorb_nibble proc fastcall state:dword, x:dword
    pushad
    call   spritz_shuffle_absorb
    ; Swap(S[a], S[[N/2] + x])
    xor    eax, eax
    add    dl, N/2
    mov    al, [state][STATE.s][edx]
    mov    bl, [state][STATE.a]            ; ebx = a
    xchg   al, [state][STATE.s][ebx]
    mov    [state][STATE.s][edx], al
    ; a = a + 1
    inc    [state][STATE.a]
    popad
    ret
spritz_absorb_nibble endp
spritz_absorb_nibbleLen equ $ - spritz_absorb_nibble

; *********************************************************
    public _spritz_absorb_byte
_spritz_absorb_byte:
spritz_absorb_byte proc fastcall state:dword, b:dword
    pushad
    mov    ebx, b
    ; spritz_absorb_nibble (LOW (b))
    and    edx, 15
    call   spritz_absorb_nibble
    ; spritz_absorb_nibble (HIGH (b))
    mov    edx, ebx
    shr    edx, 4
    call   spritz_absorb_nibble
    
    popad
    ret
spritz_absorb_byte endp
spritz_absorb_byteLen equ $ - spritz_absorb_byte

; *********************************************************
    public _spritz_absorb
_spritz_absorb:
spritz_absorb proc fastcall state:dword, I:dword, I_len:dword
    pushad
    ; for v = 0 to I.length - 1
    xor    ebx, ebx
    mov    esi, I
absorb_loop:
    ; spritz_absorb_byte(I[v])
    movzx  edx, byte ptr[esi+ebx] ; I[v]
    call   spritz_absorb_byte
    inc    ebx
    cmp    ebx, [esp+32+4]  ; v < I_len
    jnz    absorb_loop
    popad
    retn   4
spritz_absorb endp
spritz_absorbLen equ $ - spritz_absorb 

; *********************************************************
    public _spritz_absorb_stop
_spritz_absorb_stop:
spritz_absorb_stop proc fastcall state:dword
    call spritz_shuffle_absorb
    ; a = a + 1
    inc    [state][STATE.a]
    ret
spritz_absorb_stop endp
spritz_absorb_stopLen equ $ - spritz_absorb_stop

; *********************************************************
    public _spritz_absorb_and_stop
_spritz_absorb_and_stop:
spritz_absorb_and_stop proc fastcall state:dword, I:dword, I_len:dword
    push   [esp+4]
    call   spritz_absorb
    call   spritz_absorb_stop
    ret
spritz_absorb_and_stop endp
spritz_absorb_and_stopLen equ $ - spritz_absorb_and_stop

; *********************************************************
    public _spritz_shuffle_squeeze
_spritz_shuffle_squeeze:
spritz_shuffle_squeeze proc fastcall state:dword
    .if [state][STATE.a] > 0
      call  spritz_shuffle
    .endif
    ret
spritz_shuffle_squeeze endp
spritz_shuffle_squeezeLen equ $ - spritz_shuffle_squeeze

; *********************************************************
; z = S[j + S[i + S[z + k]]]
; return z;
    public _spritz_output
_spritz_output:
spritz_output proc fastcall state:dword
    pushad
    movzx   eax, byte ptr[state][STATE.i]
    movzx   ebx, byte ptr[state][STATE.j]
    movzx   edx, byte ptr[state][STATE.z]
    ; z + k
    add     dl, [state][STATE.k]
    ; i + s[z + k]
    add     al, [state][STATE.s][edx]
    ; j + s[i + s[z + k]]
    add     bl, [state][STATE.s][eax]
    ; z = s[j]
    mov     al, [state][STATE.s][ebx]
    mov     [state][STATE.z], al
    ; return z
    mov     [esp+1ch], eax
    popad
    ret
spritz_output endp
spritz_outputLen equ $ - spritz_output

; *********************************************************
    public _spritz_drip_nosqueeze
_spritz_drip_nosqueeze:
spritz_drip_nosqueeze proc fastcall state:dword
    call   spritz_update
    call   spritz_output
    ret
spritz_drip_nosqueeze endp
spritz_drip_nosqueezeLen equ $ - spritz_drip_nosqueeze

; *********************************************************
    public _spritz_squeeze
_spritz_squeeze:
spritz_squeeze proc fastcall state:dword, P:dword, P_len:dword
    pushad
    call   spritz_shuffle_squeeze
    
    ; for v = 0 to r - 1
    mov    ebx, [esp+32+4] ; r
    mov    edi, edx        ; p
squeeze_loop:
    call   spritz_drip_nosqueeze
    ; P[v] = spritz_drip_nosqueeze()
    stosb
    dec    ebx    
    jnz    squeeze_loop
    
    ; return P
    popad
    retn  4
    ret
spritz_squeeze endp
spritz_squeezeLen equ $ - spritz_squeeze

; *********************************************************
    public _spritz_drip
_spritz_drip:
spritz_drip proc fastcall state:dword
    call spritz_shuffle_squeeze
    call spritz_drip_nosqueeze
    ret
spritz_drip endp
spritz_dripLen equ $ - spritz_drip

    end
    
