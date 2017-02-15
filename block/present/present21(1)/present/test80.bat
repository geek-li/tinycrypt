@echo off
cls
if exist %ciphtmp0.txt del ciphtmp0.txt
echo Initial Plaintext:
type plain.txt
echo.
echo Used Key:
type key80.txt
present.exe -e -f -v0 -k key80.txt -t plain.txt >ciphtmp0.txt
echo.
echo.
echo Resulting Ciphertext:
type ciphtmp0.txt
echo.
echo Resulting Plaintext:
present.exe -d -f -v0 -k key80.txt -t ciphtmp0.txt
echo.
echo.
del ciphtmp0.txt
pause
