

bits 32

start:
  int3
  push -800
  pop eax
  
  ; the following requires x be equal to a
  ; if (x == a) {
  ;   x = b;
  ; } else {
  ;   x = a;
  ; }
  
  mov eax, 45341231h ; set a
  mov ebx, 00009876h ; set b
  mov ecx, 12341234h ; set c
  
  mov  eax, 0
  ; if (a==0) a=b else a=c
  cmp  eax, 1    ;
  sbb  edx, edx  ; d = (a==0) ? -1 : 0;
  mov  eax, ebx  ; a = b
  and  eax, edx  ; a &= d;
  xor  eax, ecx  ; 
  
  ; y <-- a + b - x
  lea edx, [eax+ebx]
  sub edx, ecx
  
  ; y <-- a ^ b ^ x
  mov edx, eax
  xor edx, ebx
  xor edx, ecx
  
  ; C
  
  ; D
  mov edx, eax
  cmp edx, ecx
  cmove edx, ebx
  