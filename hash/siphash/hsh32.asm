; mark_description "Intel(R) C++ Compiler XE for applications running on IA-32, Version 15.0.0.108 Build 20140726";
; mark_description "-Os -O2 -Fa -GS- -c";
	.686P
 	.387
	OPTION DOTNAME
	ASSUME	CS:FLAT,DS:FLAT,SS:FLAT
;ident "-defaultlib:libcpmt"
_TEXT	SEGMENT  DWORD PUBLIC FLAT  'CODE'
;	COMDAT _hsh32
TXTST0:
; -- Begin  _hsh32
;_hsh32	ENDS
_TEXT	ENDS
_TEXT	SEGMENT  DWORD PUBLIC FLAT  'CODE'
;	COMDAT _hsh32
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

	PUBLIC _hsh32
_hsh32	PROC NEAR 
; parameter 1: 52 + esp
; parameter 2: 56 + esp
; parameter 3: 60 + esp
.B1.1:                          ; Preds .B1.0
        push      esi                                           ;49.1
        push      edi                                           ;49.1
        push      ebx                                           ;49.1
        push      ebp                                           ;49.1
        sub       esp, 32                                       ;49.1
        mov       edx, DWORD PTR [60+esp]                       ;45.10
        mov       ebp, DWORD PTR [56+esp]                       ;45.10
        mov       ecx, ebp                                      ;59.27
        shl       ecx, 24                                       ;59.27
        mov       eax, DWORD PTR [edx]                          ;56.19
        mov       ebx, eax                                      ;67.5
        mov       edi, DWORD PTR [4+edx]                        ;57.19
        mov       edx, edi                                      ;66.5
        xor       edx, 1952801890                               ;66.5
        xor       ebx, 1819895653                               ;67.5
        mov       esi, DWORD PTR [52+esp]                       ;45.10
        mov       DWORD PTR [28+esp], ecx                       ;59.27
        mov       DWORD PTR [16+esp], edx                       ;66.5
        mov       DWORD PTR [12+esp], ebx                       ;67.5
        mov       DWORD PTR [8+esp], edi                        ;68.5
        mov       DWORD PTR [4+esp], eax                        ;69.5
        mov       BYTE PTR [20+esp], 0                          ;71.10
                                ; LOE ebp esi
.B1.2:                          ; Preds .B1.18 .B1.1
        push      4                                             ;74.7
        pop       eax                                           ;74.7
        cmp       ebp, 4                                        ;74.7
        cmovle    eax, ebp                                      ;74.7
        cmp       eax, 4                                        ;75.17
        lea       edx, DWORD PTR [-1+eax]                       ;84.16
        jge       .B1.5         ; Prob 50%                      ;75.17
                                ; LOE edx ebp esi
.B1.3:                          ; Preds .B1.2
        mov       ebx, DWORD PTR [28+esp]                       ;77.9
        test      edx, edx                                      ;84.23
        mov       BYTE PTR [20+esp], 1                          ;78.9
        jns       .B1.8         ; Prob 97%                      ;84.23
        jmp       .B1.15        ; Prob 100%                     ;84.23
                                ; LOE rbx edx ebx ebp esi bl bh
.B1.5:                          ; Preds .B1.2
        xor       ebx, ebx                                      ;81.9
        test      edx, edx                                      ;84.23
        js        .B1.13        ; Prob 2%                       ;84.23
                                ; LOE edx ebx ebp esi
.B1.6:                          ; Preds .B1.5
        mov       al, BYTE PTR [20+esp]                         ;87.13
        test      al, al                                        ;87.13
        je        .B1.11        ; Prob 50%                      ;87.13
                                ; LOE edx ebx ebp esi
.B1.8:                          ; Preds .B1.6 .B1.3 .B1.8
        movzx     eax, BYTE PTR [edx+esi]                       ;88.27
        lea       ecx, DWORD PTR [edx*8]                        ;88.44
        shl       eax, cl                                       ;88.44
        or        ebx, eax                                      ;88.11
        dec       edx                                           ;84.16
        jns       .B1.8         ; Prob 82%                      ;84.23
        jmp       .B1.15        ; Prob 100%                     ;84.23
                                ; LOE edx ebx ebp esi
.B1.11:                         ; Preds .B1.6 .B1.11
        movzx     eax, BYTE PTR [esi]                           ;90.28
        lea       ecx, DWORD PTR [edx*8]                        ;90.40
        shl       eax, cl                                       ;90.40
        inc       esi                                           ;91.11
        or        ebx, eax                                      ;90.11
        dec       ebp                                           ;92.11
        dec       edx                                           ;84.16
        jns       .B1.11        ; Prob 82%                      ;84.23
        jmp       .B1.14        ; Prob 100%                     ;84.23
                                ; LOE edx ebx ebp esi
.B1.13:                         ; Preds .B1.5
        mov       al, BYTE PTR [20+esp]                         ;96.12
        test      al, al                                        ;96.12
        jne       .B1.15        ; Prob 50%                      ;96.12
                                ; LOE ebx ebp esi
.B1.14:                         ; Preds .B1.11 .B1.13
        bswap     ebx                                           ;96.22
                                ; LOE ebx ebp esi
