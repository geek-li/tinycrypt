@echo off
yasm -DBIN -fbin asm\s20x.asm -os20x.bin
yasm -fwin32 asm\s20x.asm -os20x.obj
cl /O2 /Os test.c s20.c s20x.obj
del *.obj