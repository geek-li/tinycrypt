

; MD5 in x86 assembly
; Odzhan

ifndef MD5_H
MD5_H	EQU	<>

MD5_CBLOCK	      EQU	64
MD5_DIGEST_LENGTH	EQU	16
MD5_LBLOCK	      EQU	MD5_DIGEST_LENGTH / 4

MD5_CTX	struct 
  union state
    v8	byte  MD5_DIGEST_LENGTH dup (?)
    v32	dword MD5_LBLOCK dup (?)
  ends
  buffer	byte MD5_CBLOCK dup (?)
  len	    dword 2 dup (?)
MD5_CTX	ends

endif 