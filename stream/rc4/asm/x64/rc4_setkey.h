
#define rc4_setkey_SIZE 52

char rc4_setkey[]= {
  /* 0000 */ "\x56"         /* push rsi           */
  /* 0001 */ "\x57"         /* push rdi           */
  /* 0002 */ "\x55"         /* push rbp           */
  /* 0003 */ "\x41\x50"     /* push r8            */
  /* 0005 */ "\x5f"         /* pop rdi            */
  /* 0006 */ "\x51"         /* push rcx           */
  /* 0007 */ "\x5d"         /* pop rbp            */
  /* 0008 */ "\x52"         /* push rdx           */
  /* 0009 */ "\x5e"         /* pop rsi            */
  /* 000A */ "\x33\xc0"     /* xor eax, eax       */
  /* 000C */ "\x99"         /* cdq                */
  /* 000D */ "\xab"         /* stosd              */
  /* 000E */ "\xab"         /* stosd              */
  /* 000F */ "\x88\x04\x38" /* mov [rax+rdi], al  */
  /* 0012 */ "\xfe\xc0"     /* inc al             */
  /* 0014 */ "\x75\xf9"     /* jnz 0xf            */
  /* 0016 */ "\x3b\xcd"     /* cmp ecx, ebp       */
  /* 0018 */ "\x75\x02"     /* jnz 0x1c           */
  /* 001A */ "\x33\xc9"     /* xor ecx, ecx       */
  /* 001C */ "\x8a\x1c\x38" /* mov bl, [rax+rdi]  */
  /* 001F */ "\x02\xd3"     /* add dl, bl         */
  /* 0021 */ "\x02\x14\x31" /* add dl, [rcx+rsi]  */
  /* 0024 */ "\x86\x1c\x3a" /* xchg [rdx+rdi], bl */
  /* 0027 */ "\x88\x1c\x38" /* mov [rax+rdi], bl  */
  /* 002A */ "\xff\xc1"     /* inc ecx            */
  /* 002C */ "\xfe\xc0"     /* inc al             */
  /* 002E */ "\x75\xe6"     /* jnz 0x16           */
  /* 0030 */ "\x5d"         /* pop rbp            */
  /* 0031 */ "\x5f"         /* pop rdi            */
  /* 0032 */ "\x5e"         /* pop rsi            */
  /* 0033 */ "\xc3"         /* ret                */
};