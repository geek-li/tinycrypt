; Listing generated by Microsoft (R) Optimizing Compiler Version 16.00.40219.01 

	TITLE	C:\odzhan\tinycrypt\hash\crc\crc32.c
	.686P
	.XMM
	include listing.inc
	.model	flat

INCLUDELIB LIBCMT
INCLUDELIB OLDNAMES

PUBLIC	_xcrc32
; Function compile flags: /Ogspy
;	COMDAT _xcrc32
_TEXT	SEGMENT
_crc$ = 8						; size = 4
_inlen$ = 12						; size = 4
_in$ = 16						; size = 4
_xcrc32	PROC						; COMDAT
; File c:\odzhan\tinycrypt\hash\crc\crc32.c
; Line 13
	push	ebp
	mov	ebp, esp
; Line 18
	cmp	DWORD PTR _inlen$[ebp], 0
	mov	eax, DWORD PTR _crc$[ebp]
	not	eax
	je	SHORT $LN11@xcrc32
	mov	ecx, DWORD PTR _in$[ebp]
	push	esi
$LL5@xcrc32:
; Line 20
	movzx	edx, BYTE PTR [ecx]
	dec	DWORD PTR _inlen$[ebp]
	xor	eax, edx
	push	8
	inc	ecx
	pop	edx
$LL3@xcrc32:
; Line 23
	mov	esi, eax
	and	esi, 1
	imul	esi, -306674912				; edb88320H
	shr	eax, 1
	xor	eax, esi
	dec	edx
	jne	SHORT $LL3@xcrc32
; Line 18
	cmp	DWORD PTR _inlen$[ebp], edx
	jne	SHORT $LL5@xcrc32
	pop	esi
$LN11@xcrc32:
; Line 34
	not	eax
; Line 35
	pop	ebp
	ret	0
_xcrc32	ENDP
_TEXT	ENDS
END