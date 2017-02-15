@echo off
yasm -DBIN -fbin nx.asm -onx.bin
yasm -fwin32 nx.asm -onx.obj
cl /nologo /O2 /Os /GS- /c /Fa noekeon.c
jwasm -nologo -bin noekeon.asm
cl /nologo test.c noekeon.c nx.obj
del *.obj