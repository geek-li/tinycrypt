
Designers         Daniel J. Bernstein
Certification     SHA-3
Digest sizes      8, 16, 24, 32, ... 512
Structure         Wide-pipe construction
Rounds            16 (recommended but adjustable)

There are 3 versions provided here.

ch1.c fixed parameters {r=1, b=1, h=256}
ch2.c compile time parameters {CH_R_PARAM, CH_B_PARAM, CH_H_PARAM}
ch3.c runtime parameters passed to cube_init() function.

CubeHash has 5 parameters, a certain instance is denoted by CubeHashi+r/b+f-h.

    i is the number of initial rounds (default is 10)
    r is the number of rounds per block
    b is the block size in bytes, defined for {1, 2, 3, ... 128}
    f is the number of final rounds (default is 10)
    h is the size of the hash output in bits, defined for {8, 16, 24, 32, ... 512}

    
  Copyright © 2016 Odzhan, Peter Ferrie. All Rights Reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  3. The name of the author may not be used to endorse or promote products
  derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY AUTHORS "AS IS" AND ANY EXPRESS OR
  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
  