

; SHA-2 in x86 assembly
; Odzhan

ifndef SHA2_H
SHA2_H  EQU <>

SHA256_CBLOCK         EQU 64
SHA256_DIGEST_LENGTH  EQU 32
SHA256_LBLOCK         EQU SHA256_DIGEST_LENGTH/4

SHA256_CTX  struct 
  union state
    v8  byte  SHA256_DIGEST_LENGTH dup (?)
    v32 dword SHA256_DIGEST_LENGTH/4 dup (?)
    v64 qword SHA256_DIGEST_LENGTH/8 dup (?)
  ends
  union buffer
    v8  byte  SHA256_CBLOCK   dup (?)
    v32 dword SHA256_CBLOCK/4 dup (?)
    v64 qword SHA256_CBLOCK/8 dup (?)
  ends
  len   dword 2 dup (?)
SHA256_CTX  ends

SHA512_CBLOCK         EQU 128
SHA512_DIGEST_LENGTH  EQU 64
SHA512_LBLOCK         EQU SHA512_DIGEST_LENGTH/4

SHA512_CTX  struct 
  union state
    v8 byte   SHA512_DIGEST_LENGTH   dup (?)
    v32 dword SHA512_DIGEST_LENGTH/4 dup (?)
    v64 qword SHA512_DIGEST_LENGTH/8 dup (?)
  ends
  union buffer
    v8  byte  SHA512_CBLOCK   dup (?)
    v32 dword SHA512_CBLOCK/4 dup (?)
    v64 qword SHA512_CBLOCK/8 dup (?)
  ends
  len   dword 2 dup (?)
SHA512_CTX  ends

endif 