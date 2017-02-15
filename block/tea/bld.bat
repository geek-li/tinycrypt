@echo off
yasm -DBIN -fbin tea_crypt.asm -otea_crypt.bin
yasm -DBIN -fbin tea_crypt1.asm -otea_crypt1.bin
yasm -DBIN -fbin tea_crypt2.asm -otea_crypt2.bin
yasm -DBIN -fbin tea_crypt3.asm -otea_crypt3.bin
yasm -fwin32 tea_crypt.asm -otea_crypt.obj
cl /nologo /DUSE_ASM test.c tea_crypt.obj
test
yasm -fwin32 tea_crypt3.asm -otea_crypt3.obj
cl /nologo /DUSE_ASM test.c tea_crypt3.obj
test