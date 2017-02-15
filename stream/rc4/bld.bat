@echo off
yasm -DBIN -fbin rc4z.asm -orc4z.bin
yasm -fwin32 rc4z.asm -orc4z.obj
cl /nologo ztest.c rc4z.obj
cl /nologo test.c rc4.c
del *.obj *.err