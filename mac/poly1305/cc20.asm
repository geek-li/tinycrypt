; Listing generated by Microsoft (R) Optimizing Compiler Version 16.00.40219.01 

	TITLE	C:\odzhan\tinycrypt\aead\poly1305\cc20.c
	.686P
	.XMM
	include listing.inc
	.model	flat

INCLUDELIB LIBCMT
INCLUDELIB OLDNAMES

PUBLIC	_cc20_init
; Function compile flags: /Ogspy
;	COMDAT _cc20_init
_TEXT	SEGMENT
_s$ = 8							; size = 4
_key$ = 12						; size = 4
_counter$ = 16						; size = 4
_nonce$ = 20						; size = 4
_cc20_init PROC						; COMDAT
; File c:\odzhan\tinycrypt\aead\poly1305\cc20.c
; Line 44
	mov	eax, DWORD PTR _s$[esp-4]
	push	esi
; Line 50
	mov	esi, DWORD PTR _key$[esp]
	push	edi
	push	8
	mov	DWORD PTR [eax], 1634760805		; 61707865H
	mov	DWORD PTR [eax+4], 857760878		; 3320646eH
	mov	DWORD PTR [eax+8], 2036477234		; 79622d32H
	mov	DWORD PTR [eax+12], 1797285236		; 6b206574H
	pop	ecx
	lea	edi, DWORD PTR [eax+16]
	rep movsd
; Line 53
	mov	ecx, DWORD PTR _counter$[esp+4]
	mov	DWORD PTR [eax+48], ecx
; Line 54
	mov	ecx, DWORD PTR _nonce$[esp+4]
	mov	edx, DWORD PTR [ecx]
	mov	DWORD PTR [eax+52], edx
; Line 55
	mov	edx, DWORD PTR [ecx+4]
	mov	DWORD PTR [eax+56], edx
; Line 56
	mov	ecx, DWORD PTR [ecx+8]
	pop	edi
	mov	DWORD PTR [eax+60], ecx
	pop	esi
; Line 62
	ret	0
_cc20_init ENDP
_TEXT	ENDS
PUBLIC	_cc20_permute
; Function compile flags: /Ogspy
;	COMDAT _cc20_permute
_TEXT	SEGMENT
_idx16$ = -28						; size = 16
_c$ = -12						; size = 4
tv301 = -8						; size = 4
_j$ = -4						; size = 4
_state$ = 8						; size = 4
_cc20_permute PROC					; COMDAT
; Line 70
	push	ebp
	mov	ebp, esp
	sub	esp, 28					; 0000001cH
; Line 76
	mov	eax, 51264				; 0000c840H
	mov	WORD PTR _idx16$[ebp], ax
	mov	eax, 55633				; 0000d951H
	mov	WORD PTR _idx16$[ebp+2], ax
	mov	eax, 60002				; 0000ea62H
	mov	WORD PTR _idx16$[ebp+4], ax
	mov	eax, 64371				; 0000fb73H
	mov	WORD PTR _idx16$[ebp+6], ax
; Line 77
	mov	eax, 64080				; 0000fa50H
	mov	WORD PTR _idx16$[ebp+8], ax
	mov	eax, 52065				; 0000cb61H
	mov	WORD PTR _idx16$[ebp+10], ax
	mov	eax, 55410				; 0000d872H
	push	ebx
	mov	WORD PTR _idx16$[ebp+12], ax
	push	esi
	mov	eax, 59715				; 0000e943H
	push	edi
	mov	WORD PTR _idx16$[ebp+14], ax
	mov	DWORD PTR tv301[ebp], 10		; 0000000aH
$LL13@cc20_permu:
; Line 82
	and	DWORD PTR _j$[ebp], 0
$LL14@cc20_permu:
; Line 84
	mov	eax, DWORD PTR _j$[ebp]
	movzx	eax, WORD PTR _idx16$[ebp+eax*2]
	mov	ecx, DWORD PTR _state$[ebp]
; Line 88
	mov	edi, eax
	mov	edx, eax
	mov	esi, eax
; Line 89
	shr	eax, 12					; 0000000cH
	shr	edi, 8
	shr	edx, 4
	push	15					; 0000000fH
	pop	ebx
	and	esi, ebx
	and	eax, ebx
	and	edi, ebx
	and	edx, ebx
; Line 91
	lea	edx, DWORD PTR [ecx+edx*4]
	mov	DWORD PTR _c$[ebp], edi
; Line 94
	mov	ebx, DWORD PTR _c$[ebp]
	lea	edi, DWORD PTR [ecx+eax*4]
	lea	eax, DWORD PTR [ecx+esi*4]
	mov	esi, DWORD PTR [edx]
	add	DWORD PTR [eax], esi
	mov	esi, DWORD PTR [edi]
	xor	esi, DWORD PTR [eax]
	lea	ecx, DWORD PTR [ecx+ebx*4]
	rol	esi, 16					; 00000010H
	mov	DWORD PTR [edi], esi
	add	DWORD PTR [ecx], esi
