; Listing generated by Microsoft (R) Optimizing Compiler Version 16.00.40219.01 

	TITLE	c:\odzhan\tinycrypt\block\sm4\sm4.c
	.686P
	.XMM
	include listing.inc
	.model	flat

INCLUDELIB LIBCMT
INCLUDELIB OLDNAMES

CONST	SEGMENT
_S	DB	0d6H
	DB	090H
	DB	0e9H
	DB	0feH
	DB	0ccH
	DB	0e1H
	DB	03dH
	DB	0b7H
	DB	016H
	DB	0b6H
	DB	014H
	DB	0c2H
	DB	028H
	DB	0fbH
	DB	02cH
	DB	05H
	DB	02bH
	DB	067H
	DB	09aH
	DB	076H
	DB	02aH
	DB	0beH
	DB	04H
	DB	0c3H
	DB	0aaH
	DB	044H
	DB	013H
	DB	026H
	DB	049H
	DB	086H
	DB	06H
	DB	099H
	DB	09cH
	DB	042H
	DB	050H
	DB	0f4H
	DB	091H
	DB	0efH
	DB	098H
	DB	07aH
	DB	033H
	DB	054H
	DB	0bH
	DB	043H
	DB	0edH
	DB	0cfH
	DB	0acH
	DB	062H
	DB	0e4H
	DB	0b3H
	DB	01cH
	DB	0a9H
	DB	0c9H
	DB	08H
	DB	0e8H
	DB	095H
	DB	080H
	DB	0dfH
	DB	094H
	DB	0faH
	DB	075H
	DB	08fH
	DB	03fH
	DB	0a6H
	DB	047H
	DB	07H
	DB	0a7H
	DB	0fcH
	DB	0f3H
	DB	073H
	DB	017H
	DB	0baH
	DB	083H
	DB	059H
	DB	03cH
	DB	019H
	DB	0e6H
	DB	085H
	DB	04fH
	DB	0a8H
	DB	068H
	DB	06bH
	DB	081H
	DB	0b2H
	DB	071H
	DB	064H
	DB	0daH
	DB	08bH
	DB	0f8H
	DB	0ebH
	DB	0fH
	DB	04bH
	DB	070H
	DB	056H
	DB	09dH
	DB	035H
	DB	01eH
	DB	024H
	DB	0eH
	DB	05eH
	DB	063H
	DB	058H
	DB	0d1H
	DB	0a2H
	DB	025H
	DB	022H
	DB	07cH
	DB	03bH
	DB	01H
	DB	021H
	DB	078H
	DB	087H
	DB	0d4H
	DB	00H
	DB	046H
	DB	057H
	DB	09fH
	DB	0d3H
	DB	027H
	DB	052H
	DB	04cH
	DB	036H
	DB	02H
	DB	0e7H
	DB	0a0H
	DB	0c4H
	DB	0c8H
	DB	09eH
	DB	0eaH
	DB	0bfH
	DB	08aH
	DB	0d2H
	DB	040H
	DB	0c7H
	DB	038H
	DB	0b5H
	DB	0a3H
	DB	0f7H
	DB	0f2H
	DB	0ceH
	DB	0f9H
	DB	061H
	DB	015H
	DB	0a1H
	DB	0e0H
	DB	0aeH
	DB	05dH
	DB	0a4H
	DB	09bH
	DB	034H
	DB	01aH
	DB	055H
	DB	0adH
	DB	093H
	DB	032H
	DB	030H
	DB	0f5H
	DB	08cH
	DB	0b1H
	DB	0e3H
	DB	01dH
	DB	0f6H
	DB	0e2H
	DB	02eH
	DB	082H
	DB	066H
	DB	0caH
	DB	060H
	DB	0c0H
	DB	029H
	DB	023H
	DB	0abH
	DB	0dH
	DB	053H
	DB	04eH
	DB	06fH
	DB	0d5H
	DB	0dbH
	DB	037H
	DB	045H
	DB	0deH
	DB	0fdH
	DB	08eH
	DB	02fH
	DB	03H
	DB	0ffH
	DB	06aH
	DB	072H
	DB	06dH
	DB	06cH
	DB	05bH
	DB	051H
	DB	08dH
	DB	01bH
	DB	0afH
	DB	092H
	DB	0bbH
	DB	0ddH
	DB	0bcH
	DB	07fH
	DB	011H
	DB	0d9H
	DB	05cH
	DB	041H
	DB	01fH
	DB	010H
	DB	05aH
	DB	0d8H
	DB	0aH
	DB	0c1H
	DB	031H
	DB	088H
	DB	0a5H
	DB	0cdH
	DB	07bH
	DB	0bdH
	DB	02dH
	DB	074H
	DB	0d0H
	DB	012H
	DB	0b8H
	DB	0e5H
	DB	0b4H
	DB	0b0H
	DB	089H
	DB	069H
	DB	097H
	DB	04aH
	DB	0cH
	DB	096H
	DB	077H
	DB	07eH
	DB	065H
	DB	0b9H
	DB	0f1H
	DB	09H
	DB	0c5H
	DB	06eH
	DB	0c6H
	DB	084H
	DB	018H
	DB	0f0H
	DB	07dH
	DB	0ecH
	DB	03aH
	DB	0dcH
	DB	04dH
	DB	020H
	DB	079H
	DB	0eeH
	DB	05fH
	DB	03eH
	DB	0d7H
	DB	0cbH
	DB	039H
	DB	048H
