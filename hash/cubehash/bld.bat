@echo off
yasm -DBIN -fbin asm\cx.asm -ocx.bin
yasm -fwin32 asm\cx.asm -ocx.obj
cl -DASM ct.c cx.obj cubehash.c
copy ct.exe ctx.exe
cl ct.c ch1.c cubehash.c