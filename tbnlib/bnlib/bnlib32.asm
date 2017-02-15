; *********************************************************************
; bnlib32 v0.1 - Tiny Big Number Library
;
; Functions to perform elementary arithmetic on big numbers
;
; Optimized for size, if you want speed, use openssl, gmp or something else.
;
; Odzhan
; *********************************************************************

.686
.model flat

; extra bit is required to process 4096-bit numbers
MAX_BITS equ 4096 + 1
MAX_BYTE equ MAX_BITS / 8
MAX_WORD equ MAX_BITS / 32

.data
; used only by string conversions
buf1 db MAX_BITS/3 dup (?)
.code

; Load Immediate value into register using smallest code
LI MACRO Op1:REQ, Op2:REQ
  local lo, hi
  
  IF (((OPATTR (Op1)) AND 00000100h NE 0) AND ((OPATTR (Op2)) AND 00010000h NE 0))
    IFIDNI <Op1>, <eax>
      lo textequ <al>
      hi textequ <ah>
    ELSEIFIDNI <Op1>, <ebx>
      lo textequ <bl>
      hi textequ <bh>
    ELSEIFIDNI <Op1>, <ecx>
      lo textequ <cl>
      hi textequ <ch>
    ELSEIFIDNI <Op1>, <edx>
      lo textequ <dl>
      hi textequ <dh>
    ELSE
      %echo Supported registers are EAX, EBX, ECX or EDX
      EXITM
    ENDIF
  ELSE
    EXITM
  ENDIF
  
  IF (Op2 LT 128)
    push Op2
    pop  Op1
  ELSEIF (Op2 LT 256)
    xor  Op1, Op1   ; modifies EFL
                      ; use PUSHFD/POPFD before macro call where required
    mov  lo, Op2
  ELSE
    xor  Op1, Op1
    mov  hi, Op2 SHR 8
    IF (Op2 AND 255) NE 0
      mov  lo, Op2 AND 255
    ENDIF
  ENDIF
ENDM

; *********************************************************************
; Set a to zero
;
; void bn_zero (void *a);
;
; *********************************************************************
  public bn_zero
  public _bn_zero
_bn_zero:
bn_zero proc fastcall uses eax ecx edi a:dword
    mov    edi, a
    xor    eax, eax
    LI     ecx, MAX_WORD
    rep    stosd
    ret
bn_zero endp

; *********************************************************************
; Set a to one
;
; void bn_one (void *a);
;
; *********************************************************************
  public bn_one
  public _bn_one
_bn_one:
bn_one proc fastcall a:dword
    call   bn_zero      ; a = 0
    inc    dword ptr[a] ; a++
    ret
bn_one endp

; *********************************************************************
; Copy b to a
;
; void bn_copy (void *a, void *b);
;
; *********************************************************************
  public bn_copy
  public _bn_copy
_bn_copy:
bn_copy proc fastcall uses ecx esi edi a:dword, b:dword
    mov    esi, b
    mov    edi, a
    LI     ecx, MAX_WORD
    rep    movsd
    ret
bn_copy endp

; *********************************************************************
; Compare a with b, set flags accordingly.
;
; int bn_cmp (void *a, void *b);
;
; a < b  : JL for signed, JB for unsigned
; a > b  : JG for signed, JA for unsigned
; a == b : JE for equal

; bn_cmp returns -1 if a is less than b, 0 if a and b are the same and 
; 1 if a is greater than b.
; *********************************************************************
  public bn_cmp
  public _bn_cmp
_bn_cmp:
bn_cmp proc fastcall uses ecx edx esi edi a:dword, b:dword
    mov    esi, a
    mov    edi, b
    LI     ecx, MAX_WORD
    xor    edx, edx
    std                     ; set direction
    lodsd                   ; subtract 4
    scasd
    lea    esi, [esi+4*ecx]
    lea    edi, [edi+4*ecx]
    repz   cmpsd            ; repeat while equal
    cld
    pushfd                  ; save efl
    seta   dl               ; set dl if above
    sbb    eax, eax
    xor    eax, edx
    popfd
    ret
bn_cmp endp

; *********************************************************************
; Find most significant bit in a, return number of bits in eax
;
; uint32_t bn_num_bits (void *a);
;
; *********************************************************************
  public bn_num_bits
  public _bn_num_bits
_bn_num_bits:
bn_num_bits proc fastcall uses ecx a:dword
    mov    eax, a
    LI     ecx, (MAX_BITS-1)
