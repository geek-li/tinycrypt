

                Modular Exponentiation in x86 assembly

[ intro

Public key cryptography uses this function for encrypting information.
Due to its slow execution, it's primary purpose is for key exchange
and digital signatures.


[ tests


long_test() in mtest.c will generate random numbers from 32-bits to 8192-bits
and pass these through both BN_mod_exp() and modexp32() comparing results.

Since modexp32 was designed to be small rather than fast, this can take
along time to complete.

So far, I've only ran tests up to 4096-bits but i'm confident it
will work with larger numbers.

Finished testing 1024-bit
Finished testing 2048-bit
Finished testing 3072-bit
Finished testing 4096-bit^C   <-- ended early here


[ compiling mtest.c on windows

  1) Download and extract OpenSSL sources to C:\openssl
  2) Start a visual studio command prompt and drop into the source directory to configure and build.
    
       cd C:\openssl
       perl Configure VC-WIN32 no-asm --prefix=C:\openssl
       ms\do_ms.bat
       nmake -f ms\nt.mak
 
  3) Compile mtest.c using the static libraries.
      
       cl /GS- /Os /O2 /Os mtest.c /I C:\openssl\inc32 c:\openssl\out32\libeay32.lib
