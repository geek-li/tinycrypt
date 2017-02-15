@echo off
yasm -DBIN -fbin sha3x.asm -osha3x.bin
yasm -fwin32 sha3x.asm
cl /O2 /Os /GS- /DUSE_ASM test.c sha3x.obj
del *.obj