scan_loop:
    bt     [eax], ecx
    jc     exit_scan
    dec    ecx
    jns    scan_loop
exit_scan:
    lea    eax, [ecx+1]
    ret
bn_num_bits endp

; *********************************************************************
; Return number of bytes in big number
;
; void bn_num_bytes (void *a);
;
; *********************************************************************
  public bn_num_bytes
  public _bn_num_bytes
_bn_num_bytes:
bn_num_bytes proc fastcall a:dword
    call  bn_num_bits
    shr   eax, 3
    ret
bn_num_bytes endp

; *********************************************************************
; Return number of words in big number
;
; void bn_num_words (void *a);
;
; *********************************************************************
  public bn_num_words
  public _bn_num_words
_bn_num_words:
bn_num_words proc fastcall a:dword
    call  bn_num_bits
    shr   eax, 5
    ret
bn_num_words endp

; *********************************************************************
; Shift a left by 1, same as multiplying a by 2 or adding a to a
;
; void bn_lshift1 (void *a);
;
; a <<= 1
; *********************************************************************
  public bn_lshift1
  public _bn_lshift1
_bn_lshift1:
bn_lshift1 proc fastcall uses eax ecx a:dword
    mov    eax, a
    pushfd
    LI     ecx, MAX_WORD
    popfd
shl_loop:
    rcl    dword ptr[eax], 1
    lea    eax, [eax+4]
    loop   shl_loop
    ret
bn_lshift1 endp

; *********************************************************************
; Shift a left by constant
;
; void bn_lshift (void *a, int x);
;
; a <<= x
; *********************************************************************
  public bn_lshift
  public _bn_lshift
_bn_lshift:
bn_lshift proc fastcall uses ebx ecx edx esi a:dword, x:dword
    mov    esi, a
    mov    ecx, x
    LI     ebx, MAX_WORD
    lea    esi, [esi+4*ebx-2*4]
    xor    eax, eax
    mov    edx, [esi+1*4]
    shld   eax, edx, cl
    mov    edi, eax
    .while ebx != 0
      mov  eax, [esi]
      shld edx, eax, cl
      mov  [esi+1*4], edx
      mov  edx, eax
      sub  esi, 4 
      dec  ebx
    .endw
    shl    edx, cl
    mov    eax, edi
    mov    [esi+1*4], edx
    ret
bn_lshift endp

; *********************************************************************
; Shift a right by 1, same as dividing a by 2
;
; void bn_rshift1 (void *a);
;
; a >>= 1
; *********************************************************************
  public bn_rshift1
  public _bn_rshift1
_bn_rshift1:
bn_rshift1 proc fastcall uses eax ecx a:dword
    mov    eax, a
    LI     ecx, MAX_WORD
shr_loop:
    rcr    dword ptr[eax+4*ecx-4], 1
    loop   shr_loop
    ret
bn_rshift1 endp

; *********************************************************************
; Shift a right by x
;
; void bn_rshift (void *a, int x);
;
; a >>= x
; *********************************************************************
  public bn_rshift
  public _bn_rshift
_bn_rshift:
bn_rshift proc fastcall uses eax ebx ecx a:dword, x:dword
    mov    esi, a
    mov    ecx, x
    LI     ebx, MAX_WORD
    
    xor    eax, eax
    mov    edx, [esi]
    shrd   eax, edx, cl
    mov    edi, eax
    .while ebx != 0
      mov  eax, [esi+4]
      shrd edx, eax, cl
      mov  [esi], edx
      mov  edx, eax
      add  esi, 4   
      dec  ebx
    .endw
    shr   edx, cl
    mov   eax, edi
    mov   [esi], edx
    ret
bn_rshift endp


; *********************************************************************
; Add b to a, store result in r
;
; void bn_add (void *r, void *a, void *b);
;
; r = a + b
; *********************************************************************
  public bn_add
  public _bn_add
_bn_add:
bn_add proc fastcall uses eax ebx ecx esi edi r:dword, a:dword, b:dword
    mov    edi, r
    mov    esi, a
    mov    ebx, b
    LI     ecx, MAX_WORD
add_loop:
    lodsd              ; load a[i]
    adc    eax, [ebx]  ; add b[i]
    stosd              ; r[i] = a[i] + b[i]
    lea    ebx, [ebx+4]
    loop   add_loop
    ret
bn_add endp

