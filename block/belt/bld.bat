@echo off
del *.obj *.bin *.exe
yasm -fbin -DBIN bx.asm -obx.bin
yasm -fwin32 bx.asm -obx.obj
cl /nologo /DUSE_ASM /O2 /Os /GS- test.c bx.obj
del *.obj