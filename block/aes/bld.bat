@echo off
del *.obj *.bin *.exe
cl /nologo /Fa /c /O2 /Os /GS- aes.c
cl /nologo /Fa /c /O2 /Os /GS- aes_tbls.c
jwasm -bin aes.asm
jwasm -bin aes_tbls.asm
yasm -fbin -DBIN ax.asm -oax.bin
yasm -fwin32 ax.asm -oax.obj
cl /nologo /O2 /Os /GS- test.c aes.obj
copy test.exe testc.exe
cl /DUSE_ASM /nologo /O2 /Os /GS- test.c ax.obj
del *.obj
dir *.bin