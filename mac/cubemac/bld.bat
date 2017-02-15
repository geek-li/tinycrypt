@echo off
yasm -fbin -DBIN cm.asm -ocm.bin
yasm -fwin32 cm.asm -ocm.obj
cl test.c cm.obj cubehash.c
del *.obj