; *********************************************************************
; Add b to a (for private use)
;
; void bn_addx (void *a, void *b);
;
; a += b
; *********************************************************************:
bn_addx proc fastcall uses eax ebx ecx edx a:dword, b:dword
    local r[MAX_WORD]:dword
    
    mov   ebx, a
    
    ; r = a + b
    push  b
    mov   edx, a
    lea   ecx, r
    call  bn_add
    
    ; a = r
    mov   ecx, ebx
    lea   edx, r
    call  bn_copy
    
    ret
bn_addx endp

; *********************************************************************
; Add 32-bit word b to big number a
;
; void bn_add_word (void *a, uint32_t b);
;
; a = a + b
; *********************************************************************
  public bn_add_word
  public _bn_add_word
_bn_add_word:
bn_add_word proc fastcall uses eax ecx esi a:dword, b:dword
    mov    esi, a
    LI     ecx, MAX_WORD
    xor    eax, eax
addw_loop:
    adc    [esi], edx
    lea    esi, [esi+4]
    mov    edx, eax
    jnc    addw_exit
    loop   addw_loop
addw_exit:
    ret
bn_add_word endp
  
; *********************************************************************
; Subtract b from a
;
; void bn_sub (void *a, void *b);
;
; r = a - b
; *********************************************************************
  public bn_sub
  public _bn_sub
_bn_sub:
bn_sub proc fastcall uses eax ebx ecx esi edi r:dword, a:dword, b:dword
    mov    edi, r
    mov    esi, a
    mov    ebx, b
    LI     ecx, MAX_WORD
sub_loop:
    lodsd              ; load a[i]
    sbb    eax, [ebx]  ; subtract b[i]
    stosd              ; r[i] = a[i] - b[i]
    lea    ebx, [ebx+4]
    loop   sub_loop
    ret
bn_sub endp

; *********************************************************************
; Subtract b from a (for private use)
;
; void bn_addx (void *a, void *b);
;
; a -= b
; *********************************************************************:
bn_subx proc fastcall uses ebx ecx edx a:dword, b:dword
    local r[MAX_WORD]:dword
    
    mov   ebx, a
    
    ; r = a - b
    push  b
    mov   edx, ecx
    lea   ecx, r
    call  bn_sub
    
    ; a = r
    mov   ecx, ebx
    lea   edx, r
    call  bn_copy
    
    ret
bn_subx endp

; *********************************************************************
; Subtract 32-bit word b from big number a
;
; void bn_sub_word (void *a, uint32_t b);
;
; a = a - b
; *********************************************************************
  public bn_sub_word
  public _bn_sub_word
_bn_sub_word:
bn_sub_word proc fastcall uses eax ecx edx esi a:dword, b:dword
    mov    esi, a
    mov    edx, b
    LI     ecx, MAX_WORD
    xor    eax, eax
subw_loop:
    sbb    [esi], edx
    mov    edx, eax
    lea    esi, [esi+4]
    jnc    subw_exit
    loop   subw_loop
subw_exit:
    ret
bn_sub_word endp

; *********************************************************************
; Divide a by b, store quotient in q, remainder in r
;
; void bn_div (void *q, void *r, void *a, void *b);
;  
; {q, r} = a / b
; *********************************************************************
  public bn_div
  public _bn_div
_bn_div:
bn_div proc fastcall uses ebx ecx edx esi edi q:dword, r:dword, a:dword, b:dword               
    mov    esi, q
    mov    edi, r
    mov    ebx, a
    mov    ecx, a
    mov    edx, b
    ; get number of bits for a
    call   bn_num_bits
    test   eax, eax
    .if !zero?
      xchg   eax, ebp
      ; q = 0
      mov    ecx, esi ; q
      call   bn_zero
      ; r = 0
      mov    ecx, edi ; r
      call   bn_zero
      .repeat
        ; q <<= 1
        clc
        mov    ecx, esi ; q
        call   bn_lshift1
        ; a & 1
        bt     [ebx], ebp
        ; r <<= 1
        mov    ecx, edi ; r
        call   bn_lshift1
        ; if (r >= b) r -= b;
        call   bn_cmp
        .if !carry?
          call bn_subx
          ; q |= 1
          or     byte ptr [esi], 1
        .endif
        dec    ebp
      .until sign?
    .endif
    ret
bn_div endp

; *********************************************************************
; Divide a by b, return remainder
;
; uint32_t bn_div_word (void *a, uint32_t b);
;  
; a /= b
; *********************************************************************
  public bn_div_word
  public _bn_div_word
