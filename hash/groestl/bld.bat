@echo off
cl /Fa /O2 /Os /GS- /c groestl-ref.c
cl /Fa /O2 /Os /GS- /c hash.c
jwasm -bin groestl-ref.asm
jwasm -bin hash.asm