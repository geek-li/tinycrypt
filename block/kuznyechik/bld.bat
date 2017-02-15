@echo off
del *.obj *.bin *.exe
yasm -fbin -DBIN kz.asm -okz.bin
yasm -fwin32 kz.asm -okz.obj
cl /DUSE_ASM /nologo /O2 /Os /GS- test.c kz.obj
del *.obj
dir *.bin