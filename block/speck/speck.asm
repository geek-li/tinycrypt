; mark_description "Intel(R) C++ Compiler XE for applications running on IA-32, Version 15.0.0.108 Build 20140726";
; mark_description "-O2 -Os -GS- -c -Fa";
	.686P
 	.387
	OPTION DOTNAME
	ASSUME	CS:FLAT,DS:FLAT,SS:FLAT
;ident "-defaultlib:libcpmt"
_TEXT	SEGMENT  DWORD PUBLIC FLAT  'CODE'
;	COMDAT _speck_setkey
TXTST0:
; -- Begin  _speck_setkey
;_speck_setkey	ENDS
_TEXT	ENDS
_TEXT	SEGMENT  DWORD PUBLIC FLAT  'CODE'
;	COMDAT _speck_setkey
; mark_begin;
IF @Version GE 800
  .MMX
ELSEIF @Version GE 612
  .MMX
  MMWORD TEXTEQU <QWORD>
ENDIF
IF @Version GE 800
  .XMM
ELSEIF @Version GE 614
  .XMM
  XMMWORD TEXTEQU <OWORD>
ENDIF

	PUBLIC _speck_setkey
_speck_setkey	PROC NEAR 
; parameter 1: 20 + esp
; parameter 2: 24 + esp
.B1.1:                          ; Preds .B1.0
        push      esi                                           ;33.1
        push      edi                                           ;33.1
        push      ebx                                           ;33.1
        push      ebp                                           ;33.1
        xor       esi, esi                                      ;43.8
        mov       ebp, DWORD PTR [20+esp]                       ;32.6
        mov       ebx, DWORD PTR [24+esp]                       ;32.6
        mov       ecx, DWORD PTR [ebp]                          ;37.20
        mov       edx, DWORD PTR [4+ebp]                        ;38.20
        mov       eax, DWORD PTR [8+ebp]                        ;39.20
        mov       edi, DWORD PTR [12+ebp]                       ;40.20
                                ; LOE eax edx ecx ebx esi edi
.B1.2:                          ; Preds .B1.2 .B1.1
        rol       edx, 24                                       ;47.11
        mov       ebp, esi                                      ;47.33
        add       edx, ecx                                      ;47.27
        mov       DWORD PTR [ebx+esi*4], ecx                    ;45.17
        xor       ebp, edx                                      ;47.33
        rol       ecx, 3                                        ;48.10
        inc       esi                                           ;43.29
        xor       ecx, ebp                                      ;48.26
        mov       edx, eax                                      ;51.5
        mov       eax, edi                                      ;51.5
        mov       edi, ebp                                      ;52.5
        cmp       esi, 26                                       ;43.15
        jb        .B1.2         ; Prob 96%                      ;43.15
                                ; LOE eax edx ecx ebx esi edi
.B1.3:                          ; Preds .B1.2
        pop       ebp                                           ;54.1
        pop       ebx                                           ;54.1
        pop       edi                                           ;54.1
        pop       esi                                           ;54.1
        ret                                                     ;54.1
                                ; LOE
; mark_end;
_speck_setkey ENDP
;_speck_setkey	ENDS
_TEXT	ENDS
_DATA	SEGMENT  DWORD PUBLIC FLAT  'DATA'
_DATA	ENDS
; -- End  _speck_setkey
_TEXT	SEGMENT  DWORD PUBLIC FLAT  'CODE'
;	COMDAT _speck_encrypt
TXTST1:
; -- Begin  _speck_encrypt
;_speck_encrypt	ENDS
_TEXT	ENDS
_TEXT	SEGMENT  DWORD PUBLIC FLAT  'CODE'
;	COMDAT _speck_encrypt
; mark_begin;

	PUBLIC _speck_encrypt
_speck_encrypt	PROC NEAR 
; parameter 1: 20 + esp
; parameter 2: 24 + esp
; parameter 3: 28 + esp
.B2.1:                          ; Preds .B2.0
        push      esi                                           ;57.1
        push      edi                                           ;57.1
        push      ebx                                           ;57.1
        push      ebp                                           ;57.1
        xor       ecx, ecx                                      ;65.8
        mov       ebx, DWORD PTR [28+esp]                       ;56.6
        mov       esi, DWORD PTR [20+esp]                       ;56.6
        mov       ebp, DWORD PTR [24+esp]                       ;56.6
        mov       eax, DWORD PTR [ebx]                          ;62.27
        mov       edx, DWORD PTR [4+ebx]                        ;63.27
                                ; LOE eax edx ecx ebx ebp esi
.B2.2:                          ; Preds .B2.5 .B2.1
        cmp       ebp, 1                                        ;67.14
        jne       .B2.4         ; Prob 84%                      ;67.14
                                ; LOE eax edx ecx ebx ebp esi
.B2.3:                          ; Preds .B2.2
        mov       edi, ecx                                      ;70.12
        xor       edx, eax                                      ;69.12
        neg       edi                                           ;70.12
        add       edi, 26                                       ;70.12
        and       edi, 255                                      ;70.12
        rol       edx, 29                                       ;69.12
        mov       edi, DWORD PTR [esi+edi*4]                    ;70.12
        xor       eax, edi                                      ;70.12
        sub       eax, edx                                      ;70.12
        mov       edi, eax                                      ;70.12
        shl       edi, 8                                        ;70.12
        shr       eax, 24                                       ;70.12
        or        eax, edi                                      ;70.12
        jmp       .B2.5         ; Prob 100%                     ;70.12
                                ; LOE eax edx ecx ebx ebp esi
.B2.4:                          ; Preds .B2.2
        rol       eax, 24                                       ;72.13
        add       eax, edx                                      ;72.29
        xor       eax, DWORD PTR [esi+ecx*4]                    ;72.35
        rol       edx, 3                                        ;73.12
        xor       edx, eax                                      ;73.28
                                ; LOE eax edx ecx ebx ebp esi
.B2.5:                          ; Preds .B2.3 .B2.4
        inc       ecx                                           ;65.27
        movzx     ecx, cl                                       ;65.27
        cmp       ecx, 27                                       ;65.15
        jl        .B2.2         ; Prob 82%                      ;65.15
                                ; LOE eax edx ecx ebx ebp esi
.B2.6:                          ; Preds .B2.5
        mov       DWORD PTR [ebx], eax                          ;77.15
        mov       DWORD PTR [4+ebx], edx                        ;78.15
                                ; LOE
.B2.9:                          ; Preds .B2.6
        pop       ebp                                           ;79.1
        pop       ebx                                           ;79.1
        pop       edi                                           ;79.1
        pop       esi                                           ;79.1
        ret                                                     ;79.1
                                ; LOE
; mark_end;
_speck_encrypt ENDP
;_speck_encrypt	ENDS
_TEXT	ENDS
_DATA	SEGMENT  DWORD PUBLIC FLAT  'DATA'
_DATA	ENDS
; -- End  _speck_encrypt
_DATA	SEGMENT  DWORD PUBLIC FLAT  'DATA'
_DATA	ENDS
	INCLUDELIB <libmmt>
	INCLUDELIB <LIBCMT>
	INCLUDELIB <libirc>
	INCLUDELIB <svml_dispmt>
	INCLUDELIB <OLDNAMES>
	INCLUDELIB <libdecimal>
	END
