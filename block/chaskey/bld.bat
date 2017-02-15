@echo off
cl /nologo /O2 /Os /GS- /c /Fa chaskey.c
jwasm -bin -nologo chaskey.asm
cl /nologo test.c chaskey.c