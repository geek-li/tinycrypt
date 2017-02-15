@echo off
yasm -fwin32 m32.asm -om32.obj
yasm -fbin -DBIN m32.asm -om32.bin
cl /nologo /O2 /Os /GS- modexp_test.c m32.obj /IC:\openssl\inc32\ C:\openssl\out32\libeay32.lib user32.lib advapi32.lib gdi32.lib
del *.err *.obj