; Listing generated by Microsoft (R) Optimizing Compiler Version 16.00.40219.01 

	TITLE	C:\odzhan\tinycrypt\block\sea\sea.c
	.686P
	.XMM
	include listing.inc
	.model	flat

INCLUDELIB LIBCMT
INCLUDELIB OLDNAMES

PUBLIC	_XOR
; Function compile flags: /Ogspy
;	COMDAT _XOR
_TEXT	SEGMENT
_x$ = 8							; size = 4
_y$ = 12						; size = 4
_XOR	PROC						; COMDAT
; File c:\odzhan\tinycrypt\block\sea\sea.c
; Line 20
	mov	eax, DWORD PTR _y$[esp-4]
	mov	ecx, DWORD PTR [eax]
	mov	eax, DWORD PTR _x$[esp-4]
	xor	DWORD PTR [eax], ecx
; Line 23
	ret	0
_XOR	ENDP
_TEXT	ENDS
PUBLIC	_Sub
; Function compile flags: /Ogspy
;	COMDAT _Sub
_TEXT	SEGMENT
_x$ = 8							; size = 4
_Sub	PROC						; COMDAT
; Line 35
	ret	0
_Sub	ENDP
_TEXT	ENDS
PUBLIC	_WordRot
; Function compile flags: /Ogspy
;	COMDAT _WordRot
_TEXT	SEGMENT
_x$ = 8							; size = 4
_WordRot PROC						; COMDAT
; Line 47
	ret	0
_WordRot ENDP
_TEXT	ENDS
PUBLIC	_InvWordRot
; Function compile flags: /Ogspy
;	COMDAT _InvWordRot
_TEXT	SEGMENT
_x$ = 8							; size = 4
_InvWordRot PROC					; COMDAT
; Line 59
	ret	0
_InvWordRot ENDP
_TEXT	ENDS
PUBLIC	_BitRot
; Function compile flags: /Ogspy
;	COMDAT _BitRot
_TEXT	SEGMENT
_x$ = 8							; size = 4
_BitRot	PROC						; COMDAT
; Line 70
	ret	0
_BitRot	ENDP
_TEXT	ENDS
PUBLIC	_Add
; Function compile flags: /Ogspy
;	COMDAT _Add
_TEXT	SEGMENT
_x$ = 8							; size = 4
_y$ = 12						; size = 4
_Add	PROC						; COMDAT
; Line 77
	mov	eax, DWORD PTR _y$[esp-4]
	mov	ecx, DWORD PTR [eax]
	mov	eax, DWORD PTR _x$[esp-4]
	add	DWORD PTR [eax], ecx
; Line 80
	ret	0
_Add	ENDP
_TEXT	ENDS
PUBLIC	_fk
; Function compile flags: /Ogspy
;	COMDAT _fk
_TEXT	SEGMENT
_kr$ = 8						; size = 4
_kl$ = 12						; size = 4
_krDest$ = 16						; size = 4
_klDest$ = 20						; size = 4
_c$ = 24						; size = 4
_fk	PROC						; COMDAT
; Line 84
	push	ebp
	mov	ebp, esp
; Line 86
	mov	ecx, DWORD PTR _kr$[ebp]
	mov	edx, DWORD PTR [ecx]
	mov	eax, DWORD PTR _krDest$[ebp]
	mov	DWORD PTR [eax], edx
; Line 87
	mov	ecx, DWORD PTR [ecx]
	mov	edx, DWORD PTR _klDest$[ebp]
	mov	DWORD PTR [edx], ecx
; Line 89
	mov	ecx, DWORD PTR _c$[ebp]
	mov	ecx, DWORD PTR [ecx]
	add	DWORD PTR [eax], ecx
; Line 93
	mov	ecx, DWORD PTR _kl$[ebp]
	mov	ecx, DWORD PTR [ecx]
	xor	DWORD PTR [eax], ecx
; Line 95
	pop	ebp
	ret	0
_fk	ENDP
_TEXT	ENDS
PUBLIC	_fe
; Function compile flags: /Ogspy
;	COMDAT _fe
_TEXT	SEGMENT
_r$ = 8							; size = 4
_l$ = 12						; size = 4
_k$ = 16						; size = 4
_fe	PROC						; COMDAT
; Line 100
	mov	eax, DWORD PTR _r$[esp-4]