_bn_div_word:
bn_div_word proc fastcall uses esi ebx edi ecx a:dword, b:dword
    mov    ebx, b
    xor    edx, edx
    test   ebx, ebx
    .if !zero?
      mov    esi, a
      call   bn_num_words
      xchg   eax, ecx
      xor    edx, edx
      .repeat
        mov  eax, [esi+4*ecx]
        div  ebx
        mov  [esi+4*ecx], eax
        dec  ecx
      .until sign?
    .endif
    xchg   eax, edx
    ret
bn_div_word endp

; *********************************************************************
; Multiply a by b and store result in r which should hold MAX_WORD*2
;
; void bn_mul (void *r, void *a, void *b);
;  
; r = a * b
; *********************************************************************
  public bn_mul
  public _bn_mul
_bn_mul:
bn_mul proc fastcall uses esi ebx edi ecx edx r:dword, a:dword, b:dword
    local t[MAX_WORD*2]:dword
    
    mov    esi, a
    mov    ebx, b
    push   r
    
    ; t = 0
    lea    edi, t
    push   edi
    xor    eax, eax
    LI     ecx, MAX_WORD * 2
    rep    stosd
    
    LI     ecx, MAX_WORD
set_j:
    push   ecx
    push   ebx
    push   esi
    push   edi
    LI     ecx, MAX_WORD
load_x:
    mov    eax, [esi]         ; eax      = a[i]
    mul    dword ptr[ebx]     ; eax:edx  = a[i] * b[j]
    push   edi
    add    eax, [edi]
    stosd                     ; r[i+j]   = eax
    xchg   eax, edx
    adc    eax, [edi]         ; r[i+j+1] = edx
    stosd
add_carry:
    adc    dword ptr[edi], 0
    lea    edi, [edi+4]
    jc     add_carry
    pop    edi
    add    ebx, 4
    scasd
    loop   load_x
    pop    edi
    pop    esi
    pop    ebx
    pop    ecx
    scasd
    lodsd 
    loop   set_j
    pop    edx    ; t
    pop    ecx    ; r
    ; r = t
    call   bn_copy
    ret
bn_mul endp

; *********************************************************************
; Modulo operation
;
; void bn_modx (void *a, void *b);
;
; a *= b
; *********************************************************************:
bn_mulx proc fastcall uses ebx ecx edx a:dword, b:dword
    local r[MAX_WORD*2]:dword
    
    mov   ebx, a
    
    ; r = a * b
    push  b
    mov   edx, ecx
    lea   ecx, r
    call  bn_mul
    
    ; a = r
    mov   ecx, ebx
    lea   edx, r
    call  bn_copy
    
    ret
bn_mulx endp

; *********************************************************************
; Multiply a by b
;
; void bn_mul_word (void *a, uint32_t b);
;  
; r = a * b
; *********************************************************************
  public bn_mul_word
  public _bn_mul_word
_bn_mul_word:
bn_mul_word proc fastcall uses eax ebx ecx edx esi edi a:dword, b:dword
    local t[MAX_WORD*2]:dword
    
    mov    esi, a
    mov    ebx, b
    ; t = 0
    lea    ecx, t
    mov    edi, ecx
    call   bn_zero
    
    push   esi
    LI     ecx, MAX_WORD
mulw_loop:
    lodsd               ; load a[i]
    mul    ebx          ; 
    add    [edi], eax   ; t[i] = a[i] * b;
    adc    [edi+4], edx
    scasd
    loop   mulw_loop
    pop    ecx
    ; a = t
    lea    edx, t
    call   bn_copy
    ret
bn_mul_word endp

; *********************************************************************
; Return result of a * a
;
; void bn_sqr (void *r, void *a);
;
; *********************************************************************
  public bn_sqr
  public _bn_sqr
_bn_sqr:
bn_sqr proc fastcall uses ebx ecx edx esi edi r:dword, a:dword
    local t1[MAX_WORD*2]:dword
    local t2[MAX_WORD*2]:dword
    
    mov    esi, r
    mov    edi, a
    
    ; t1 = a
    lea    ecx, t1
    mov    edi, edi
    call   bn_copy
    
    ; t2 = a
    lea    ecx, t2
    mov    edx, edi
    call   bn_copy
    
    ; r = t1 * t2
    push   ecx
    lea    edx, t1
    mov    ecx, esi
    call   bn_mul
    ret
bn_sqr endp
; *********************************************************************
; Return result of a * a
;
; void bn_sqr (void *r, void *a);
;
; *********************************************************************
  public bn_mod_sqr
  public _bn_mod_sqr
