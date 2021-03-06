; Listing generated by Microsoft (R) Optimizing Compiler Version 16.00.40219.01 

	TITLE	C:\odzhan\tinycrypt\aead\acorn128v2\ref\encrypt.c
	.686P
	.XMM
	include listing.inc
	.model	flat

INCLUDELIB LIBCMT
INCLUDELIB OLDNAMES

PUBLIC	_KSG128
; Function compile flags: /Ogspy
;	COMDAT _KSG128
_TEXT	SEGMENT
_state$ = 8						; size = 4
_KSG128	PROC						; COMDAT
; File c:\odzhan\tinycrypt\aead\acorn128v2\ref\encrypt.c
; Line 10
	mov	ecx, DWORD PTR _state$[esp-4]
	movzx	edx, BYTE PTR [ecx+61]
	push	esi
	movzx	esi, BYTE PTR [ecx+193]
	push	edi
	movzx	edi, BYTE PTR [ecx+235]
	mov	eax, edx
	xor	eax, esi
	and	eax, edi
	and	edx, esi
	xor	eax, edx
	movzx	edx, BYTE PTR [ecx+154]
	movzx	ecx, BYTE PTR [ecx+12]
	xor	eax, edx
	pop	edi
	xor	eax, ecx
	pop	esi
; Line 11
	ret	0
_KSG128	ENDP
_TEXT	ENDS
PUBLIC	_FBK128
; Function compile flags: /Ogspy
;	COMDAT _FBK128
_TEXT	SEGMENT
_state$ = 8						; size = 4
tv230 = 11						; size = 1
_ks$ = 12						; size = 4
_ca$ = 16						; size = 1
_cb$ = 20						; size = 1
_FBK128	PROC						; COMDAT
; Line 14
	push	ebp
	mov	ebp, esp
	push	ebx
	push	esi
; Line 16
	mov	esi, DWORD PTR _state$[ebp]
	push	esi
	call	_KSG128
	pop	ecx
	mov	cl, al
	mov	eax, DWORD PTR _ks$[ebp]
	mov	BYTE PTR [eax], cl
; Line 17
	mov	bl, BYTE PTR [esi+230]
	mov	al, BYTE PTR [esi+23]
	xor	al, BYTE PTR [esi+160]
	and	cl, BYTE PTR _cb$[ebp]
	and	al, BYTE PTR [esi+244]
	mov	dl, bl
	xor	dl, 1
	and	dl, BYTE PTR [esi+66]
	xor	al, dl
	mov	dl, BYTE PTR [esi+111]
	and	dl, bl
	xor	al, dl
	mov	dl, BYTE PTR [esi+196]
	and	dl, BYTE PTR _ca$[ebp]
	xor	al, dl
	mov	dl, BYTE PTR [esi+23]
	and	dl, BYTE PTR [esi+160]
	xor	al, dl
	xor	al, cl
	xor	al, BYTE PTR [esi+107]
	xor	al, BYTE PTR [esi]
	pop	esi
	xor	al, 1
	pop	ebx
; Line 19
	pop	ebp
	ret	0
_FBK128	ENDP
_TEXT	ENDS
PUBLIC	_Encrypt_StateUpdate128_1bit
; Function compile flags: /Ogspy
;	COMDAT _Encrypt_StateUpdate128_1bit
_TEXT	SEGMENT
_state$ = 8						; size = 4
_plaintextbit$ = 12					; size = 1
_ciphertextbit$ = 16					; size = 4
_ks$ = 20						; size = 4
_ca$ = 24						; size = 1
_cb$ = 28						; size = 1
_Encrypt_StateUpdate128_1bit PROC			; COMDAT
; Line 23
	push	ebp
	mov	ebp, esp
	push	ebx
; Line 27
	mov	ebx, DWORD PTR _state$[ebp]
	mov	al, BYTE PTR [ebx+230]
	mov	cl, BYTE PTR [ebx+235]
; Line 28
	mov	dl, BYTE PTR [ebx+196]
	xor	cl, al
	xor	BYTE PTR [ebx+289], cl
	mov	cl, BYTE PTR [ebx+193]
	xor	dl, cl
	xor	dl, al
; Line 29
	mov	al, BYTE PTR [ebx+154]
	mov	BYTE PTR [ebx+230], dl
	mov	dl, BYTE PTR [ebx+160]
	xor	dl, al
	xor	dl, cl
; Line 30
	mov	cl, BYTE PTR [ebx+107]
	mov	BYTE PTR [ebx+193], dl
	mov	dl, BYTE PTR [ebx+111]
	xor	dl, cl
	xor	dl, al
