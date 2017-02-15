@echo off
yasm -DBIN -fbin ccx.asm -occx.bin
yasm -fwin32 ccx.asm -occx.obj
cl /O2 /Os /DUSE_ASM test.c ccx.obj
del *.obj