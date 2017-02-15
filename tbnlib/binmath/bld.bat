@echo off
cl /nologo /O2 binmath.c /c
cl /nologo /O2 dh\dh.c /I .\ binmath.obj
cl /nologo /O2 elgamal\elgamal.c /I .\ binmath.obj
cl /nologo /O2 jpake\jpake.c /I .\ binmath.obj
cl /nologo /O2 rsa\rsa.c /I .\ binmath.obj
cl /nologo /O2 speke\speke.c /I .\ binmath.obj
cl /nologo /O2 srp\srp.c /I .\ binmath.obj
cl /nologo /O2 ss\ss.c /I .\ binmath.obj
cl /nologo /O2 dsa\dsa.c /I .\ binmath.obj
cl /nologo /O2 utils\factor.c /I .\ binmath.obj
move dh.exe bin\
move elgamal.exe bin\
move jpake.exe bin\
move rsa.exe bin\
move speke.exe bin\
move srp.exe bin\
move ss.exe bin\
move dsa.exe bin\
move factor.exe bin\
del *.obj *.err