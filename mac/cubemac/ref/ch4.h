
#define CH4_SIZE 377

char CH4[] = {
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
  /* 00C3 */ "\x55"                         /* push ebp                   */
  /* 00C4 */ "\x8d\x6c\x24\x90"             /* lea ebp, [esp-0x70]        */
  /* 00C8 */ "\x81\xec\x84\x00\x00\x00"     /* sub esp, 0x84              */
  /* 00CE */ "\x53"                         /* push ebx                   */
  /* 00CF */ "\x56"                         /* push esi                   */
  /* 00D0 */ "\x57"                         /* push edi                   */
  /* 00D1 */ "\x8b\xf1"                     /* mov esi, ecx               */
  /* 00D3 */ "\x32\xc0"                     /* xor al, al                 */
  /* 00D5 */ "\x6a\x10"                     /* push 0x10                  */
  /* 00D7 */ "\x8d\x7d\xec"                 /* lea edi, [ebp-0x14]        */
  /* 00DA */ "\xb9\x80\x00\x00\x00"         /* mov ecx, 0x80              */
  /* 00DF */ "\xf3\xaa"                     /* rep stosb                  */
  /* 00E1 */ "\x58"                         /* pop eax                    */
  /* 00E2 */ "\x6a\x20"                     /* push 0x20                  */
  /* 00E4 */ "\x5f"                         /* pop edi                    */
  /* 00E5 */ "\x8d\x4d\xec"                 /* lea ecx, [ebp-0x14]        */
  /* 00E8 */ "\x8b\xda"                     /* mov ebx, edx               */
  /* 00EA */ "\x89\x45\xec"                 /* mov [ebp-0x14], eax        */
  /* 00ED */ "\x89\x7d\xf0"                 /* mov [ebp-0x10], edi        */
  /* 00F0 */ "\x89\x45\xf4"                 /* mov [ebp-0xc], eax         */
  /* 00F3 */ "\xe8\x08\xff\xff\xff"         /* call 0x0                   */
  /* 00F8 */ "\xc7\x45\x6c\x02\x00\x00\x00" /* mov dword [ebp+0x6c], 0x2  */
  /* 00FF */ "\x33\xc0"                     /* xor eax, eax               */
  /* 0101 */ "\x8b\x0e"                     /* mov ecx, [esi]             */
  /* 0103 */ "\x31\x4c\x85\xec"             /* xor [ebp+eax*4-0x14], ecx  */
  /* 0107 */ "\x83\xc6\x04"                 /* add esi, 0x4               */
  /* 010A */ "\x40"                         /* inc eax                    */
  /* 010B */ "\x83\xf8\x08"                 /* cmp eax, 0x8               */
  /* 010E */ "\x72\xf1"                     /* jb 0x101                   */
  /* 0110 */ "\x8d\x4d\xec"                 /* lea ecx, [ebp-0x14]        */
  /* 0113 */ "\xe8\xe8\xfe\xff\xff"         /* call 0x0                   */
  /* 0118 */ "\xff\x4d\x6c"                 /* dec dword [ebp+0x6c]       */
  /* 011B */ "\x75\xe2"                     /* jnz 0xff                   */
  /* 011D */ "\x33\xc0"                     /* xor eax, eax               */
  /* 011F */ "\x33\xf6"                     /* xor esi, esi               */
  /* 0121 */ "\x39\x45\x78"                 /* cmp [ebp+0x78], eax        */
  /* 0124 */ "\x76\x1c"                     /* jbe 0x142                  */
  /* 0126 */ "\x8a\x0c\x33"                 /* mov cl, [ebx+esi]          */
  /* 0129 */ "\x30\x4c\x28\xec"             /* xor [eax+ebp-0x14], cl     */
  /* 012D */ "\x40"                         /* inc eax                    */
  /* 012E */ "\x3b\xc7"                     /* cmp eax, edi               */
  /* 0130 */ "\x75\x0a"                     /* jnz 0x13c                  */
  /* 0132 */ "\x8d\x4d\xec"                 /* lea ecx, [ebp-0x14]        */
  /* 0135 */ "\xe8\xc6\xfe\xff\xff"         /* call 0x0                   */
  /* 013A */ "\x33\xc0"                     /* xor eax, eax               */
  /* 013C */ "\x46"                         /* inc esi                    */
  /* 013D */ "\x3b\x75\x78"                 /* cmp esi, [ebp+0x78]        */
  /* 0140 */ "\x72\xe4"                     /* jb 0x126                   */
  /* 0142 */ "\x8d\x44\x28\xec"             /* lea eax, [eax+ebp-0x14]    */
  /* 0146 */ "\x80\x30\x80"                 /* xor byte [eax], 0x80       */
  /* 0149 */ "\x8d\x4d\xec"                 /* lea ecx, [ebp-0x14]        */
  /* 014C */ "\xe8\xaf\xfe\xff\xff"         /* call 0x0                   */
  /* 0151 */ "\x83\x75\x68\x01"             /* xor dword [ebp+0x68], 0x1  */
  /* 0155 */ "\x8d\x4d\xec"                 /* lea ecx, [ebp-0x14]        */
  /* 0158 */ "\xe8\xa3\xfe\xff\xff"         /* call 0x0                   */
  /* 015D */ "\x8d\x4d\xec"                 /* lea ecx, [ebp-0x14]        */
  /* 0160 */ "\xe8\x9b\xfe\xff\xff"         /* call 0x0                   */
  /* 0165 */ "\x8b\x7d\x7c"                 /* mov edi, [ebp+0x7c]        */
  /* 0168 */ "\x8d\x75\xec"                 /* lea esi, [ebp-0x14]        */
  /* 016B */ "\xa5"                         /* movsd                      */
  /* 016C */ "\xa5"                         /* movsd                      */
  /* 016D */ "\xa5"                         /* movsd                      */
  /* 016E */ "\xa5"                         /* movsd                      */
  /* 016F */ "\x5f"                         /* pop edi                    */
  /* 0170 */ "\x5e"                         /* pop esi                    */
  /* 0171 */ "\x5b"                         /* pop ebx                    */
  /* 0172 */ "\x83\xc5\x70"                 /* add ebp, 0x70              */
  /* 0175 */ "\xc9"                         /* leave                      */
  /* 0176 */ "\xc2\x08\x00"                 /* ret 0x8                    */
};