; Line 31
	mov	al, BYTE PTR [ebx+61]
	push	esi
	mov	BYTE PTR [ebx+154], dl
	mov	dl, BYTE PTR [ebx+66]
	push	edi
; Line 34
	push	DWORD PTR _cb$[ebp]
	xor	dl, al
	push	DWORD PTR _ca$[ebp]
	xor	dl, cl
	mov	cl, BYTE PTR [ebx+23]
	xor	cl, BYTE PTR [ebx]
	push	DWORD PTR _ks$[ebp]
	xor	cl, al
	push	ebx
	mov	BYTE PTR [ebx+107], dl
	mov	BYTE PTR [ebx+61], cl
	call	_FBK128
; Line 37
	xor	al, BYTE PTR _plaintextbit$[ebp]
	lea	esi, DWORD PTR [ebx+1]
	mov	edi, ebx
	mov	ecx, 292				; 00000124H
	rep movsb
; Line 38
	mov	ecx, DWORD PTR _ciphertextbit$[ebp]
	mov	BYTE PTR [ebx+292], al
	mov	eax, DWORD PTR _ks$[ebp]
	mov	al, BYTE PTR [eax]
	add	esp, 16					; 00000010H
	xor	al, BYTE PTR _plaintextbit$[ebp]
	pop	edi
	pop	esi
	mov	BYTE PTR [ecx], al
	pop	ebx
; Line 39
	pop	ebp
	ret	0
_Encrypt_StateUpdate128_1bit ENDP
_TEXT	ENDS
PUBLIC	_Decrypt_StateUpdate128_1bit
; Function compile flags: /Ogspy
;	COMDAT _Decrypt_StateUpdate128_1bit
_TEXT	SEGMENT
_state$ = 8						; size = 4
_plaintextbit$ = 12					; size = 4
_ciphertextbit$ = 16					; size = 1
_ks$ = 20						; size = 4
_ca$ = 24						; size = 1
_cb$ = 28						; size = 1
_Decrypt_StateUpdate128_1bit PROC			; COMDAT
; Line 44
	push	ebp
	mov	ebp, esp
	push	ebx
; Line 48
	mov	ebx, DWORD PTR _state$[ebp]
	mov	al, BYTE PTR [ebx+230]
	mov	cl, BYTE PTR [ebx+235]
; Line 49
	mov	dl, BYTE PTR [ebx+196]
	xor	cl, al
	xor	BYTE PTR [ebx+289], cl
	mov	cl, BYTE PTR [ebx+193]
	xor	dl, cl
	xor	dl, al
; Line 50
	mov	al, BYTE PTR [ebx+154]
	mov	BYTE PTR [ebx+230], dl
	mov	dl, BYTE PTR [ebx+160]
	xor	dl, al
	xor	dl, cl
; Line 51
	mov	cl, BYTE PTR [ebx+107]
	mov	BYTE PTR [ebx+193], dl
	mov	dl, BYTE PTR [ebx+111]
	xor	dl, cl
	xor	dl, al
; Line 52
	mov	al, BYTE PTR [ebx+61]
	push	esi
	mov	BYTE PTR [ebx+154], dl
	mov	dl, BYTE PTR [ebx+66]
	push	edi
; Line 55
	push	DWORD PTR _cb$[ebp]
	xor	dl, al
	push	DWORD PTR _ca$[ebp]
	xor	dl, cl
	mov	cl, BYTE PTR [ebx+23]
	xor	cl, BYTE PTR [ebx]
	push	DWORD PTR _ks$[ebp]
	xor	cl, al
	push	ebx
	mov	BYTE PTR [ebx+107], dl
	mov	BYTE PTR [ebx+61], cl
	call	_FBK128
; Line 58
	mov	edx, DWORD PTR _plaintextbit$[ebp]
	lea	esi, DWORD PTR [ebx+1]
	mov	edi, ebx
	mov	ecx, 292				; 00000124H
	rep movsb
	mov	ecx, DWORD PTR _ks$[ebp]
	mov	cl, BYTE PTR [ecx]
	xor	cl, BYTE PTR _ciphertextbit$[ebp]
	add	esp, 16					; 00000010H
	mov	BYTE PTR [edx], cl
	pop	edi
; Line 59
	xor	cl, al
	pop	esi
	mov	BYTE PTR [ebx+292], cl
	pop	ebx
; Line 60
	pop	ebp
	ret	0
