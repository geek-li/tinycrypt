@echo off
cl /O2 /Ot tbntest.c tbnlib.c /I C:\openssl\include C:\openssl\out32\libeay32.lib gdi32.lib user32.lib advapi32.lib
del *.obj