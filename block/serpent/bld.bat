@echo off
yasm -fbin -DBIN sx.asm -o sx.bin
yasm -fwin32 sx.asm
cl /O2 /Os /GS- /DUSE_ASM test.c sx.obj