_Decrypt_StateUpdate128_1bit ENDP
_TEXT	ENDS
PUBLIC	_acorn128_enc_onebyte
; Function compile flags: /Ogspy
;	COMDAT _acorn128_enc_onebyte
_TEXT	SEGMENT
tv187 = -4						; size = 4
_state$ = 8						; size = 4
_plaintextbyte$ = 12					; size = 1
_ciphertextbyte$ = 16					; size = 4
_kstem$ = 19						; size = 1
_ksbyte$ = 20						; size = 4
_ciphertextbit$ = 23					; size = 1
_cabyte$ = 24						; size = 1
_cbbyte$ = 28						; size = 1
_acorn128_enc_onebyte PROC				; COMDAT
; Line 66
	push	ebp
	mov	ebp, esp
	push	ecx
	push	ebx
	push	esi
; Line 70
	mov	esi, DWORD PTR _ciphertextbyte$[ebp]
	xor	ebx, ebx
	push	edi
; Line 72
	mov	edi, DWORD PTR _ksbyte$[ebp]
	mov	BYTE PTR [esi], bl
	mov	BYTE PTR _kstem$[ebp], bl
	mov	BYTE PTR [edi], bl
	mov	DWORD PTR tv187[ebp], ebx
$LL3@acorn128_e:
; Line 76
	mov	al, BYTE PTR _cbbyte$[ebp]
	mov	cl, bl
	shr	al, cl
	and	al, 1
; Line 78
	movzx	eax, al
	push	eax
	mov	al, BYTE PTR _cabyte$[ebp]
	shr	al, cl
	and	al, 1
	movzx	eax, al
	push	eax
	lea	eax, DWORD PTR _kstem$[ebp]
	push	eax
	lea	eax, DWORD PTR _ciphertextbit$[ebp]
	push	eax
	mov	al, BYTE PTR _plaintextbyte$[ebp]
	shr	al, cl
	and	al, 1
	movzx	eax, al
	push	eax
	push	DWORD PTR _state$[ebp]
	call	_Encrypt_StateUpdate128_1bit
; Line 79
	mov	ecx, DWORD PTR tv187[ebp]
	mov	al, BYTE PTR _ciphertextbit$[ebp]
	shl	al, cl
	add	esp, 24					; 00000018H
	or	BYTE PTR [esi], al
; Line 80
	mov	al, BYTE PTR _kstem$[ebp]
	shl	al, cl
	or	BYTE PTR [edi], al
	inc	bl
	inc	DWORD PTR tv187[ebp]
	cmp	bl, 8
	jb	SHORT $LL3@acorn128_e
	pop	edi
	pop	esi
	pop	ebx
; Line 82
	leave
	ret	0
_acorn128_enc_onebyte ENDP
_TEXT	ENDS
PUBLIC	_acorn128_dec_onebyte
; Function compile flags: /Ogspy
;	COMDAT _acorn128_dec_onebyte
_TEXT	SEGMENT
_ks$ = -1						; size = 1
_state$ = 8						; size = 4
_plaintextbyte$ = 12					; size = 4
_plaintextbit$ = 15					; size = 1
_ciphertextbyte$ = 16					; size = 1
_ksbyte$ = 20						; size = 4
_cabyte$ = 24						; size = 1
_cbbyte$ = 28						; size = 1
_acorn128_dec_onebyte PROC				; COMDAT
; Line 88
	push	ebp
	mov	ebp, esp
	push	ecx
	push	ebx
	push	esi
; Line 92
	mov	esi, DWORD PTR _plaintextbyte$[ebp]
	xor	ebx, ebx
	push	edi
	mov	BYTE PTR [esi], bl
	xor	edi, edi
$LL3@acorn128_d:
; Line 96
	mov	al, BYTE PTR _cbbyte$[ebp]
	mov	cl, bl
	shr	al, cl
	and	al, 1
; Line 98
	movzx	eax, al
	push	eax
	mov	al, BYTE PTR _cabyte$[ebp]
	shr	al, cl
	and	al, 1
	movzx	eax, al
	push	eax
	lea	eax, DWORD PTR _ks$[ebp]
	push	eax
	mov	al, BYTE PTR _ciphertextbyte$[ebp]
	shr	al, cl
	and	al, 1
	movzx	eax, al
	push	eax
	lea	eax, DWORD PTR _plaintextbit$[ebp]
	push	eax
	push	DWORD PTR _state$[ebp]
	call	_Decrypt_StateUpdate128_1bit
; Line 99
	mov	al, BYTE PTR _plaintextbit$[ebp]
	mov	ecx, edi
	shl	al, cl
	add	esp, 24					; 00000018H
	or	BYTE PTR [esi], al
	inc	bl
	inc	edi
	cmp	bl, 8
	jb	SHORT $LL3@acorn128_d
	pop	edi
	pop	esi
	pop	ebx
; Line 101
	leave
	ret	0
