

; SHA-1 in x86 assembly
; Odzhan

ifndef SHA_H
SHA_H	EQU	<>

SHA_CBLOCK	      EQU	64
SHA_DIGEST_LENGTH	EQU	20
SHA_LBLOCK	      EQU	SHA_DIGEST_LENGTH / 4

SHA_CTX	struct 
  union state
    v8	byte  SHA_DIGEST_LENGTH dup (?)
    v32	dword SHA_LBLOCK dup (?)
  ends
  buffer byte SHA_CBLOCK dup (?)
  len	   dword 2 dup (?)
SHA_CTX	ends

endif 