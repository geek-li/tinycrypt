@echo off
yasm -fbin -DBIN cmx.asm -ocmx.bin
yasm -fwin64 cmx.asm -ocmx.obj
cl test.c cmx.obj cubehash.c
del *.obj