_acorn128_dec_onebyte ENDP
_TEXT	ENDS
PUBLIC	_acorn128_initialization
; Function compile flags: /Ogspy
;	COMDAT _acorn128_initialization
_TEXT	SEGMENT
_m$ = -300						; size = 293
_ks$ = -2						; size = 1
_tem$ = -1						; size = 1
_key$ = 8						; size = 4
_iv$ = 12						; size = 4
_state$ = 16						; size = 4
_acorn128_initialization PROC				; COMDAT
; Line 106
	push	ebp
	mov	ebp, esp
	sub	esp, 300				; 0000012cH
	push	esi
	push	edi
; Line 111
	mov	edi, DWORD PTR _state$[ebp]
	xor	al, al
	mov	ecx, 292				; 00000124H
	rep stosb
; Line 115
	mov	ecx, DWORD PTR _key$[ebp]
	mov	esi, ecx
	lea	edi, DWORD PTR _m$[ebp]
	movsd
	movsd
	movsd
	movsd
; Line 116
	mov	esi, DWORD PTR _iv$[ebp]
	lea	edi, DWORD PTR _m$[ebp+16]
	movsd
	movsd
	movsd
; Line 117
	push	32					; 00000020H
	movsd
	pop	eax
$LL6@acorn128_i:
	mov	edx, eax
	and	edx, 15					; 0000000fH
	mov	dl, BYTE PTR [edx+ecx]
	mov	BYTE PTR _m$[ebp+eax], dl
	inc	eax
	cmp	eax, 223				; 000000dfH
	jle	SHORT $LL6@acorn128_i
; Line 118
	xor	BYTE PTR _m$[ebp+32], 1
; Line 121
	xor	edi, edi
	mov	esi, 255				; 000000ffH
$LL3@acorn128_i:
; Line 123
	push	esi
	push	esi
	lea	eax, DWORD PTR _ks$[ebp]
	push	eax
	lea	eax, DWORD PTR _tem$[ebp]
	push	eax
	movzx	eax, BYTE PTR _m$[ebp+edi]
	push	eax
	push	DWORD PTR _state$[ebp]
	call	_acorn128_enc_onebyte
	add	esp, 24					; 00000018H
	inc	edi
	cmp	edi, 224				; 000000e0H
	jl	SHORT $LL3@acorn128_i
	pop	edi
	pop	esi
; Line 125
	leave
	ret	0
_acorn128_initialization ENDP
_TEXT	ENDS
PUBLIC	_acorn128_tag_generation
; Function compile flags: /Ogspy
;	COMDAT _acorn128_tag_generation
_TEXT	SEGMENT
_ciphertextbyte$ = -2					; size = 1
_ksbyte$ = -1						; size = 1
_msglen$ = 8						; size = 8
_adlen$ = 16						; size = 8
_maclen$ = 24						; size = 1
_mac$ = 28						; size = 4
_state$ = 32						; size = 4
_acorn128_tag_generation PROC				; COMDAT
; Line 129
	push	ebp
	mov	ebp, esp
	push	ecx
	push	esi
	push	edi
; Line 132
	mov	BYTE PTR _ciphertextbyte$[ebp], 0
; Line 133
	mov	BYTE PTR _ksbyte$[ebp], 0
; Line 135
	xor	edi, edi
	mov	esi, 255				; 000000ffH
$LL4@acorn128_t:
; Line 137
	push	esi
	push	esi
	lea	eax, DWORD PTR _ksbyte$[ebp]
	push	eax
	lea	eax, DWORD PTR _ciphertextbyte$[ebp]
	push	eax
	push	0
	push	DWORD PTR _state$[ebp]
	call	_acorn128_enc_onebyte
	add	esp, 24					; 00000018H
; Line 138
	cmp	edi, 80					; 00000050H
	jl	SHORT $LN3@acorn128_t
	mov	al, BYTE PTR _ksbyte$[ebp]
	mov	ecx, DWORD PTR _mac$[ebp]
	mov	BYTE PTR [edi+ecx-80], al
$LN3@acorn128_t:
; Line 135
	inc	edi
	cmp	edi, 96					; 00000060H
	jl	SHORT $LL4@acorn128_t
	pop	edi
	pop	esi
; Line 140
	leave
	ret	0
