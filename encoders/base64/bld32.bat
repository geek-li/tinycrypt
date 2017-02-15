@echo off
ml /coff /Cp /c /nologo src\x86\b64encode.asm
ml /coff /Cp /c /nologo src\x86\b64decode.asm
lib /out:lib\x86\b64.lib b64encode.obj b64decode.obj
cl /O1 b64e.c lib\x86\b64.lib
cl /O1 b64d.c lib\x86\b64.lib
move b64d.exe bin\x86\
move b64e.exe bin\x86\
del *.obj *.err