@echo off
yasm -DBIN -fbin asm\sha256x.asm -osha256x.bin
yasm -fwin32 asm\sha256x.asm -osha256x.obj
cl test.c sha256.c
mv test.exe testc.exe
cl /DUSE_ASM test.c sha256x.obj