_acorn128_tag_generation ENDP
_TEXT	ENDS
PUBLIC	_crypto_aead_encrypt
; Function compile flags: /Ogspy
;	COMDAT _crypto_aead_encrypt
_TEXT	SEGMENT
_state$ = -324						; size = 293
_mac$ = -28						; size = 16
_i$ = -20						; size = 8
_plaintextbyte$ = -10					; size = 1
_ca$ = -6						; size = 1
_ciphertextbyte$ = -2					; size = 1
_ksbyte$ = -1						; size = 1
_c$ = 8							; size = 4
_clen$ = 12						; size = 4
_m$ = 16						; size = 4
_mlen$ = 20						; size = 8
_ad$ = 28						; size = 4
_adlen$ = 32						; size = 8
_nsec$ = 40						; size = 4
_npub$ = 44						; size = 4
_k$ = 48						; size = 4
_crypto_aead_encrypt PROC				; COMDAT
; Line 152
	push	ebp
	mov	ebp, esp
	sub	esp, 324				; 00000144H
	push	ebx
	push	esi
	push	edi
; Line 159
	lea	eax, DWORD PTR _state$[ebp]
	push	eax
	push	DWORD PTR _npub$[ebp]
	push	DWORD PTR _k$[ebp]
	call	_acorn128_initialization
; Line 162
	xor	edi, edi
	add	esp, 12					; 0000000cH
	xor	esi, esi
	mov	ebx, 255				; 000000ffH
	cmp	DWORD PTR _adlen$[ebp+4], edi
	jb	SHORT $LN51@crypto_aea
	ja	SHORT $LL45@crypto_aea
	cmp	DWORD PTR _adlen$[ebp], edi
	jbe	SHORT $LN51@crypto_aea
$LL45@crypto_aea:
; Line 164
	push	ebx
	push	ebx
	lea	eax, DWORD PTR _ksbyte$[ebp]
	push	eax
	lea	eax, DWORD PTR _ciphertextbyte$[ebp]
	push	eax
	mov	eax, DWORD PTR _ad$[ebp]
	movzx	eax, BYTE PTR [esi+eax]
	push	eax
	lea	eax, DWORD PTR _state$[ebp]
	push	eax
	call	_acorn128_enc_onebyte
	add	esp, 24					; 00000018H
	add	esi, 1
	adc	edi, 0
	cmp	edi, DWORD PTR _adlen$[ebp+4]
	jb	SHORT $LL45@crypto_aea
; Line 162
	ja	SHORT $LN51@crypto_aea
	cmp	esi, DWORD PTR _adlen$[ebp]
	jb	SHORT $LL45@crypto_aea
$LN51@crypto_aea:
; Line 167
	xor	esi, esi
	xor	edi, edi
$LL52@crypto_aea:
; Line 169
	mov	eax, esi
	or	eax, edi
	jne	SHORT $LN14@crypto_aea
	mov	BYTE PTR _plaintextbyte$[ebp], 1
	jmp	SHORT $LN35@crypto_aea
$LN14@crypto_aea:
; Line 170
	mov	BYTE PTR _plaintextbyte$[ebp], 0
; Line 172
	test	edi, edi
	ja	SHORT $LN12@crypto_aea
	jb	SHORT $LN35@crypto_aea
	cmp	esi, 16					; 00000010H
	jae	SHORT $LN12@crypto_aea
$LN35@crypto_aea:
	mov	BYTE PTR _ca$[ebp], bl
	jmp	SHORT $LN11@crypto_aea
$LN12@crypto_aea:
; Line 173
	mov	BYTE PTR _ca$[ebp], 0
$LN11@crypto_aea:
; Line 177
	push	ebx
	push	DWORD PTR _ca$[ebp]
	lea	eax, DWORD PTR _ksbyte$[ebp]
	push	eax
	lea	eax, DWORD PTR _ciphertextbyte$[ebp]
	push	eax
	push	DWORD PTR _plaintextbyte$[ebp]
	lea	eax, DWORD PTR _state$[ebp]
	push	eax
	call	_acorn128_enc_onebyte
	add	esp, 24					; 00000018H
	add	esi, 1
	adc	edi, 0
	jne	SHORT $LN36@crypto_aea
; Line 167
	cmp	esi, 32					; 00000020H
	jb	SHORT $LL52@crypto_aea
$LN36@crypto_aea:
; Line 182
	mov	esi, DWORD PTR _mlen$[ebp+4]
	xor	edi, edi
	mov	DWORD PTR _i$[ebp+4], edi
	test	esi, esi
	jb	SHORT $LN47@crypto_aea
	ja	SHORT $LL43@crypto_aea
	cmp	DWORD PTR _mlen$[ebp], edi
	jbe	SHORT $LN47@crypto_aea
