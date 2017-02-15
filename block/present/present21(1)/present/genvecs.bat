@echo off
echo Generating 80bit Test Vectors (Encryption)...
.\present -e -v2 -k 00000000000000000000 -t 0000000000000000 > .\testvecs\e80-k00_t00.txt
.\present -e -v2 -k 00000000000000000000 -t ffffffffffffffff > .\testvecs\e80-k00_tff.txt
.\present -e -v2 -k ffffffffffffffffffff -t 0000000000000000 > .\testvecs\e80-kff_t00.txt
.\present -e -v2 -k ffffffffffffffffffff -t ffffffffffffffff > .\testvecs\e80-kff_tff.txt

echo Generating 128bit Test Vectors (Encryption)...
.\present -e -v2 -k 00000000000000000000000000000000 -t 0000000000000000 > .\testvecs\e128-k00_t00.txt
.\present -e -v2 -k 00000000000000000000000000000000 -t ffffffffffffffff > .\testvecs\e128-k00_tff.txt
.\present -e -v2 -k ffffffffffffffffffffffffffffffff -t 0000000000000000 > .\testvecs\e128-kff_t00.txt
.\present -e -v2 -k ffffffffffffffffffffffffffffffff -t ffffffffffffffff > .\testvecs\e128-kff_tff.txt

echo Generating 80bit Test Vectors (Decryption)...
echo 00000000000000000000 >tmpkey.txt
.\present -e -v0 -k 00000000000000000000 -t 0000000000000000 > tmpenc.txt 
.\present -d -v2 -f -k tmpkey.txt -t tmpenc.txt > .\testvecs\d80-k00_t00.txt
.\present -e -v0 -k 00000000000000000000 -t ffffffffffffffff > tmpenc.txt
.\present -d -v2 -f -k tmpkey.txt -t tmpenc.txt > .\testvecs\d80-k00_tff.txt
echo ffffffffffffffffffff >tmpkey.txt
.\present -e -v0 -k ffffffffffffffffffff -t 0000000000000000 > tmpenc.txt
.\present -d -v2 -f -k tmpkey.txt -t tmpenc.txt > .\testvecs\d80-kff_t00.txt
.\present -e -v0 -k ffffffffffffffffffff -t ffffffffffffffff > tmpenc.txt
.\present -d -v2 -f -k tmpkey.txt -t tmpenc.txt > .\testvecs\d80-kff_tff.txt

echo Generating 128bit Test Vectors (Decryption)...
echo 00000000000000000000000000000000 >tmpkey.txt
.\present -e -v0 -k 00000000000000000000000000000000 -t 0000000000000000 > tmpenc.txt 
.\present -d -v2 -f -k tmpkey.txt -t tmpenc.txt > .\testvecs\d128-k00_t00.txt
.\present -e -v0 -k 00000000000000000000000000000000 -t ffffffffffffffff > tmpenc.txt
.\present -d -v2 -f -k tmpkey.txt -t tmpenc.txt > .\testvecs\d128-k00_tff.txt
echo ffffffffffffffffffffffffffffffff >tmpkey.txt
.\present -e -v0 -k ffffffffffffffffffffffffffffffff -t 0000000000000000 > tmpenc.txt
.\present -d -v2 -f -k tmpkey.txt -t tmpenc.txt > .\testvecs\d128-kff_t00.txt
.\present -e -v0 -k ffffffffffffffffffffffffffffffff -t ffffffffffffffff > tmpenc.txt
.\present -d -v2 -f -k tmpkey.txt -t tmpenc.txt > .\testvecs\d128-kff_tff.txt

del tmpkey.txt >NUL
del tmpenc.txt >NUL

echo Done!