_bn_mod_sqr:
bn_mod_sqr proc fastcall uses esi r:dword, a:dword, m:dword
    local t1[MAX_WORD*2]:dword
    local t2[MAX_WORD*2]:dword
    
    mov    esi, r
    
    ; t1 = a
    lea    ecx, t1
    call   bn_copy
    
    ; t2 = t1 * t1
    push   a
    lea    edx, t1
    lea    ecx, t2
    call   bn_mul
    
    ; r = t2 % m
    push   m
    lea    edx, t2
    mov    ecx, esi
    call   bn_mod
    ret
bn_mod_sqr endp

; *********************************************************************
; Return result of a % m
;
; void bn_mod (void *r, void *a, void *m);
;
; *********************************************************************
  public bn_mod
  public _bn_mod
_bn_mod:
bn_mod proc fastcall uses eax ebx ecx edx esi edi r:dword, a:dword, m:dword
    local t[MAX_WORD]:dword
    
    mov    esi, r
    lea    ebx, t
    
    ; store copy of a before clearing r
    ; t = a
    mov    ecx, ebx
    call   bn_copy
    
    ; r = 0
    mov    ecx, esi
    call   bn_zero
    
    ; get number of bits for t
    mov    ecx, ebx
    call   bn_num_bits
    .repeat
      ; t & 1
      bt   [ebx], eax
      ; r <<= 1
      mov  ecx, esi
      call bn_lshift1
      ; if (r >= m) r -= m;
      mov  edx, m
      call bn_cmpsub
      dec  eax
    .until sign?
    ret
bn_mod endp

; *********************************************************************
; Modulo operation
;
; void bn_modx (void *a, void *b);
;
; a %= b
; *********************************************************************:
bn_modx proc fastcall uses ebx ecx edx a:dword, b:dword
    local r[MAX_WORD]:dword
    
    mov   ebx, a
    
    ; r = a % b
    push  b
    mov   edx, ecx
    lea   ecx, r
    call  bn_mod
    
    ret
bn_modx endp

; *********************************************************************
; Returns the remainder of a divided by b
;
; void bn_mod_word (void *a, uint32_t b);
;  
; return a % b
; *********************************************************************
  public bn_mod_word
  public _bn_mod_word
_bn_mod_word:
bn_mod_word proc fastcall uses ebx ecx edx esi a:dword, b:dword
    mov    ebx, b
    test   ebx, ebx
    .if !zero?
      mov    esi, a
      call   bn_num_words
      xchg   eax, ecx
      xor    edx, edx
      .repeat
        mov  eax, [esi+4*ecx]
        div  ebx
        dec  ecx
      .until sign?
    .endif
    xchg   eax, edx
    ret
bn_mod_word endp

; *********************************************************************
; Adds a to b modulo m and places the non-negative remainder in r
;
; void bn_mod_add (void *r, void *a, void *b, void *m);
;
; *********************************************************************
  public bn_mod_add
  public _bn_mod_add
_bn_mod_add:
bn_mod_add proc fastcall uses ecx edx esi edi ebx r:dword, a:dword, b:dword, m:dword
    local t1[MAX_WORD]:dword
    local t2[MAX_WORD]:dword
    
    mov    esi, r
    
    ; t1 = a
    lea    ecx, t1
    call   bn_copy
    
    ; t2 = t1 + b
    push   b
    lea    edx, t1
    lea    ecx, t2
    call   bn_add
    
    ; r = t2 % m
    push   m
    lea    edx, t2
    mov    ecx, esi
    call   bn_mod
    ret
bn_mod_add endp

; *********************************************************************
; Subtracts b from a modulo m and places the non-negative result in r
;
; void bn_mod_sub (void *r, void *a, void *b, void *m);
;
; *********************************************************************
  public bn_mod_sub
  public _bn_mod_sub
_bn_mod_sub:
bn_mod_sub proc fastcall uses ebx ecx edx esi edi r:dword, a:dword, b:dword, m:dword
    local t1[MAX_WORD]:dword
    local t2[MAX_WORD]:dword
    
    mov    esi, r
    
    ; t1 = a
    lea    ecx, t1
    call   bn_copy
    
    ; t2 = t1 - b
    push   b
    lea    edx, t1
    lea    ecx, t2
    call   bn_sub
    
    ; r = t2 % m
    push   m
    lea    edx, t2
    mov    ecx, esi
    call   bn_mod 
    ret
