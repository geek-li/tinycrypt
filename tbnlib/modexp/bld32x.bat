@echo off

yasm -fwin32 mx.asm -omx.obj

yasm -fbin -DBIN mx.asm -omx.bin

cl /nologo /O2 /Os /GS- mx_test.c mx.obj /IC:\openssl\inc32\ C:\openssl\out32\libeay32.lib user32.lib advapi32.lib gdi32.lib
cl /nologo mtest.c /I C:\openssl\inc32 C:\openssl\out32\libeay32.lib advapi32.lib user32.lib gdi32.lib mx.obj
del *.err *.obj