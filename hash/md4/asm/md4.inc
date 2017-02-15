

; MD4 in x86 assembly
; Odzhan

ifndef MD4_H
MD4_H EQU <>

MD4_CBLOCK        EQU 64
MD4_DIGEST_LENGTH EQU 16
MD4_LBLOCK        EQU MD4_DIGEST_LENGTH / 4

MD4_CTX struct 
  union state
    v8  byte  MD4_DIGEST_LENGTH dup (?)
    v32 dword MD4_LBLOCK dup (?)
  ends
  buffer byte MD4_CBLOCK dup (?)
  len    dword 2 dup (?)
MD4_CTX ends

endif 