; Line 95
	mov	esi, DWORD PTR [edx]
	xor	esi, DWORD PTR [ecx]
	rol	esi, 12					; 0000000cH
	mov	DWORD PTR [edx], esi
; Line 97
	add	DWORD PTR [eax], esi
	mov	eax, DWORD PTR [eax]
; Line 98
	mov	esi, DWORD PTR [edi]
	xor	esi, eax
	rol	esi, 8
	mov	DWORD PTR [edi], esi
; Line 100
	add	DWORD PTR [ecx], esi
	mov	ecx, DWORD PTR [ecx]
; Line 101
	mov	eax, DWORD PTR [edx]
	xor	eax, ecx
	inc	DWORD PTR _j$[ebp]
	rol	eax, 7
	cmp	DWORD PTR _j$[ebp], 8
	mov	DWORD PTR [edx], eax
	jb	SHORT $LL14@cc20_permu
; Line 80
	dec	DWORD PTR tv301[ebp]
	jne	SHORT $LL13@cc20_permu
	pop	edi
	pop	esi
	pop	ebx
; Line 104
	leave
	ret	0
_cc20_permute ENDP
_TEXT	ENDS
PUBLIC	_cc20_stream
; Function compile flags: /Ogspy
;	COMDAT _cc20_stream
_TEXT	SEGMENT
_ws$ = -64						; size = 64
_s$ = 8							; size = 4
_key$ = 12						; size = 4
_counter$ = 16						; size = 4
_nonce$ = 20						; size = 4
_cc20_stream PROC					; COMDAT
; Line 116
	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
; Line 120
	mov	ebx, DWORD PTR _s$[ebp]
	push	esi
	push	edi
	push	DWORD PTR _nonce$[ebp]
	push	DWORD PTR _counter$[ebp]
	push	DWORD PTR _key$[ebp]
	push	ebx
	call	_cc20_init
; Line 123
	push	16					; 00000010H
	pop	ecx
; Line 126
	lea	eax, DWORD PTR _ws$[ebp]
	mov	esi, ebx
	lea	edi, DWORD PTR _ws$[ebp]
	push	eax
	rep movsd
	call	_cc20_permute
	add	esp, 20					; 00000014H
; Line 129
	lea	ecx, DWORD PTR _ws$[ebp]
	push	16					; 00000010H
	mov	eax, ebx
	sub	ecx, ebx
	pop	edx
$LL3@cc20_strea:
; Line 130
	mov	esi, DWORD PTR [ecx+eax]
	add	DWORD PTR [eax], esi
	add	eax, 4
	dec	edx
	jne	SHORT $LL3@cc20_strea
	pop	edi
	pop	esi
	pop	ebx
; Line 132
	leave
	ret	0
_cc20_stream ENDP
_TEXT	ENDS
PUBLIC	_cc20_encrypt
; Function compile flags: /Ogspy
;	COMDAT _cc20_encrypt
_TEXT	SEGMENT
_s$ = -64						; size = 64
_key$ = 8						; size = 4
_counter$ = 12						; size = 4
_nonce$ = 16						; size = 4
_ptr$ = 20						; size = 4
_data$ = 20						; size = 4
tv191 = 24						; size = 4
_len$ = 24						; size = 4
_cc20_encrypt PROC					; COMDAT
; Line 141
	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
; Line 144
	mov	eax, DWORD PTR _data$[ebp]
	push	edi
; Line 146
	mov	edi, DWORD PTR _len$[ebp]
	mov	DWORD PTR _ptr$[ebp], eax
	test	edi, edi
	je	SHORT $LN4@cc20_encry
; Line 144
	mov	eax, DWORD PTR _counter$[ebp]
	push	ebx
	mov	DWORD PTR tv191[ebp], eax
	push	esi
$LL16@cc20_encry:
; Line 148
	push	DWORD PTR _nonce$[ebp]
	lea	eax, DWORD PTR _s$[ebp]
	push	DWORD PTR tv191[ebp]
	push	DWORD PTR _key$[ebp]
	push	eax
	call	_cc20_stream
	add	esp, 16					; 00000010H
; Line 149
	push	64					; 00000040H
	pop	esi
	cmp	edi, esi
	ja	SHORT $LN15@cc20_encry
	mov	esi, edi
; Line 151
	test	edi, edi
	je	SHORT $LN1@cc20_encry
$LN15@cc20_encry:
	mov	eax, DWORD PTR _ptr$[ebp]
	lea	ecx, DWORD PTR _s$[ebp]
	sub	ecx, eax
	mov	edx, esi
$LL3@cc20_encry:
; Line 152
	mov	bl, BYTE PTR [ecx+eax]
	xor	BYTE PTR [eax], bl
	inc	eax
	dec	edx
	jne	SHORT $LL3@cc20_encry
$LN1@cc20_encry:
; Line 155
	add	DWORD PTR _ptr$[ebp], esi
	inc	DWORD PTR tv191[ebp]
	sub	edi, esi
	jne	SHORT $LL16@cc20_encry
	pop	esi
	pop	ebx
$LN4@cc20_encry:
	pop	edi
; Line 157
	leave
	ret	0
_cc20_encrypt ENDP
_TEXT	ENDS
END
