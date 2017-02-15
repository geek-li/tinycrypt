@echo off
yasm -DBIN -fbin spk.asm -ospk.bin
yasm -fwin32 spk.asm -ospk.obj
cl /nologo /GS- /O2 /Os /Fa /c speck.c
jwasm -nologo -bin speck.asm
cl /nologo test.c speck.obj spk.obj
del *.obj