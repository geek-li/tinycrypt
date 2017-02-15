@echo off
cl /Fa /O2 /Os /GS- /c jh.c
cl /Fa /O2 /Os /GS- /c jh_opt8.c
cl /Fa /O2 /Os /GS- /c ref.c
jwasm -bin jh.asm
jwasm -bin jh_opt8.asm
jwasm -bin ref.asm
del *.obj