CONST	ENDS
PUBLIC	_T
; Function compile flags: /Ogspy
;	COMDAT _T
_TEXT	SEGMENT
_u$ = 8							; size = 4
_x$ = 8							; size = 4
_t$ = 12						; size = 4
_T	PROC						; COMDAT
; File c:\odzhan\tinycrypt\block\sm4\sm4.c
; Line 72
	push	ebp
	mov	ebp, esp
; Line 76
	mov	eax, DWORD PTR _x$[ebp]
	mov	DWORD PTR _u$[ebp], eax
; Line 78
	xor	ecx, ecx
$LL5@T:
; Line 79
	lea	eax, DWORD PTR _u$[ebp+ecx]
	movzx	edx, BYTE PTR [eax]
	mov	dl, BYTE PTR _S[edx]
	inc	ecx
	mov	BYTE PTR [eax], dl
	cmp	ecx, 4
	jl	SHORT $LL5@T
; Line 81
	cmp	DWORD PTR _t$[ebp], 0
; Line 85
	mov	ecx, DWORD PTR _u$[ebp]
	jne	SHORT $LN2@T
	mov	eax, ecx
	rol	eax, 13					; 0000000dH
	ror	ecx, 9
	xor	eax, ecx
	xor	eax, DWORD PTR _u$[ebp]
; Line 93
	pop	ebp
	ret	0
$LN2@T:
; Line 91
	mov	eax, ecx
	ror	eax, 14					; 0000000eH
	mov	edx, ecx
	rol	edx, 10					; 0000000aH
	xor	eax, edx
	mov	edx, ecx
	ror	edx, 8
	xor	eax, edx
	mov	edx, ecx
	rol	edx, 2
	xor	eax, edx
	xor	eax, ecx
; Line 93
	pop	ebp
	ret	0
_T	ENDP
_TEXT	ENDS
PUBLIC	_F
; Function compile flags: /Ogspy
;	COMDAT _F
_TEXT	SEGMENT
_x0$ = 8						; size = 4
_x1$ = 12						; size = 4
_x2$ = 16						; size = 4
_x3$ = 20						; size = 4
_rk$ = 24						; size = 4
_F	PROC						; COMDAT
; Line 100
	push	ebp
	mov	ebp, esp