$LL43@crypto_aea:
; Line 184
	push	0
	push	ebx
	lea	eax, DWORD PTR _ksbyte$[ebp]
	push	eax
	mov	eax, DWORD PTR _c$[ebp]
	add	eax, edi
	push	eax
	mov	eax, DWORD PTR _m$[ebp]
	movzx	eax, BYTE PTR [edi+eax]
	push	eax
	lea	eax, DWORD PTR _state$[ebp]
	push	eax
	call	_acorn128_enc_onebyte
	add	esp, 24					; 00000018H
	add	edi, 1
	adc	DWORD PTR _i$[ebp+4], 0
	cmp	DWORD PTR _i$[ebp+4], esi
	jb	SHORT $LL43@crypto_aea
; Line 182
	ja	SHORT $LN47@crypto_aea
	cmp	edi, DWORD PTR _mlen$[ebp]
	jb	SHORT $LL43@crypto_aea
$LN47@crypto_aea:
; Line 187
	xor	edi, edi
	mov	DWORD PTR _i$[ebp+4], edi
$LL48@crypto_aea:
; Line 189
	mov	eax, edi
	or	eax, DWORD PTR _i$[ebp+4]
	jne	SHORT $LN4@crypto_aea
	mov	BYTE PTR _plaintextbyte$[ebp], 1
	jmp	SHORT $LN49@crypto_aea
$LN4@crypto_aea:
; Line 192
	cmp	DWORD PTR _i$[ebp+4], 0
	mov	BYTE PTR _plaintextbyte$[ebp], 0
	ja	SHORT $LN50@crypto_aea
	jb	SHORT $LN49@crypto_aea
	cmp	edi, 16					; 00000010H
	jae	SHORT $LN50@crypto_aea
$LN49@crypto_aea:
	mov	BYTE PTR _ca$[ebp], bl
	jmp	SHORT $LN1@crypto_aea
$LN50@crypto_aea:
; Line 193
	mov	BYTE PTR _ca$[ebp], 0
$LN1@crypto_aea:
; Line 197
	push	0
	push	DWORD PTR _ca$[ebp]
	lea	eax, DWORD PTR _ksbyte$[ebp]
	push	eax
	lea	eax, DWORD PTR _ciphertextbyte$[ebp]
	push	eax
	push	DWORD PTR _plaintextbyte$[ebp]
	lea	eax, DWORD PTR _state$[ebp]
	push	eax
	call	_acorn128_enc_onebyte
	add	esp, 24					; 00000018H
	add	edi, 1
	adc	DWORD PTR _i$[ebp+4], 0
	jne	SHORT $LN40@crypto_aea
; Line 187
	cmp	edi, 32					; 00000020H
	jb	SHORT $LL48@crypto_aea
$LN40@crypto_aea:
; Line 201
	mov	edi, DWORD PTR _mlen$[ebp]
	lea	eax, DWORD PTR _state$[ebp]
	push	eax
	lea	eax, DWORD PTR _mac$[ebp]
	push	eax
	push	16					; 00000010H
	push	DWORD PTR _adlen$[ebp+4]
	push	DWORD PTR _adlen$[ebp]
	push	esi
	push	edi
	call	_acorn128_tag_generation
; Line 203
	mov	eax, DWORD PTR _clen$[ebp]
	mov	ecx, edi
	add	esp, 28					; 0000001cH
	add	ecx, 16					; 00000010H
	adc	esi, 0
; Line 204
	add	edi, DWORD PTR _c$[ebp]
	mov	DWORD PTR [eax+4], esi
	mov	DWORD PTR [eax], ecx
	lea	esi, DWORD PTR _mac$[ebp]
	movsd
	movsd
	movsd
	movsd
	pop	edi
	pop	esi
; Line 206
	xor	eax, eax
	pop	ebx
; Line 207
	leave
	ret	0
_crypto_aead_encrypt ENDP
_TEXT	ENDS
PUBLIC	_crypto_aead_decrypt
; Function compile flags: /Ogspy
;	COMDAT _crypto_aead_decrypt
_TEXT	SEGMENT
_state$ = -324						; size = 293
_tag$ = -28						; size = 16
_i$ = -20						; size = 8
_plaintextbyte$ = -11					; size = 1
_ca$ = -7						; size = 1
_ciphertextbyte$ = -3					; size = 1
_ksbyte$ = -2						; size = 1
_check$ = -1						; size = 1
_m$ = 8							; size = 4
_mlen$ = 12						; size = 4
_nsec$ = 16						; size = 4
_c$ = 20						; size = 4
_clen$ = 24						; size = 8
_ad$ = 32						; size = 4
_adlen$ = 36						; size = 8
_npub$ = 44						; size = 4
_k$ = 48						; size = 4
_crypto_aead_decrypt PROC				; COMDAT
; Line 218
	push	ebp
	mov	ebp, esp
	sub	esp, 324				; 00000144H
	push	esi
	push	edi
