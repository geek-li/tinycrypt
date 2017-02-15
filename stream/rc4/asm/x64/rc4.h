
#define rc4_SIZE 45

char rc4[]= {
  /* 0000 */ "\x56"         /* push rsi           */
  /* 0001 */ "\x57"         /* push rdi           */
  /* 0002 */ "\x41\x50"     /* push r8            */
  /* 0004 */ "\x5e"         /* pop rsi            */
  /* 0005 */ "\x52"         /* push rdx           */
  /* 0006 */ "\x5f"         /* pop rdi            */
  /* 0007 */ "\x56"         /* push rsi           */
  /* 0008 */ "\xad"         /* lodsd              */
  /* 0009 */ "\x93"         /* xchg ebx, eax      */
  /* 000A */ "\xad"         /* lodsd              */
  /* 000B */ "\x93"         /* xchg ebx, eax      */
  /* 000C */ "\x99"         /* cdq                */
  /* 000D */ "\xfe\xc0"     /* inc al             */
  /* 000F */ "\x8a\x14\x30" /* mov dl, [rax+rsi]  */
  /* 0012 */ "\x02\xda"     /* add bl, dl         */
  /* 0014 */ "\x86\x14\x33" /* xchg [rbx+rsi], dl */
  /* 0017 */ "\x88\x14\x30" /* mov [rax+rsi], dl  */
  /* 001A */ "\x02\x14\x33" /* add dl, [rbx+rsi]  */
  /* 001D */ "\x8a\x14\x32" /* mov dl, [rdx+rsi]  */
  /* 0020 */ "\x30\x17"     /* xor [rdi], dl      */
  /* 0022 */ "\xff\xc7"     /* inc edi            */
  /* 0024 */ "\xe2\xe7"     /* loop 0xd           */
  /* 0026 */ "\x5f"         /* pop rdi            */
  /* 0027 */ "\xab"         /* stosd              */
  /* 0028 */ "\x93"         /* xchg ebx, eax      */
  /* 0029 */ "\xab"         /* stosd              */
  /* 002A */ "\x5f"         /* pop rdi            */
  /* 002B */ "\x5e"         /* pop rsi            */
  /* 002C */ "\xc3"         /* ret                */
};