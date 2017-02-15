@echo off
del *.obj *.bin *.exe
yasm -fbin -DBIN rb.asm -orb.bin
yasm -fwin32 rb.asm -orb.obj
cl /nologo /DUSE_ASM /O2 /Os /GS- test.c rb.obj
del *.obj