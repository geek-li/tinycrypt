@echo off
yasm -fbin -DBIN p1305.asm -op1305.bin
yasm -fwin32 p1305.asm -op1305.obj
cl /Os /O2 /Fa /c /GS- px.c
jwasm -bin px.asm
cl test.c p1305.obj px.c cc20.c 