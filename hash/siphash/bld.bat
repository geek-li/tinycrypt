@echo off
yasm -DBIN -fbin sh.asm -osh.bin
yasm -fwin32 sh.asm -osh.obj
cl /O2 /Os test.c sh32.c sh.obj halfsiphash.c