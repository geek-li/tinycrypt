@echo off
yasm -DBIN -fbin bf.asm -obf.bin
yasm -fwin32 bf.asm -obf.obj
cl /DUSE_ASM test.c blowfish.c bf.obj
del *.obj