.B1.15:                         ; Preds .B1.8 .B1.3 .B1.14 .B1.13
        xor       DWORD PTR [16+esp], ebx                       ;100.7
        mov       BYTE PTR [24+esp], 0                          ;102.12
                                ; LOE ebx ebp esi
.B1.16:                         ; Preds .B1.17 .B1.15
        lea       eax, DWORD PTR [4+esp]                        ;103.9
        call      _XSIPROUND.                                   ;103.9
                                ; LOE ebx ebp esi
.B1.17:                         ; Preds .B1.16
        mov       al, BYTE PTR [24+esp]                         ;102.28
        inc       al                                            ;102.28
        mov       BYTE PTR [24+esp], al                         ;102.28
        cmp       al, 2                                         ;102.19
        jl        .B1.16        ; Prob 50%                      ;102.19
                                ; LOE ebx ebp esi
.B1.18:                         ; Preds .B1.17
        xor       DWORD PTR [4+esp], ebx                        ;106.7
        cmp       BYTE PTR [20+esp], 1                          ;71.23
        jne       .B1.2         ; Prob 82%                      ;71.23
                                ; LOE ebp esi
.B1.19:                         ; Preds .B1.18
        xor       DWORD PTR [12+esp], 255                       ;109.5
        xor       bl, bl                                        ;111.10
                                ; LOE bl
.B1.20:                         ; Preds .B1.21 .B1.19
        lea       eax, DWORD PTR [4+esp]                        ;112.9
        call      _XSIPROUND.                                   ;112.9
                                ; LOE bl
.B1.21:                         ; Preds .B1.20
        inc       bl                                            ;111.32
        cmp       bl, 4                                         ;111.21
        jl        .B1.20        ; Prob 75%                      ;111.21
                                ; LOE bl
.B1.22:                         ; Preds .B1.21
        mov       eax, DWORD PTR [8+esp]                        ;114.12
        xor       eax, DWORD PTR [16+esp]                       ;114.19
                                ; LOE
.B1.25:                         ; Preds .B1.22
        add       esp, 32                                       ;114.19
        pop       ebp                                           ;114.19
        pop       ebx                                           ;114.19
        pop       edi                                           ;114.19
        pop       esi                                           ;114.19
        ret                                                     ;114.19
                                ; LOE
; mark_end;
_hsh32 ENDP
;_hsh32	ENDS
_TEXT	ENDS
_DATA	SEGMENT  DWORD PUBLIC FLAT  'DATA'
_DATA	ENDS
; -- End  _hsh32
_TEXT	SEGMENT  DWORD PUBLIC FLAT  'CODE'
;	COMDAT _XSIPROUND
TXTST1:
; -- Begin  _XSIPROUND
;_XSIPROUND	ENDS
_TEXT	ENDS
_TEXT	SEGMENT  DWORD PUBLIC FLAT  'CODE'
;	COMDAT _XSIPROUND
; mark_begin;

	PUBLIC _XSIPROUND
_XSIPROUND	PROC NEAR 
; parameter 1: eax
.B2.1:                          ; Preds .B2.0
        mov       eax, DWORD PTR [4+esp]                        ;28.1
	PUBLIC _XSIPROUND.
_XSIPROUND.::
        push      esi                                           ;28.1
        push      edi                                           ;28.1
        mov       edi, eax                                      ;28.1
        mov       eax, DWORD PTR [4+edi]                        ;29.13
        mov       ecx, DWORD PTR [edi]                          ;29.5
        mov       edx, DWORD PTR [12+edi]                       ;33.13
        add       ecx, eax                                      ;29.5
        mov       esi, DWORD PTR [8+edi]                        ;33.5
        rol       eax, 5                                        ;30.12
        add       esi, edx                                      ;33.5
        rol       edx, 8                                        ;34.12
        xor       eax, ecx                                      ;31.5
        rol       ecx, 16                                       ;32.12
        xor       edx, esi                                      ;35.5
        add       ecx, edx                                      ;36.5
        add       esi, eax                                      ;39.5
        rol       edx, 7                                        ;37.12
        rol       eax, 13                                       ;40.12
        mov       DWORD PTR [edi], ecx                          ;36.5
        xor       ecx, edx                                      ;38.5
        xor       eax, esi                                      ;41.5
        rol       esi, 16                                       ;42.12
        mov       DWORD PTR [12+edi], ecx                       ;38.5
        mov       DWORD PTR [4+edi], eax                        ;41.5
        mov       DWORD PTR [8+edi], esi                        ;42.5
                                ; LOE
.B2.4:                          ; Preds .B2.1
        pop       edi                                           ;43.1
        pop       esi                                           ;43.1
        ret                                                     ;43.1
                                ; LOE
; mark_end;
_XSIPROUND ENDP
;_XSIPROUND	ENDS
_TEXT	ENDS
_DATA	SEGMENT  DWORD PUBLIC FLAT  'DATA'
_DATA	ENDS
; -- End  _XSIPROUND
_DATA	SEGMENT  DWORD PUBLIC FLAT  'DATA'
_DATA	ENDS
	INCLUDELIB <libmmt>
	INCLUDELIB <LIBCMT>
	INCLUDELIB <libirc>
	INCLUDELIB <svml_dispmt>
	INCLUDELIB <OLDNAMES>
	INCLUDELIB <libdecimal>
	END
