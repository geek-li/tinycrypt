

; Blowfish in x86 assembly
; Odzhan

ifndef BF_H
BF_H equ <>

BF_ROUNDS  equ 16
BF_BLOCK   equ  8
BF_MIN_KEY equ  4
BF_MAX_KEY equ 56

BF_KEY struct
  p     dd BF_ROUNDS+2 dup (?)
  
  sbox1 dd 256 dup (?)
  sbox2 dd 256 dup (?)
  sbox3 dd 256 dup (?)
  sbox4 dd 256 dup (?)
BF_KEY ends

endif