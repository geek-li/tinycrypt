
#define RC4Z_SIZE 64

char RC4Z[] = {
  /* 0000 */ "\x60"             /* pushad              */
  /* 0001 */ "\x8b\x7c\x24\x24" /* mov edi, [esp+0x24] */
  /* 0005 */ "\x89\xfe"         /* mov esi, edi        */
  /* 0007 */ "\x31\xc9"         /* xor ecx, ecx        */
  /* 0009 */ "\xf7\xe1"         /* mul ecx             */
  /* 000B */ "\xaa"             /* stosb               */
  /* 000C */ "\xfe\xc0"         /* inc al              */
  /* 000E */ "\x75\xfb"         /* jnz 0xb             */
  /* 0010 */ "\x0f\xb6\xd8"     /* movzx ebx, al       */
  /* 0013 */ "\x80\xe3\x0f"     /* and bl, 0xf         */
  /* 0016 */ "\x02\x14\x1f"     /* add dl, [edi+ebx]   */
  /* 0019 */ "\xeb\x0a"         /* jmp 0x25            */
  /* 001B */ "\xfe\xc0"         /* inc al              */
  /* 001D */ "\x75\xf1"         /* jnz 0x10            */
  /* 001F */ "\x99"             /* cdq                 */
  /* 0020 */ "\x8b\x4f\x10"     /* mov ecx, [edi+0x10] */
  /* 0023 */ "\xfe\xc0"         /* inc al              */
  /* 0025 */ "\x8a\x1c\x06"     /* mov bl, [esi+eax]   */
  /* 0028 */ "\x00\xda"         /* add dl, bl          */
  /* 002A */ "\x86\x1c\x16"     /* xchg [esi+edx], bl  */
  /* 002D */ "\x88\x1c\x06"     /* mov [esi+eax], bl   */
  /* 0030 */ "\xe3\xe9"         /* jecxz 0x1b          */
  /* 0032 */ "\x02\x1c\x16"     /* add bl, [esi+edx]   */
  /* 0035 */ "\x8a\x1c\x1e"     /* mov bl, [esi+ebx]   */
  /* 0038 */ "\x30\x5f\x14"     /* xor [edi+0x14], bl  */
  /* 003B */ "\x47"             /* inc edi             */
  /* 003C */ "\xe2\xe5"         /* loop 0x23           */
  /* 003E */ "\x61"             /* popad               */
  /* 003F */ "\xc3"             /* ret                 */
};