; Line 226
	mov	edi, DWORD PTR _clen$[ebp+4]
	xor	esi, esi
	mov	BYTE PTR _check$[ebp], 0
	cmp	edi, esi
	ja	SHORT $LN56@crypto_aea@2
	jb	SHORT $LN41@crypto_aea@2
	cmp	DWORD PTR _clen$[ebp], 16		; 00000010H
	jae	SHORT $LN56@crypto_aea@2
$LN41@crypto_aea@2:
	or	eax, -1
	jmp	$LN2@crypto_aea@2
$LN56@crypto_aea@2:
	push	ebx
; Line 229
	lea	eax, DWORD PTR _state$[ebp]
	push	eax
	push	DWORD PTR _npub$[ebp]
	push	DWORD PTR _k$[ebp]
	call	_acorn128_initialization
	add	esp, 12					; 0000000cH
; Line 232
	mov	DWORD PTR _i$[ebp+4], esi
	mov	ebx, 255				; 000000ffH
	cmp	DWORD PTR _adlen$[ebp+4], esi
	jb	SHORT $LN63@crypto_aea@2
	ja	SHORT $LL57@crypto_aea@2
	cmp	DWORD PTR _adlen$[ebp], esi
	jbe	SHORT $LN63@crypto_aea@2
$LL57@crypto_aea@2:
; Line 234
	push	ebx
	push	ebx
	lea	eax, DWORD PTR _ksbyte$[ebp]
	push	eax
	lea	eax, DWORD PTR _ciphertextbyte$[ebp]
	push	eax
	mov	eax, DWORD PTR _ad$[ebp]
	movzx	eax, BYTE PTR [esi+eax]
	push	eax
	lea	eax, DWORD PTR _state$[ebp]
	push	eax
	call	_acorn128_enc_onebyte
	add	esp, 24					; 00000018H
	add	esi, 1
	adc	DWORD PTR _i$[ebp+4], 0
	mov	eax, DWORD PTR _i$[ebp+4]
	cmp	eax, DWORD PTR _adlen$[ebp+4]
	jb	SHORT $LL57@crypto_aea@2
; Line 232
	ja	SHORT $LN63@crypto_aea@2
	cmp	esi, DWORD PTR _adlen$[ebp]
	jb	SHORT $LL57@crypto_aea@2
$LN63@crypto_aea@2:
; Line 237
	xor	esi, esi
	mov	DWORD PTR _i$[ebp+4], esi
$LL64@crypto_aea@2:
; Line 239
	mov	eax, esi
	or	eax, DWORD PTR _i$[ebp+4]
	jne	SHORT $LN19@crypto_aea@2
	mov	BYTE PTR _plaintextbyte$[ebp], 1
	jmp	SHORT $LN44@crypto_aea@2
$LN19@crypto_aea@2:
; Line 242
	cmp	DWORD PTR _i$[ebp+4], 0
	mov	BYTE PTR _plaintextbyte$[ebp], 0
	ja	SHORT $LN17@crypto_aea@2
	jb	SHORT $LN44@crypto_aea@2
	cmp	esi, 16					; 00000010H
	jae	SHORT $LN17@crypto_aea@2
$LN44@crypto_aea@2:
	mov	BYTE PTR _ca$[ebp], bl
	jmp	SHORT $LN16@crypto_aea@2
$LN17@crypto_aea@2:
; Line 243
	mov	BYTE PTR _ca$[ebp], 0
$LN16@crypto_aea@2:
; Line 247
	push	ebx
	push	DWORD PTR _ca$[ebp]
	lea	eax, DWORD PTR _ksbyte$[ebp]
	push	eax
	lea	eax, DWORD PTR _ciphertextbyte$[ebp]
	push	eax
	push	DWORD PTR _plaintextbyte$[ebp]
	lea	eax, DWORD PTR _state$[ebp]
	push	eax
	call	_acorn128_enc_onebyte
	add	esp, 24					; 00000018H
	add	esi, 1
	adc	DWORD PTR _i$[ebp+4], 0
	jne	SHORT $LN45@crypto_aea@2
; Line 237
	cmp	esi, 32					; 00000020H
	jb	SHORT $LL64@crypto_aea@2
$LN45@crypto_aea@2:
; Line 251
	mov	eax, DWORD PTR _clen$[ebp]
	mov	esi, DWORD PTR _mlen$[ebp]
	add	eax, -16				; fffffff0H
	adc	edi, -1
	mov	DWORD PTR [esi], eax
	mov	DWORD PTR [esi+4], edi