; Line 102
	mov	edx, DWORD PTR _k$[esp-4]
	mov	edx, DWORD PTR [edx]
	mov	ecx, DWORD PTR [eax]
	add	DWORD PTR [eax], edx
; Line 106
	mov	edx, DWORD PTR _l$[esp-4]
	push	esi
	mov	esi, DWORD PTR [edx]
	xor	DWORD PTR [eax], esi
; Line 108
	mov	DWORD PTR [edx], ecx
	pop	esi
; Line 111
	ret	0
_fe	ENDP
_TEXT	ENDS
PUBLIC	_fd
; Function compile flags: /Ogspy
;	COMDAT _fd
_TEXT	SEGMENT
_r$ = 8							; size = 4
_l$ = 12						; size = 4
_k$ = 16						; size = 4
_fd	PROC						; COMDAT
; Line 116
	mov	eax, DWORD PTR _r$[esp-4]
; Line 118
	mov	edx, DWORD PTR _k$[esp-4]
	mov	edx, DWORD PTR [edx]
	mov	ecx, DWORD PTR [eax]
	add	DWORD PTR [eax], edx
; Line 121
	mov	edx, DWORD PTR _l$[esp-4]
	push	esi
	mov	esi, DWORD PTR [edx]
	xor	DWORD PTR [eax], esi
; Line 123
	mov	DWORD PTR [edx], ecx
	pop	esi
; Line 126
	ret	0
_fd	ENDP
_TEXT	ENDS
PUBLIC	_KeySchedul
; Function compile flags: /Ogspy
;	COMDAT _KeySchedul
_TEXT	SEGMENT
tv1467 = -4						; size = 4
_mkey$ = 8						; size = 4
_rkey$ = 12						; size = 4
_KeySchedul PROC					; COMDAT
; Line 129
	push	ebp
	mov	ebp, esp
	push	ecx
; Line 132
	mov	eax, DWORD PTR _rkey$[ebp]
	mov	edx, DWORD PTR _mkey$[ebp]
	push	ebx
	push	esi
	push	edi
	push	2
	mov	ecx, eax
	sub	edx, eax
	pop	esi
$LL12@KeySchedul:
	mov	edi, DWORD PTR [edx+ecx]
	mov	DWORD PTR [ecx], edi
	add	ecx, 4
	dec	esi
	jne	SHORT $LL12@KeySchedul
; Line 133
	xor	edx, edx
	inc	edx
	lea	edi, DWORD PTR [eax+12]
$LL9@KeySchedul:
	lea	esi, DWORD PTR [eax+edx*8-8]
; Line 137
	mov	ebx, DWORD PTR [esi]
	lea	ecx, DWORD PTR [eax+edx*8]
	mov	DWORD PTR [ecx], ebx
	mov	esi, DWORD PTR [esi]
	mov	DWORD PTR [edi+8], esi
	add	DWORD PTR [ecx], edx
	mov	esi, DWORD PTR [edi]
	xor	DWORD PTR [ecx], esi
	inc	edx
	add	edi, 8
	cmp	edx, 12					; 0000000cH
	jbe	SHORT $LL9@KeySchedul
; Line 142
	mov	ecx, DWORD PTR [eax+96]
; Line 143
	mov	esi, DWORD PTR [eax+108]
	mov	DWORD PTR [eax+96], esi
; Line 144
	mov	DWORD PTR [eax+108], ecx
; Line 147
	cmp	edx, 51					; 00000033H
	jae	SHORT $LN1@KeySchedul
	lea	ecx, DWORD PTR [eax+edx*8+4]
	mov	DWORD PTR tv1467[ebp], ecx
$LL3@KeySchedul:
	lea	edi, DWORD PTR [eax+edx*8-8]
; Line 151
	mov	ebx, DWORD PTR [edi]
	lea	ecx, DWORD PTR [eax+edx*8]
	mov	DWORD PTR [ecx], ebx
	mov	ebx, DWORD PTR [edi]
	mov	edi, DWORD PTR tv1467[ebp]
	push	51					; 00000033H
	pop	esi
	sub	esi, edx
	mov	DWORD PTR [edi+8], ebx
	add	DWORD PTR [ecx], esi
	mov	esi, DWORD PTR [edi]
	xor	DWORD PTR [ecx], esi
	inc	edx
	add	edi, 8
	mov	DWORD PTR tv1467[ebp], edi
	cmp	edx, 51					; 00000033H
	jb	SHORT $LL3@KeySchedul