bn_mod_sub endp

; *********************************************************************
; Multiplies a by b and finds the non-negative remainder respective 
; to modulus m
;
; void bn_mod_mul(void *r, void *a, void *b, void *m);
;
; *********************************************************************
  public bn_mod_mul
  public _bn_mod_mul
_bn_mod_mul:
bn_mod_mul proc fastcall uses eax ebx ecx edx esi edi r:dword, a:dword, b:dword, m:dword
    local t1[MAX_WORD]:dword
    local t2[MAX_WORD]:dword
    
    mov    esi, r
    
    ; t1 = a
    lea    ecx, t1
    call   bn_copy
    
    ; t2 = t1 * b
    push   b
    lea    edx, t1
    lea    ecx, t2
    call   bn_mul
    
    ; r = t2 % m
    push   m
    lea    edx, t2
    mov    ecx, esi
    call   bn_mod
    ret
bn_mod_mul endp

; *********************************************************************
; Exponentiation, raise a by b
;
; void bn_exp (void *r, void *a, void *b);
;
; *********************************************************************
  public bn_exp
  public _bn_exp
_bn_exp:
bn_exp proc fastcall uses esi edi ebx r:dword, a:dword, b:dword
    local t[MAX_WORD*2]:dword
    local bitlen       :dword
    
    mov    esi, r
    mov    ebx, b
    lea    edi, t
    
    ; r = 1
    call   bn_one
    ; t = b
    mov    ecx, edi
    call   bn_copy
    
    mov    ecx, ebx    ; b
    call   bn_num_bits
    mov    [bitlen], eax
    xor    eax, eax
exp_loop:
    ; if (e & 1)
    bt     [ebx], eax
    
    ; r = r * t;
    .if carry?
      mov    ecx, esi ; r
      mov    edx, esi ; r
      call   bn_mulx
    .endif
    
    ; t = t * t;
    mov    ecx, edi
    mov    edx, edi
    call   bn_mulx
    
    inc    eax
    cmp    eax, [bitlen]
    jbe    exp_loop
    ret
bn_exp endp

; *********************************************************************
; If a is greater than or equal to b, subtract b from a
;
; void bn_cmpsub (void *a, void *b);
;
; *********************************************************************
bn_cmpsub proc fastcall uses eax a:dword, b:dword
    call   bn_cmp    ; if (a >= b)
    .if !carry?
      call bn_subx    ; a = a - b
    .endif
    ret
bn_cmpsub endp

; *********************************************************************
; Add b to a, if a is greater than m, subtract m
;
; void bn_addcmpsub (void *a, void *b, void*m);
;
; *********************************************************************
bn_addcmpsub proc fastcall uses eax ecx edx a:dword, b:dword, m:dword
    ; a += b
    call   bn_addx
    ; if (a >= m)
    mov    edx, m
    call   bn_cmp
    .if !carry?
      ; a = a - m
      call bn_subx
    .endif
    ret
bn_addcmpsub endp

; *********************************************************************
; Modular multiplication where a and b are less than m
;
; void bn_mod_mul_quick (void *r, void *a, void *b, void*m);
;
; *********************************************************************
  public bn_mod_mul_quick
  public _bn_mod_mul_quick
_bn_mod_mul_quick:
bn_mod_mul_quick proc fastcall uses eax ebx ecx edx esi edi r:dword, a:dword, b:dword, m:dword
    local t[MAX_WORD]:dword
    local x[MAX_WORD]:dword
    local bitlen     :dword
    
    mov   esi, r
    mov   edi, a
    mov   ebx, b
    
    ; t = a
    lea   ecx, t
    call  bn_copy
    
    ; x = 0
    lea   ecx, x
    call  bn_zero
    
    mov   ecx, b
    call  bn_num_bits
    mov [bitlen], eax
    
    xor eax, eax
    .repeat 
      ; b & 1
      bt  [ebx], eax
      
      .if carry?
        ; x = addcmpsub (x, t, m);
        push  m
        lea   edx, t
        lea   ecx, x
        call  bn_addcmpsub
      .endif
      
      ; t = addcmpsub (t, t, m);
      push  m
      lea   ecx, t
      mov   edx, ecx
      call  bn_addcmpsub
      
      inc   eax
      dec [bitlen]
    .until sign?
    
    ; r = x
    lea  edx, x
    mov  ecx, esi
    call bn_copy
    ret
bn_mod_mul_quick endp