; Line 253
	xor	eax, eax
	xor	edi, edi
	mov	DWORD PTR _i$[ebp+4], eax
	cmp	DWORD PTR [esi+4], eax
	jb	SHORT $LN59@crypto_aea@2
	ja	SHORT $LL53@crypto_aea@2
	cmp	DWORD PTR [esi], eax
	jbe	SHORT $LN59@crypto_aea@2
$LL53@crypto_aea@2:
; Line 255
	push	0
	push	ebx
	lea	eax, DWORD PTR _ksbyte$[ebp]
	push	eax
	mov	eax, DWORD PTR _c$[ebp]
	movzx	eax, BYTE PTR [edi+eax]
	push	eax
	mov	eax, DWORD PTR _m$[ebp]
	add	eax, edi
	push	eax
	lea	eax, DWORD PTR _state$[ebp]
	push	eax
	call	_acorn128_dec_onebyte
	add	esp, 24					; 00000018H
	add	edi, 1
	adc	DWORD PTR _i$[ebp+4], 0
	mov	eax, DWORD PTR _i$[ebp+4]
	cmp	eax, DWORD PTR [esi+4]
	jb	SHORT $LL53@crypto_aea@2
; Line 253
	ja	SHORT $LN59@crypto_aea@2
	cmp	edi, DWORD PTR [esi]
	jb	SHORT $LL53@crypto_aea@2
$LN59@crypto_aea@2:
; Line 258
	xor	edi, edi
	mov	DWORD PTR _i$[ebp+4], edi
$LL60@crypto_aea@2:
; Line 260
	mov	eax, edi
	or	eax, DWORD PTR _i$[ebp+4]
	jne	SHORT $LN9@crypto_aea@2
	mov	BYTE PTR _plaintextbyte$[ebp], 1
	jmp	SHORT $LN61@crypto_aea@2
$LN9@crypto_aea@2:
; Line 263
	cmp	DWORD PTR _i$[ebp+4], 0
	mov	BYTE PTR _plaintextbyte$[ebp], 0
	ja	SHORT $LN62@crypto_aea@2
	jb	SHORT $LN61@crypto_aea@2
	cmp	edi, 16					; 00000010H
	jae	SHORT $LN62@crypto_aea@2
$LN61@crypto_aea@2:
	mov	BYTE PTR _ca$[ebp], bl
	jmp	SHORT $LN6@crypto_aea@2
$LN62@crypto_aea@2:
; Line 264
	mov	BYTE PTR _ca$[ebp], 0
$LN6@crypto_aea@2:
; Line 268
	push	0
	push	DWORD PTR _ca$[ebp]
	lea	eax, DWORD PTR _ksbyte$[ebp]
	push	eax
	lea	eax, DWORD PTR _ciphertextbyte$[ebp]
	push	eax
	push	DWORD PTR _plaintextbyte$[ebp]
	lea	eax, DWORD PTR _state$[ebp]
	push	eax
	call	_acorn128_enc_onebyte
	add	esp, 24					; 00000018H
	add	edi, 1
	adc	DWORD PTR _i$[ebp+4], 0
	jne	SHORT $LN49@crypto_aea@2
; Line 258
	cmp	edi, 32					; 00000020H
	jb	SHORT $LL60@crypto_aea@2
$LN49@crypto_aea@2:
; Line 272
	lea	eax, DWORD PTR _state$[ebp]
	push	eax
	lea	eax, DWORD PTR _tag$[ebp]
	push	eax
	push	16					; 00000010H
	push	DWORD PTR _adlen$[ebp+4]
	push	DWORD PTR _adlen$[ebp]
	push	DWORD PTR [esi+4]
	push	DWORD PTR [esi]
	call	_acorn128_tag_generation
; Line 274
	mov	ecx, DWORD PTR _clen$[ebp]
	mov	edx, DWORD PTR _c$[ebp]
	add	esp, 28					; 0000001cH
	xor	eax, eax
	xor	edi, edi
	lea	esi, DWORD PTR [ecx+edx-16]
	pop	ebx
$LL5@crypto_aea@2:
	mov	cl, BYTE PTR [esi+eax]
	xor	cl, BYTE PTR _tag$[ebp+eax]
	or	BYTE PTR _check$[ebp], cl
	add	eax, 1
	adc	edi, 0
	jne	SHORT $LN50@crypto_aea@2
	cmp	eax, 16					; 00000010H
	jb	SHORT $LL5@crypto_aea@2
$LN50@crypto_aea@2:
; Line 275
	xor	eax, eax
	cmp	BYTE PTR _check$[ebp], al
	sete	al
	dec	eax
$LN2@crypto_aea@2:
	pop	edi
	pop	esi
; Line 277
	leave
	ret	0
_crypto_aead_decrypt ENDP
_TEXT	ENDS
END
