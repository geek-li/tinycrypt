---------------------------------------
PRESENT Commandline Tool v2.1
Thomas Siebert, thomas.siebert@rub.de
---------------------------------------

I. INTRODUCTION
-----------------
With this tool, you are able to encrypt and decrypt single blocks with
the PRESENT-Cipher.



II. SYNTAX
-----------------
PRESENT -d|e [-f] [-r rounds] [-v level] -k key -t text

Choose -d to decrypt, or -e to encrypt one block

-f (optional): File input, see below
-r rounds (optional): Change number of rounds (up to 65534, standard is 32)
-v level (optional): Specify verbose level:
   0 for result-output only
   1 for output of mode, input, result (standard)
   2 for roundwise output

-k key: Key in hexadecimal (length: *EXACTLY* 20 chars(80bit)/32 chars(128bit))
-t text: Text in hexadecimal (length: *EXACTLY* 16 chars)
If -f is set, key and text represent files containing the values,
otherwise they must be passed directly via commandline.

Returned Errorlevel: 0 if successful, 1 if non-successful



III. FILES
-----------------
(a) Code
present.c - main code
boxes.inc - S-Box/P-Box definitions
comline.inc - command line interpreter
verbose.inc - verbose output


(b) Integrity Test
key80.txt - test key file (80bit)
key128.txt - test key file (128bit)
plain.txt - test plaintext file
--Plaintext and keys for usage within the scripts below.

test128.bat - test-script 128bit (windows)
test128.sh - test-script 128bit (linux)
test80.bat - test-script 80bit (windows)
test80.sh - test-script 80bit (linux)
--You can perform an integrity test of the code with these scripts. They encrypt
the plaintext in plain.txt with the corresponding key*.txt and decrypt the ciphertext
immediately. Then you can compare if the result is equal to the initial plaintext.


(c) Test Vector Generation
genvecs.bat - test vector generator script (windows)
genvecs.sh - test vector generator script (linux) 
--These scripts generate test vectors (80 and 128 bits,
encryption and decryption) by using the -v2 verbose level.



IV. COMPILING
-----------------
This code uses 64-bit-variables. The implementation of these variables is done
by the file "inttypes.h" introduced in the ANSI-C99-specification. This e.g. works
for gcc when adding the "--std=c99" option.
You MAY find inttypes.h for non-C99-compliant compilers in the internet and you
COULD be able to compile it with this file, but there is no guarantee.



V. LIMITATIONS
-----------------
-Maximum for Rounds is 65535.
-When using the -f parameter to read plain/ciphertext and key from files, this program
 uses the fscanf-function. There may be limits due to the compilers implementation of this
 function, e.g. it could be required to have a new line at the end of the value. Also, the
 behaviour when encountering non-hexadecimal letters is basically undefined.



VI. HISTORY
-----------------
1.x:	No public release, PRESENT still being changed
2.0:	First public release
2.1:	Removed hardcoded 5-bit-limit for key salt in 128-bit key scheduling (for rounds >32)
			Fixed Bug in decryption roundwise output: Text after S-Box was same as after Key-Xor
