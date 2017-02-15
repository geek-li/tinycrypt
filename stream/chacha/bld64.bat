@echo off
yasm -fbin -DBIN ccxx.asm -occxx.bin
yasm -fwin64 ccxx.asm -occxx.obj
cl /DUSE_ASM test.c ccxx.obj
del *.obj