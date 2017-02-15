

; RC4 in x64 assembly
; Odzhan

ifndef RC4_H
RC4_H equ <>

RC4_BUFSIZ equ 256 ; do not change

RC4_CTX struct
  x   dword ?
  y   dword ?
  s   byte 256 dup (?)
  buf byte RC4_BUFSIZ dup (?)
RC4_CTX ends

endif