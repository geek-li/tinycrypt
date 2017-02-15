
#define mx_SIZE 143

char mx[]= {
  /* 0000 */ "\x60"                 /* pushad                   */
  /* 0001 */ "\x8d\x74\x24\x24"     /* lea esi, [esp+0x24]      */
  /* 0005 */ "\xad"                 /* lodsd                    */
  /* 0006 */ "\x99"                 /* cdq                      */
  /* 0007 */ "\x91"                 /* xchg ecx, eax            */
  /* 0008 */ "\xad"                 /* lodsd                    */
  /* 0009 */ "\x50"                 /* push eax                 */
  /* 000A */ "\xad"                 /* lodsd                    */
  /* 000B */ "\x93"                 /* xchg ebx, eax            */
  /* 000C */ "\xad"                 /* lodsd                    */
  /* 000D */ "\x95"                 /* xchg ebp, eax            */
  /* 000E */ "\xad"                 /* lodsd                    */
  /* 000F */ "\x97"                 /* xchg edi, eax            */
  /* 0010 */ "\x5e"                 /* pop esi                  */
  /* 0011 */ "\x42"                 /* inc edx                  */
  /* 0012 */ "\xf9"                 /* stc                      */
  /* 0013 */ "\xb0\x60"             /* mov al, 0x60             */
  /* 0015 */ "\x57"                 /* push edi                 */
  /* 0016 */ "\x29\xcc"             /* sub esp, ecx             */
  /* 0018 */ "\x89\xe7"             /* mov edi, esp             */
  /* 001A */ "\x60"                 /* pushad                   */
  /* 001B */ "\x49"                 /* dec ecx                  */
  /* 001C */ "\x92"                 /* xchg edx, eax            */
  /* 001D */ "\xaa"                 /* stosb                    */
  /* 001E */ "\x30\xc0"             /* xor al, al               */
  /* 0020 */ "\xf3\xaa"             /* rep stosb                */
  /* 0022 */ "\x61"                 /* popad                    */
  /* 0023 */ "\x29\xcc"             /* sub esp, ecx             */
  /* 0025 */ "\x89\xe0"             /* mov eax, esp             */
  /* 0027 */ "\x60"                 /* pushad                   */
  /* 0028 */ "\x97"                 /* xchg edi, eax            */
  /* 0029 */ "\xf3\xa4"             /* rep movsb                */
  /* 002B */ "\x61"                 /* popad                    */
  /* 002C */ "\x96"                 /* xchg esi, eax            */
  /* 002D */ "\xe8\x26\x00\x00\x00" /* call 0x58                */
  /* 0032 */ "\x60"                 /* pushad                   */
  /* 0033 */ "\xc1\xe9\x02"         /* shr ecx, 0x2             */
  /* 0036 */ "\xf8"                 /* clc                      */
  /* 0037 */ "\x60"                 /* pushad                   */
  /* 0038 */ "\xad"                 /* lodsd                    */
  /* 0039 */ "\x13\x07"             /* adc eax, [edi]           */
  /* 003B */ "\xab"                 /* stosd                    */
  /* 003C */ "\xe2\xfa"             /* loop 0x38                */
  /* 003E */ "\x61"                 /* popad                    */
  /* 003F */ "\x89\xee"             /* mov esi, ebp             */
  /* 0041 */ "\x51"                 /* push ecx                 */
  /* 0042 */ "\x49"                 /* dec ecx                  */
  /* 0043 */ "\x8b\x04\x8f"         /* mov eax, [edi+ecx*4]     */
  /* 0046 */ "\x3b\x04\x8e"         /* cmp eax, [esi+ecx*4]     */
  /* 0049 */ "\xe1\xf8"             /* loope 0x43               */
  /* 004B */ "\x59"                 /* pop ecx                  */
  /* 004C */ "\x72\x08"             /* jb 0x56                  */
  /* 004E */ "\x8b\x07"             /* mov eax, [edi]           */
  /* 0050 */ "\x1b\x06"             /* sbb eax, [esi]           */
  /* 0052 */ "\xab"                 /* stosd                    */
  /* 0053 */ "\xad"                 /* lodsd                    */
  /* 0054 */ "\xe2\xf8"             /* loop 0x4e                */
  /* 0056 */ "\x61"                 /* popad                    */
  /* 0057 */ "\xc3"                 /* ret                      */
  /* 0058 */ "\x4a"                 /* dec edx                  */
  /* 0059 */ "\x78\x04"             /* js 0x5f                  */
  /* 005B */ "\x83\x2c\x24\x1e"     /* sub dword [esp], 0x1e    */
  /* 005F */ "\x31\xd2"             /* xor edx, edx             */
  /* 0061 */ "\x8d\x04\xca"         /* lea eax, [edx+ecx*8]     */
  /* 0064 */ "\x48"                 /* dec eax                  */
  /* 0065 */ "\x74\x05"             /* jz 0x6c                  */
  /* 0067 */ "\x0f\xa3\x03"         /* bt [ebx], eax            */
  /* 006A */ "\x73\xf8"             /* jae 0x64                 */
  /* 006C */ "\x0f\xa3\x13"         /* bt [ebx], edx            */
  /* 006F */ "\x9c"                 /* pushfd                   */
  /* 0070 */ "\x60"                 /* pushad                   */
  /* 0071 */ "\x99"                 /* cdq                      */
  /* 0072 */ "\x0f\x43\xfe"         /* cmovae edi, esi          */
  /* 0075 */ "\x89\xfb"             /* mov ebx, edi             */
  /* 0077 */ "\xff\x54\x24\x24"     /* call dword [esp+0x24]    */
  /* 007B */ "\x61"                 /* popad                    */
  /* 007C */ "\x9d"                 /* popfd                    */
  /* 007D */ "\xf5"                 /* cmc                      */
  /* 007E */ "\x73\xef"             /* jae 0x6f                 */
  /* 0080 */ "\x42"                 /* inc edx                  */
  /* 0081 */ "\x48"                 /* dec eax                  */
  /* 0082 */ "\x79\xe8"             /* jns 0x6c                 */
  /* 0084 */ "\x89\xfe"             /* mov esi, edi             */
  /* 0086 */ "\x8d\x64\x4c\x04"     /* lea esp, [esp+ecx*2+0x4] */
  /* 008A */ "\x5f"                 /* pop edi                  */
  /* 008B */ "\xf3\xa4"             /* rep movsb                */
  /* 008D */ "\x61"                 /* popad                    */
  /* 008E */ "\xc3"                 /* ret                      */
};