; Line 101
	mov	eax, DWORD PTR _x1$[ebp]
	xor	eax, DWORD PTR _x2$[ebp]
	push	1
	xor	eax, DWORD PTR _x3$[ebp]
	xor	eax, DWORD PTR _rk$[ebp]
	push	eax
	call	_T
	xor	eax, DWORD PTR _x0$[ebp]
	pop	ecx
	pop	ecx
; Line 102
	pop	ebp
	ret	0
_F	ENDP
_TEXT	ENDS
PUBLIC	_CK
; Function compile flags: /Ogspy
;	COMDAT _CK
_TEXT	SEGMENT
_i$ = 8							; size = 4
_CK	PROC						; COMDAT
; Line 112
	mov	cl, BYTE PTR _i$[esp-4]
	push	ebx
	push	esi
	xor	esi, esi
	xor	edx, edx
	shl	cl, 2
$LL3@CK:
; Line 114
	lea	eax, DWORD PTR [edx+ecx]
	mov	bl, 7
	imul	bl
	movzx	eax, al
	shl	esi, 8
	or	esi, eax
	inc	edx
	cmp	edx, 4
	jl	SHORT $LL3@CK
; Line 116
	mov	eax, esi
	pop	esi
	pop	ebx
; Line 117
	ret	0
_CK	ENDP
_TEXT	ENDS
PUBLIC	_sm4_setkey
; Function compile flags: /Ogspy
;	COMDAT _sm4_setkey
_TEXT	SEGMENT
_c$ = 8							; size = 4
_k1$ = 12						; size = 4
_key$ = 12						; size = 4
_enc$ = 16						; size = 4
_sm4_setkey PROC					; COMDAT
; Line 120
	push	ebp
	mov	ebp, esp
	push	ebx
	push	esi
; Line 127
	mov	esi, DWORD PTR _key$[ebp]
; Line 128
	mov	ebx, DWORD PTR [esi+8]
	mov	eax, DWORD PTR [esi+4]
	push	edi
; Line 129
	mov	edi, DWORD PTR [esi+12]
	bswap	edi
	bswap	ebx
	xor	edi, -1301273892			; b27022dcH
	bswap	eax
	xor	eax, 1453994832				; 56aa3350H
	xor	ebx, 1736282519				; 677d9197H
; Line 131
	mov	ecx, edi
	xor	ecx, ebx
	xor	ecx, eax
	xor	ecx, 462357				; 00070e15H
	push	0
	push	ecx
	mov	DWORD PTR _k1$[ebp], eax
	call	_T
	mov	ecx, DWORD PTR [esi]
	mov	esi, DWORD PTR _c$[ebp]
	bswap	ecx
	xor	eax, ecx
	xor	eax, -1548633402			; a3b1bac6H
	mov	DWORD PTR [esi], eax
; Line 132
	xor	eax, edi
	xor	eax, ebx
	xor	eax, 472066609				; 1c232a31H
	push	0
	push	eax
	call	_T
	xor	eax, DWORD PTR _k1$[ebp]
; Line 133
	mov	ecx, DWORD PTR [esi]
	xor	ecx, eax
	xor	ecx, edi
	xor	ecx, 943670861				; 383f464dH
	push	0
	push	ecx
	mov	DWORD PTR [esi+4], eax
	call	_T
	xor	eax, ebx
	mov	DWORD PTR [esi+8], eax
; Line 134
	xor	eax, DWORD PTR [esi]
	lea	ebx, DWORD PTR [esi+4]
	xor	eax, DWORD PTR [ebx]
	push	0
	xor	eax, 1415275113				; 545b6269H
	push	eax
	call	_T
	add	esp, 32					; 00000020H
	xor	eax, edi
; Line 136
	push	4
	mov	DWORD PTR [esi+12], eax
	pop	edi
