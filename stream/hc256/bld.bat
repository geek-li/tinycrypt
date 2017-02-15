@echo off
yasm -DBIN -fbin asm\hx.asm -ohx.bin
yasm -fwin32 asm\hx.asm -ohx.obj
cl /O2 /Os test.c hc256.c hx.obj
del *.obj