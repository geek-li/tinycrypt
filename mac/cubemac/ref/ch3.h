
#define CH3_SIZE 355

char CH3[] = {
  /* 0000 */ "\x55"                         /* push ebp                   */
  /* 0001 */ "\x8b\xec"                     /* mov ebp, esp               */
  /* 0003 */ "\x83\xec\x54"                 /* sub esp, 0x54              */
  /* 0006 */ "\x53"                         /* push ebx                   */
  /* 0007 */ "\x56"                         /* push esi                   */
  /* 0008 */ "\x57"                         /* push edi                   */
  /* 0009 */ "\x8b\xd1"                     /* mov edx, ecx               */
  /* 000B */ "\x8d\x45\xac"                 /* lea eax, [ebp-0x54]        */
  /* 000E */ "\x2b\xc2"                     /* sub eax, edx               */
  /* 0010 */ "\x6a\x04"                     /* push 0x4                   */
  /* 0012 */ "\xc7\x45\xf0\x10\x00\x00\x00" /* mov dword [ebp-0x10], 0x10 */
  /* 0019 */ "\x89\x45\xec"                 /* mov [ebp-0x14], eax        */
  /* 001C */ "\x5f"                         /* pop edi                    */
  /* 001D */ "\xc7\x45\xf8\x07\x00\x00\x00" /* mov dword [ebp-0x8], 0x7   */
  /* 0024 */ "\xc7\x45\xf4\x02\x00\x00\x00" /* mov dword [ebp-0xc], 0x2   */
  /* 002B */ "\xc7\x45\xfc\x08\x00\x00\x00" /* mov dword [ebp-0x4], 0x8   */
  /* 0032 */ "\x6a\x0f"                     /* push 0xf                   */
  /* 0034 */ "\x5e"                         /* pop esi                    */
  /* 0035 */ "\x8d\x42\x7c"                 /* lea eax, [edx+0x7c]        */
  /* 0038 */ "\x8b\x48\xc0"                 /* mov ecx, [eax-0x40]        */
  /* 003B */ "\x01\x08"                     /* add [eax], ecx             */
  /* 003D */ "\x2b\xc7"                     /* sub eax, edi               */
  /* 003F */ "\x4e"                         /* dec esi                    */
  /* 0040 */ "\x79\xf6"                     /* jns 0x38                   */
  /* 0042 */ "\x6a\x0f"                     /* push 0xf                   */
  /* 0044 */ "\x58"                         /* pop eax                    */
  /* 0045 */ "\x8b\x4d\xfc"                 /* mov ecx, [ebp-0x4]         */
  /* 0048 */ "\x8b\x34\x82"                 /* mov esi, [edx+eax*4]       */
  /* 004B */ "\x33\xc8"                     /* xor ecx, eax               */
  /* 004D */ "\x48"                         /* dec eax                    */
  /* 004E */ "\x89\x74\x8d\xac"             /* mov [ebp+ecx*4-0x54], esi  */
  /* 0052 */ "\x79\xf1"                     /* jns 0x45                   */
  /* 0054 */ "\x6a\x0f"                     /* push 0xf                   */
  /* 0056 */ "\x5e"                         /* pop esi                    */
  /* 0057 */ "\x8d\x42\x3c"                 /* lea eax, [edx+0x3c]        */
  /* 005A */ "\x8b\x4d\xec"                 /* mov ecx, [ebp-0x14]        */
  /* 005D */ "\x8b\x1c\x08"                 /* mov ebx, [eax+ecx]         */
  /* 0060 */ "\x8b\x4d\xf8"                 /* mov ecx, [ebp-0x8]         */
  /* 0063 */ "\xd3\xc3"                     /* rol ebx, cl                */
  /* 0065 */ "\x89\x18"                     /* mov [eax], ebx             */
  /* 0067 */ "\x2b\xc7"                     /* sub eax, edi               */
  /* 0069 */ "\x4e"                         /* dec esi                    */
  /* 006A */ "\x79\xee"                     /* jns 0x5a                   */
  /* 006C */ "\x6a\x0f"                     /* push 0xf                   */
  /* 006E */ "\x5e"                         /* pop esi                    */
  /* 006F */ "\x8b\x4c\xb2\x40"             /* mov ecx, [edx+esi*4+0x40]  */
  /* 0073 */ "\x8d\x04\xb2"                 /* lea eax, [edx+esi*4]       */
  /* 0076 */ "\x31\x08"                     /* xor [eax], ecx             */
  /* 0078 */ "\x4e"                         /* dec esi                    */
  /* 0079 */ "\x79\xf4"                     /* jns 0x6f                   */
  /* 007B */ "\x6a\x0f"                     /* push 0xf                   */
  /* 007D */ "\x5e"                         /* pop esi                    */
  /* 007E */ "\x8d\x42\x7c"                 /* lea eax, [edx+0x7c]        */
  /* 0081 */ "\x8b\x18"                     /* mov ebx, [eax]             */
  /* 0083 */ "\x8b\xce"                     /* mov ecx, esi               */
  /* 0085 */ "\x33\x4d\xf4"                 /* xor ecx, [ebp-0xc]         */
  /* 0088 */ "\x2b\xc7"                     /* sub eax, edi               */
  /* 008A */ "\x4e"                         /* dec esi                    */
  /* 008B */ "\x89\x5c\x8d\xac"             /* mov [ebp+ecx*4-0x54], ebx  */
  /* 008F */ "\x79\xf0"                     /* jns 0x81                   */
  /* 0091 */ "\x6a\x0f"                     /* push 0xf                   */
  /* 0093 */ "\x58"                         /* pop eax                    */
  /* 0094 */ "\x8d\x72\x7c"                 /* lea esi, [edx+0x7c]        */
  /* 0097 */ "\x8b\x4c\x85\xac"             /* mov ecx, [ebp+eax*4-0x54]  */
  /* 009B */ "\x89\x0e"                     /* mov [esi], ecx             */
  /* 009D */ "\x2b\xf7"                     /* sub esi, edi               */
  /* 009F */ "\x48"                         /* dec eax                    */
  /* 00A0 */ "\x79\xf5"                     /* jns 0x97                   */
  /* 00A2 */ "\x01\x7d\xf8"                 /* add [ebp-0x8], edi         */
  /* 00A5 */ "\x29\x7d\xfc"                 /* sub [ebp-0x4], edi         */
  /* 00A8 */ "\xff\x4d\xf4"                 /* dec dword [ebp-0xc]        */
  /* 00AB */ "\x83\x7d\xfc\x00"             /* cmp dword [ebp-0x4], 0x0   */
  /* 00AF */ "\x7f\x81"                     /* jg 0x32                    */
  /* 00B1 */ "\xff\x4d\xf0"                 /* dec dword [ebp-0x10]       */
  /* 00B4 */ "\x83\x7d\xf0\x00"             /* cmp dword [ebp-0x10], 0x0  */
  /* 00B8 */ "\x0f\x8f\x5f\xff\xff\xff"     /* jg 0x1d                    */
  /* 00BE */ "\x5f"                         /* pop edi                    */
  /* 00BF */ "\x5e"                         /* pop esi                    */
  /* 00C0 */ "\x5b"                         /* pop ebx                    */
  /* 00C1 */ "\xc9"                         /* leave                      */
  /* 00C2 */ "\xc3"                         /* ret                        */
  /* 00C3 */ "\x53"                         /* push ebx                   */
  /* 00C4 */ "\x56"                         /* push esi                   */
  /* 00C5 */ "\x8b\xf1"                     /* mov esi, ecx               */
  /* 00C7 */ "\x57"                         /* push edi                   */
  /* 00C8 */ "\x32\xc0"                     /* xor al, al                 */
  /* 00CA */ "\x8b\xfe"                     /* mov edi, esi               */
  /* 00CC */ "\xb9\x80\x00\x00\x00"         /* mov ecx, 0x80              */
  /* 00D1 */ "\xf3\xaa"                     /* rep stosb                  */
  /* 00D3 */ "\x6a\x10"                     /* push 0x10                  */
  /* 00D5 */ "\x58"                         /* pop eax                    */
  /* 00D6 */ "\x8b\xce"                     /* mov ecx, esi               */
  /* 00D8 */ "\x8b\xda"                     /* mov ebx, edx               */
  /* 00DA */ "\x89\x06"                     /* mov [esi], eax             */
  /* 00DC */ "\xc7\x46\x04\x20\x00\x00\x00" /* mov dword [esi+0x4], 0x20  */
  /* 00E3 */ "\x89\x46\x08"                 /* mov [esi+0x8], eax         */
  /* 00E6 */ "\xe8\x15\xff\xff\xff"         /* call 0x0                   */
  /* 00EB */ "\x6a\x02"                     /* push 0x2                   */
  /* 00ED */ "\x5f"                         /* pop edi                    */
  /* 00EE */ "\x33\xc0"                     /* xor eax, eax               */
  /* 00F0 */ "\x8b\x0b"                     /* mov ecx, [ebx]             */
  /* 00F2 */ "\x31\x0c\x86"                 /* xor [esi+eax*4], ecx       */
  /* 00F5 */ "\x83\xc3\x04"                 /* add ebx, 0x4               */
  /* 00F8 */ "\x40"                         /* inc eax                    */
  /* 00F9 */ "\x83\xf8\x08"                 /* cmp eax, 0x8               */
  /* 00FC */ "\x7c\xf2"                     /* jl 0xf0                    */
  /* 00FE */ "\x8b\xce"                     /* mov ecx, esi               */
  /* 0100 */ "\xe8\xfb\xfe\xff\xff"         /* call 0x0                   */
  /* 0105 */ "\x4f"                         /* dec edi                    */
  /* 0106 */ "\x75\xe6"                     /* jnz 0xee                   */
  /* 0108 */ "\x5f"                         /* pop edi                    */
  /* 0109 */ "\x5e"                         /* pop esi                    */
  /* 010A */ "\x5b"                         /* pop ebx                    */
  /* 010B */ "\xc3"                         /* ret                        */
  /* 010C */ "\x53"                         /* push ebx                   */
  /* 010D */ "\x56"                         /* push esi                   */
  /* 010E */ "\x57"                         /* push edi                   */
  /* 010F */ "\x33\xff"                     /* xor edi, edi               */
  /* 0111 */ "\x33\xc0"                     /* xor eax, eax               */
  /* 0113 */ "\x8b\xda"                     /* mov ebx, edx               */
  /* 0115 */ "\x8b\xf1"                     /* mov esi, ecx               */
  /* 0117 */ "\x39\x7c\x24\x10"             /* cmp [esp+0x10], edi        */
  /* 011B */ "\x76\x1c"                     /* jbe 0x139                  */
  /* 011D */ "\x8a\x0c\x3b"                 /* mov cl, [ebx+edi]          */
  /* 0120 */ "\x30\x0c\x06"                 /* xor [esi+eax], cl          */
  /* 0123 */ "\x40"                         /* inc eax                    */
  /* 0124 */ "\x83\xf8\x20"                 /* cmp eax, 0x20              */
  /* 0127 */ "\x75\x09"                     /* jnz 0x132                  */
  /* 0129 */ "\x8b\xce"                     /* mov ecx, esi               */
  /* 012B */ "\xe8\xd0\xfe\xff\xff"         /* call 0x0                   */
  /* 0130 */ "\x33\xc0"                     /* xor eax, eax               */
  /* 0132 */ "\x47"                         /* inc edi                    */
  /* 0133 */ "\x3b\x7c\x24\x10"             /* cmp edi, [esp+0x10]        */
  /* 0137 */ "\x72\xe4"                     /* jb 0x11d                   */
  /* 0139 */ "\x80\x34\x06\x80"             /* xor byte [esi+eax], 0x80   */
  /* 013D */ "\x8b\xce"                     /* mov ecx, esi               */
  /* 013F */ "\xe8\xbc\xfe\xff\xff"         /* call 0x0                   */
  /* 0144 */ "\x83\x76\x7c\x01"             /* xor dword [esi+0x7c], 0x1  */
  /* 0148 */ "\x6a\x02"                     /* push 0x2                   */
  /* 014A */ "\x5f"                         /* pop edi                    */
  /* 014B */ "\x8b\xce"                     /* mov ecx, esi               */
  /* 014D */ "\xe8\xae\xfe\xff\xff"         /* call 0x0                   */
  /* 0152 */ "\x4f"                         /* dec edi                    */
  /* 0153 */ "\x75\xf6"                     /* jnz 0x14b                  */
  /* 0155 */ "\x8b\x7c\x24\x14"             /* mov edi, [esp+0x14]        */
  /* 0159 */ "\xa5"                         /* movsd                      */
  /* 015A */ "\xa5"                         /* movsd                      */
  /* 015B */ "\xa5"                         /* movsd                      */
  /* 015C */ "\xa5"                         /* movsd                      */
  /* 015D */ "\x5f"                         /* pop edi                    */
  /* 015E */ "\x5e"                         /* pop esi                    */
  /* 015F */ "\x5b"                         /* pop ebx                    */
  /* 0160 */ "\xc2\x08\x00"                 /* ret 0x8                    */
};