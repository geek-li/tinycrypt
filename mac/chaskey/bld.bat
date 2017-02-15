@echo off
yasm -DBIN -fbin cx.asm -ocx.bin
yasm -fwin32 cx.asm -ocx.obj
cl /nologo /O2 /Os /GS- /c /Fa chaskey.c
jwasm -bin -nologo chaskey.asm
cl /nologo test.c chaskey.c cx.obj
del *.obj