$LL7@sm4_setkey:
; Line 137
	push	0
	push	edi
	call	_CK
	xor	eax, DWORD PTR [ebx+8]
	pop	ecx
	xor	eax, DWORD PTR [ebx+4]
	xor	eax, DWORD PTR [ebx]
	push	eax
	call	_T
	xor	eax, DWORD PTR [ebx-4]
	inc	edi
	mov	DWORD PTR [ebx+12], eax
	pop	ecx
	add	ebx, 4
	pop	ecx
	cmp	edi, 32					; 00000020H
	jl	SHORT $LL7@sm4_setkey
; Line 140
	cmp	DWORD PTR _enc$[ebp], 1
	jne	SHORT $LN1@sm4_setkey
; Line 141
	xor	edx, edx
	lea	eax, DWORD PTR [esi+124]
$LL3@sm4_setkey:
; Line 142
	mov	edi, DWORD PTR [eax]
	mov	ecx, DWORD PTR [esi+edx*4]
	mov	DWORD PTR [esi+edx*4], edi
	mov	DWORD PTR [eax], ecx
	inc	edx
	sub	eax, 4
	cmp	edx, 16					; 00000010H
	jl	SHORT $LL3@sm4_setkey
$LN1@sm4_setkey:
	pop	edi
	pop	esi
	pop	ebx
; Line 145
	pop	ebp
	ret	0
_sm4_setkey ENDP
_TEXT	ENDS
PUBLIC	_sm4_encrypt
; Function compile flags: /Ogspy
;	COMDAT _sm4_encrypt
_TEXT	SEGMENT
_x0$ = -8						; size = 4
_x3$ = -4						; size = 4
_c$ = 8							; size = 4
_i$ = 12						; size = 4
_buf$ = 12						; size = 4
_sm4_encrypt PROC					; COMDAT
; Line 148
	push	ebp
	mov	ebp, esp
	push	ecx
	push	ecx
	push	ebx
	push	esi
; Line 153
	mov	esi, DWORD PTR _buf$[ebp]
	mov	eax, DWORD PTR [esi]
; Line 155
	mov	ebx, DWORD PTR [esi+8]
; Line 158
	and	DWORD PTR _i$[ebp], 0
	bswap	eax
	mov	DWORD PTR _x0$[ebp], eax
	mov	eax, DWORD PTR [esi+12]
	push	edi
	mov	edi, DWORD PTR [esi+4]
	bswap	eax
	bswap	edi
	bswap	ebx
	mov	DWORD PTR _x3$[ebp], eax
	jmp	SHORT $LN3@sm4_encryp
$LL10@sm4_encryp:
	mov	eax, DWORD PTR _x3$[ebp]
$LN3@sm4_encryp:
; Line 159
	mov	edx, DWORD PTR _i$[ebp]
	mov	ecx, DWORD PTR _c$[ebp]
	mov	ecx, DWORD PTR [ecx+edx*4]
	xor	ecx, eax
	xor	ecx, ebx
	xor	ecx, edi
	push	1
	push	ecx
	call	_T
	xor	eax, DWORD PTR _x0$[ebp]
	inc	DWORD PTR _i$[ebp]
	cmp	DWORD PTR _i$[ebp], 32			; 00000020H
; Line 160
	mov	DWORD PTR _x0$[ebp], edi
	pop	ecx
; Line 161
	mov	edi, ebx
; Line 162
	mov	ebx, DWORD PTR _x3$[ebp]
	pop	ecx
	mov	DWORD PTR _x3$[ebp], eax
	jb	SHORT $LL10@sm4_encryp
; Line 165
	bswap	eax
	mov	DWORD PTR [esi], eax
; Line 168
	mov	eax, DWORD PTR _x0$[ebp]
	bswap	edi
	bswap	ebx
	mov	DWORD PTR [esi+8], edi
	bswap	eax
	pop	edi
	mov	DWORD PTR [esi+4], ebx
	mov	DWORD PTR [esi+12], eax
	pop	esi
	pop	ebx
; Line 169
	leave
	ret	0
_sm4_encrypt ENDP
_TEXT	ENDS
END