@echo off
yasm -DBIN r6.asm -or6.bin
yasm -fwin32 r6.asm -or6.obj
cl /O2 /Os /DUSE_ASM test.c r6.obj
del *.obj