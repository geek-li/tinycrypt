@echo off
yasm -DBIN -fbin tx.asm -otx.bin
yasm -fwin32 tx.asm -otx.obj
cl /DINTRINSICS /nologo /O2 /Os /Fa /GS- /c threefish.c
jwasm -nologo -bin threefish.asm
cl /nologo test.c threefish.obj tx.obj
del *.obj
dir *.bin