$LN1@KeySchedul:
	pop	edi
	pop	esi
	pop	ebx
; Line 154
	leave
	ret	0
_KeySchedul ENDP
_TEXT	ENDS
PUBLIC	_Decrypt
; Function compile flags: /Ogspy
;	COMDAT _Decrypt
_TEXT	SEGMENT
tv581 = -4						; size = 4
_state$ = 8						; size = 4
_rkey$ = 12						; size = 4
_Decrypt PROC						; COMDAT
; Line 157
	push	ebp
	mov	ebp, esp
	push	ecx
; Line 160
	mov	eax, DWORD PTR _state$[ebp]
	push	esi
	push	edi
	mov	edi, DWORD PTR _rkey$[ebp]
	push	51					; 00000033H
	pop	esi
	lea	ecx, DWORD PTR [edi+412]
	mov	DWORD PTR tv581[ebp], 38		; 00000026H
	push	ebx
$LL9@Decrypt:
; Line 162
	mov	ebx, DWORD PTR [ecx]
	mov	edx, DWORD PTR [eax]
	add	DWORD PTR [eax], ebx
	mov	ebx, DWORD PTR [eax+12]
	xor	DWORD PTR [eax], ebx
	dec	esi
	sub	ecx, 8
	dec	DWORD PTR tv581[ebp]
	mov	DWORD PTR [eax+12], edx
	jne	SHORT $LL9@Decrypt
; Line 164
	pop	ebx
	cmp	esi, 1
	jb	SHORT $LL4@Decrypt
	lea	ecx, DWORD PTR [edi+esi*8-8]
$LL6@Decrypt:
; Line 166
	mov	edi, DWORD PTR [ecx]
	mov	edx, DWORD PTR [eax]
	add	DWORD PTR [eax], edi
	mov	edi, DWORD PTR [eax+12]
	xor	DWORD PTR [eax], edi
	sub	ecx, 8
	dec	esi
	mov	DWORD PTR [eax+12], edx
	jne	SHORT $LL6@Decrypt
$LL4@Decrypt:
; Line 170
	mov	ecx, DWORD PTR [eax]
; Line 171
	mov	edx, DWORD PTR [eax+12]
	pop	edi
	mov	DWORD PTR [eax], edx
; Line 172
	mov	DWORD PTR [eax+12], ecx
	pop	esi
; Line 176
	leave
	ret	0
_Decrypt ENDP
_TEXT	ENDS
PUBLIC	_Encrypt
; Function compile flags: /Ogspy
;	COMDAT _Encrypt
_TEXT	SEGMENT
_state$ = 8						; size = 4
_rkey$ = 12						; size = 4
_Encrypt PROC						; COMDAT
; Line 181
	mov	eax, DWORD PTR _state$[esp-4]
	push	esi
	push	edi
	mov	edi, DWORD PTR _rkey$[esp+4]
	push	13					; 0000000dH
	pop	esi
	mov	edx, edi
	push	ebx
$LL9@Encrypt:
; Line 183
	mov	ebx, DWORD PTR [edx]
	mov	ecx, DWORD PTR [eax]
	add	DWORD PTR [eax], ebx
	mov	ebx, DWORD PTR [eax+12]
	xor	DWORD PTR [eax], ebx
	add	edx, 8
	dec	esi
	mov	DWORD PTR [eax+12], ecx
	jne	SHORT $LL9@Encrypt
; Line 185
	push	38					; 00000026H
	pop	esi
	lea	edx, DWORD PTR [edi+116]
	pop	ebx
$LL6@Encrypt:
; Line 187
	mov	edi, DWORD PTR [edx]
	mov	ecx, DWORD PTR [eax]
	add	DWORD PTR [eax], edi
	mov	edi, DWORD PTR [eax+12]
	xor	DWORD PTR [eax], edi
	add	edx, 8
	dec	esi
	mov	DWORD PTR [eax+12], ecx
	jne	SHORT $LL6@Encrypt
; Line 192
	mov	ecx, DWORD PTR [eax]
; Line 193
	mov	edx, DWORD PTR [eax+12]
	pop	edi
	mov	DWORD PTR [eax], edx
; Line 194
	mov	DWORD PTR [eax+12], ecx
	pop	esi
; Line 197
	ret	0
_Encrypt ENDP
_TEXT	ENDS
END