; *********************************************************************
; Modular Exponentiation, raise a by b modulo m
; 
; void bn_mod_exp (void *r, void *a, void *b, void *m);
;
; *********************************************************************
  public bn_mod_exp
  public _bn_mod_exp
_bn_mod_exp:
bn_mod_exp proc fastcall uses esi edi ebx r:dword, a:dword, b:dword, m:dword
    local p[MAX_WORD]:dword
    local x[MAX_WORD]:dword
    
    mov   edi, r
    mov   esi, a
    
    ; x = 1
    lea   ecx, x
    call  bn_one

    ; get bits (b)
    mov   ecx, b
    call  bn_num_bits
    xchg  ebx, eax
    
    ; p = a
    lea   ecx, p
    mov   edx, esi
    call  bn_copy
    xor   eax, eax
    mov   esi, b
exp_loop:    
    ; if (b & 1)
    bt  [esi], eax
    
    ; x = mulmod (x, p, m)
    .if carry?
      push m
      lea  ecx, p
      push ecx
      lea  ecx, x
      lea  edx, x
      call bn_mod_mul
    .endif
    
    ; p = mulmod (p, p, m)
    push m
    lea  edx, p
    push edx
    mov  ecx, edx
    call bn_mod_mul
    
    inc  eax
    dec  ebx
    jns  exp_loop
    
    ; r = x
    lea  edx, x
    mov  ecx, edi
    call bn_copy
    ret
bn_mod_exp endp

; *********************************************************************
; Modular multiplicative inverse
;
; void bn_mod_inverse (void *r, void *a, void *m);
;
; *********************************************************************
  public bn_mod_inv
  public _bn_mod_inv
_bn_mod_inv:
bn_mod_inv proc fastcall uses esi edi ebx ebp r:dword, a:dword, m:dword
    local j[MAX_WORD]:dword
    local i[MAX_WORD]:dword
    local b[MAX_WORD]:dword
    local d[MAX_WORD]:dword
    local _c[MAX_WORD]:dword
    local y[MAX_WORD]:dword
    
    ; c = a
    lea    ecx, b
    mov    edx, m
    call   bn_copy
    
    ; b = m
    lea    ecx, b
    mov    edx, m
    call   bn_copy
    
    ; j = 1
    lea    ecx, j
    call   bn_one
    
    ; i = 0
    lea    ecx, i
    call   bn_zero
  mod_inv_loop:
    ; x = b / d;
    ; y = b % d;
    ;push   m
    ;push   b
    ;lea    edx, r
    ;lea    ecx, q
    call   bn_div
    
    ; b = c;
    lea    edx, _c
    lea    ecx, b
    call   bn_copy
    
    ; c = y;
    lea    edx, y
    lea    ecx, _c
    call   bn_copy
    
    ; y = j;
    lea    edx, j
    lea    ecx, y
    call   bn_copy
      
    ; j = i - j * x;
    ; j = j * x;
    lea    edx, _c
    lea    ecx, j
    call   bn_mulx
    
    ; i = i - j
    lea    edx, j
    lea    ecx, i
    call   bn_subx
    
    ; i = y;
    call   bn_copy
    jmp    mod_inv_loop
    
    ; i += m
    call   bn_add
    ret
bn_mod_inv endp

; *********************************************************************
; Convert hexadecimal string to big number
;
; void bn_hex2bn (void *bn, const char *hex);
;
; *********************************************************************
  public _bn_hex2bn
  public bn_hex2bn
_bn_hex2bn:
bn_hex2bn proc fastcall uses ebx ecx edx esi edi bn:dword, s:dword
    ; bn = 0
    call   bn_zero
    mov   esi, edx   ; s
    mov   edi, ecx   ; bn
    or    ecx, -1
    .repeat
      inc ecx
    .until byte ptr[esi+ecx] == 0
    cdq
    .while ecx != 0
      mov   al, byte ptr[esi+ecx-1]
      .if (al >= 'a' && al <= 'f')
        sub al, 'a' - 10
      .elseif (al >= 'A' && al <= 'F')
        sub al, 'A' - 10
      .elseif (al >= '0' && al <= '9')
        sub al, '0'
      .endif
      test  dl, 1
      .if !zero?
        shl al, 4
      .endif
      mov   ebx, edx
      shr   ebx, 1
      or    byte ptr[edi+ebx], al
      inc   edx
      dec   ecx
    .endw
    ret
bn_hex2bn endp

; *********************************************************************
; Convert big number to hexadecimal string, return string
;
; char* bn_bn2hex (void *bn);
;
; *********************************************************************
  public _bn_bn2hex
  public bn_bn2hex
_bn_bn2hex:
bn_bn2hex proc fastcall uses ebx ecx edx esi edi bn:dword
    mov   esi, bn
    mov   edi, offset buf1
    .if dword ptr[ecx] == 0      ; problem where 1st 32-bits of big number are zero?
      mov   byte ptr[edi], '0'   ; store zero in case nothing to process
      mov   byte ptr[edi+1], 0
    .else
      call  bn_num_bytes
      xchg  eax, ecx
      mov   byte ptr[edi+2*ecx], 0
      .while ecx != 0
        movzx edx, byte ptr[esi+ecx-1]
        mov   eax, edx
        shr   eax, 4
        .if al < 10
          add al, '0'
        .else
          add al, '7'
        .endif
        stosb
        mov   eax, edx
        and   eax, 15
        .if al < 10
          add al, '0'
        .else
          add al, '7'
        .endif
        stosb
        dec   ecx
      .endw
    .endif
    mov     eax, offset buf1
    ret
bn_bn2hex endp

; *********************************************************************
; Convert decimal string to big number
;
; void bn_dec2bn (void *bn, const char *s);
;
; *********************************************************************
  public _bn_dec2bn
  public bn_dec2bn
_bn_dec2bn:
bn_dec2bn proc fastcall uses ebx ecx edx esi edi bn:dword, s:dword
    ; bn = 0
    call   bn_zero
    mov    edi, bn
    mov    esi, s
    xor    eax, eax
    cdq
    or     ebx, -1
    .repeat
      inc  ebx
    .until byte ptr[esi+ebx] == 0
    
    .while ebx != 0
      ; bn = bn * 10
      mov    dl, 10
      call   bn_mul_word
    
      ; bn = bn + s[i] - '0'
      movzx  edx, byte ptr[esi]
      inc    esi
      sub    dl, '0'
      call   bn_add_word
      dec    ebx
    .endw
    ret
bn_dec2bn endp

; *********************************************************************
; Convert big number to decimal string, return string
;
; char* bn_bn2dec (void *bn);
;
; *********************************************************************
  public _bn_bn2dec
  public bn_bn2dec
_bn_bn2dec:
bn_bn2dec proc fastcall uses ebx ecx edx esi edi bn:dword
    local t[MAX_WORD]:dword
   
    ; t = bn
    mov    edx, bn
    lea    ecx, t
    call   bn_copy
    
    mov    edi, offset buf1
    mov    esi, edi
    xor    ebx, ebx
    mov    byte ptr[esi], '0'
    mov    byte ptr[esi+1], bl
    .while dword ptr[ecx] != 0
      ; bn /= 10
      push 10
      pop  edx
      call bn_div_word
      add  al, '0'
      stosb
      inc  ebx
    .endw
    mov    ecx, ebx
    mov    edx, esi
    shr    ecx, 1       ; /= 2
    .if !zero?
      mov  byte ptr[esi+ebx], 0
      ; reverse string
      .repeat
        dec  edi
        mov  al, [edi]
        xchg al, [edx]
        inc  edx
        mov  [edi], al
        dec  ecx
      .until zero?
    .endif
    xchg   eax, esi
    ret
bn_bn2dec endp

; *********************************************************************
; Convert big number to binary
;
; void bn_bn2bin (void *bn, uint8_t bin[]);
;
; *********************************************************************
  public _bn_bn2bin
  public bn_bn2bin
_bn_bn2bin:
bn_bn2bin proc fastcall uses ecx esi edi bn:dword, bin:dword
    mov    esi, bn
    mov    edi, bin
    call   bn_num_bytes
    xchg   eax, ecx
    .repeat
      mov  al, [esi+ecx-1]
      stosb
      dec  ecx
    .until zero?
    ret
bn_bn2bin endp

; *********************************************************************
; Convert binary to big number
;
; void bn_bin2bn (uint8_t bin[], size_t len, void *bn);
;
; *********************************************************************
  public _bn_bin2bn
  public bn_bin2bn
_bn_bin2bn:
bn_bin2bn proc fastcall uses ecx esi edi bin:dword, len:dword, bn:dword
    mov    edi, bn
    mov    esi, bin
    mov    ecx, len
    .repeat
      mov  al, [esi+ecx-1]
      stosb
      dec  ecx
    .until zero?
    ret
bn_bin2bn endp

    end
    
