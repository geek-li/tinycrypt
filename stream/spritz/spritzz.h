
/* Copyright (c) 2014 Kevin Devine */
/* Size = 357 bytes */

char spritzz[] = {
  /************************************************************************/
  /* 0000 */  "\x60"                  /*  pushad                          */
  /* 0001 */  "\x8B\xF9"              /*  mov edi, ecx                    */
  /* 0003 */  "\x33\xC0"              /*  xor eax, eax                    */
  /* 0005 */  "\xAA"                  /*  stosb                           */
  /* 0006 */  "\xAA"                  /*  stosb                           */
  /* 0007 */  "\xAA"                  /*  stosb                           */
  /* 0008 */  "\xAA"                  /*  stosb                           */
  /* 0009 */  "\xAA"                  /*  stosb                           */
  /* 000A */  "\x40"                  /*  inc eax                         */
  /* 000B */  "\xAA"                  /*  stosb                           */
  /* 000C */  "\x48"                  /*  dec eax                         */
  /* 000D */  "\xAA"                  /*  stosb                           */
  /* 000E */  "\xFE\xC0"              /*  inc al                          */
  /* 0010 */  "\x75\xFB"              /*  jne 0000000Dh                   */
  /* 0012 */  "\x61"                  /*  popad                           */
  /* 0013 */  "\xC3"                  /*  ret                             */
  /************************************************************************/
  /* 0014 */  "\x60"                  /*  pushad                          */
  /* 0015 */  "\x0F\xB6\x41\x01"      /*  movzx eax, byte ptr [ecx+01h]   */
  /* 0019 */  "\x0F\xB6\x59\x02"      /*  movzx ebx, byte ptr [ecx+02h]   */
  /* 001D */  "\x8A\x71\x03"          /*  mov dh, byte ptr [ecx+03h]      */
  /* 0020 */  "\x02\x41\x05"          /*  add al, byte ptr [ecx+05h]      */
  /* 0023 */  "\x8A\x54\x08\x06"      /*  mov dl, byte ptr [eax+ecx+06h]  */
  /* 0027 */  "\x02\xDA"              /*  add bl, dl                      */
  /* 0029 */  "\x8A\x5C\x0B\x06"      /*  mov bl, byte ptr [ebx+ecx+06h]  */
  /* 002D */  "\x02\xDE"              /*  add bl, dh                      */
  /* 002F */  "\x86\x54\x0B\x06"      /*  xchg byte ptr [ebx+ecx+06h], dl */
  /* 0033 */  "\x88\x54\x08\x06"      /*  mov byte ptr [eax+ecx+06h], dl  */
  /* 0037 */  "\x02\xD0"              /*  add dl, al                      */
  /* 0039 */  "\x02\xD6"              /*  add dl, dh                      */
  /* 003B */  "\x88\x41\x01"          /*  mov byte ptr [ecx+01h], al      */
  /* 003E */  "\x88\x59\x02"          /*  mov byte ptr [ecx+02h], bl      */
  /* 0041 */  "\x88\x51\x03"          /*  mov byte ptr [ecx+03h], dl      */
  /* 0044 */  "\x61"                  /*  popad                           */
  /* 0045 */  "\xC3"                  /*  ret                             */
  /************************************************************************/
  /* 0046 */  "\x60"                  /*  pushad                          */
  /* 0047 */  "\x33\xDB"              /*  xor ebx, ebx                    */
  /* 0049 */  "\xB7\x02"              /*  mov bh, 02h                     */
  /* 004B */  "\xE8\xC4\xFF\xFF\xFF"  /*  call 00000014h                  */
  /* 0050 */  "\x4B"                  /*  dec ebx                         */
  /* 0051 */  "\x75\xF8"              /*  jne 0000004Bh                   */
  /* 0053 */  "\x80\x41\x05\x02"      /*  add byte ptr [ecx+05h], 02h     */
  /* 0057 */  "\x61"                  /*  popad                           */
  /* 0058 */  "\xC3"                  /*  ret                             */
  /************************************************************************/
  /* 0059 */  "\x60"                  /*  pushad                          */
  /* 005A */  "\x33\xC0"              /*  xor eax, eax                    */
  /* 005C */  "\x99"                  /*  cdq                             */
  /* 005D */  "\xB2\xFF"              /*  mov dl, FFh                     */
  /* 005F */  "\x2A\xD0"              /*  sub dl, al                      */
  /* 0061 */  "\x8A\x5C\x08\x06"      /*  mov bl, byte ptr [eax+ecx+06h]  */
  /* 0065 */  "\x8A\x7C\x0A\x06"      /*  mov bh, byte ptr [edx+ecx+06h]  */
  /* 0069 */  "\x3A\xDF"              /*  cmp bl, bh                      */
  /* 006B */  "\x76\x08"              /*  jbe 00000075h                   */
  /* 006D */  "\x88\x7C\x08\x06"      /*  mov byte ptr [eax+ecx+06h], bh  */
  /* 0071 */  "\x88\x5C\x0A\x06"      /*  mov byte ptr [edx+ecx+06h], bl  */
  /* 0075 */  "\xFE\xC0"              /*  inc al                          */
  /* 0077 */  "\x79\xE4"              /*  jns 0000005Dh                   */
  /* 0079 */  "\x61"                  /*  popad                           */
  /* 007A */  "\xC3"                  /*  ret                             */
  /************************************************************************/
  /* 007B */  "\xE8\xC6\xFF\xFF\xFF"  /*  call 00000046h                  */
  /* 0080 */  "\xE8\xD4\xFF\xFF\xFF"  /*  call 00000059h                  */
  /* 0085 */  "\xE8\xBC\xFF\xFF\xFF"  /*  call 00000046h                  */
  /* 008A */  "\xE8\xCA\xFF\xFF\xFF"  /*  call 00000059h                  */
  /* 008F */  "\xE8\xB2\xFF\xFF\xFF"  /*  call 00000046h                  */
  /* 0094 */  "\x80\x21\x00"          /*  and byte ptr [ecx], 00h         */
  /* 0097 */  "\xC3"                  /*  ret                             */
  /************************************************************************/
  /* 0098 */  "\x80\x39\x80"          /*  cmp byte ptr [ecx], 80h         */
  /* 009B */  "\x75\x05"              /*  jne 000000A2h                   */
  /* 009D */  "\xE8\xD9\xFF\xFF\xFF"  /*  call 0000007Bh                  */
  /* 00A2 */  "\xC3"                  /*  ret                             */
  /************************************************************************/
  /* 00A3 */  "\x60"                  /*  pushad                          */
  /* 00A4 */  "\xE8\xEF\xFF\xFF\xFF"  /*  call 00000098h                  */
  /* 00A9 */  "\x33\xC0"              /*  xor eax, eax                    */
  /* 00AB */  "\x80\xC2\x80"          /*  add dl, 80h                     */
  /* 00AE */  "\x8A\x44\x0A\x06"      /*  mov al, byte ptr [edx+ecx+06h]  */
  /* 00B2 */  "\x8A\x19"              /*  mov bl, byte ptr [ecx]          */
  /* 00B4 */  "\x86\x44\x0B\x06"      /*  xchg byte ptr [ebx+ecx+06h], al */
  /* 00B8 */  "\x88\x44\x0A\x06"      /*  mov byte ptr [edx+ecx+06h], al  */
  /* 00BC */  "\xFE\x01"              /*  inc byte ptr [ecx]              */
  /* 00BE */  "\x61"                  /*  popad                           */
  /* 00BF */  "\xC3"                  /*  ret                             */
  /************************************************************************/
  /* 00C0 */  "\x60"                  /*  pushad                          */
  /* 00C1 */  "\x8B\xDA"              /*  mov ebx, edx                    */
  /* 00C3 */  "\x83\xE2\x0F"          /*  and edx, 0Fh                    */
  /* 00C6 */  "\xE8\xD8\xFF\xFF\xFF"  /*  call 000000A3h                  */
  /* 00CB */  "\x8B\xD3"              /*  mov edx, ebx                    */
  /* 00CD */  "\xC1\xEA\x04"          /*  shr edx, 04h                    */
  /* 00D0 */  "\xE8\xCE\xFF\xFF\xFF"  /*  call 000000A3h                  */
  /* 00D5 */  "\x61"                  /*  popad                           */
  /* 00D6 */  "\xC3"                  /*  ret                             */
  /************************************************************************/
  /* 00D7 */  "\x60"                  /*  pushad                          */
  /* 00D8 */  "\x33\xDB"              /*  xor ebx, ebx                    */
  /* 00DA */  "\x8B\xF2"              /*  mov esi, edx                    */
  /* 00DC */  "\x0F\xB6\x14\x33"      /*  movzx edx, byte ptr [ebx+esi]   */
  /* 00E0 */  "\xE8\xDB\xFF\xFF\xFF"  /*  call 000000C0h                  */
  /* 00E5 */  "\x43"                  /*  inc ebx                         */
  /* 00E6 */  "\x3B\x5C\x24\x24"      /*  cmp ebx, dword ptr [esp+24h]    */
  /* 00EA */  "\x75\xF0"              /*  jne 000000DCh                   */
  /* 00EC */  "\x61"                  /*  popad                           */
  /* 00ED */  "\xC2\x04\x00"          /*  retn 0004h                      */
  /* 00F0 */  "\xE8\xA3\xFF\xFF\xFF"  /*  call 00000098h                  */
  /* 00F5 */  "\xFE\x01"              /*  inc byte ptr [ecx]              */
  /* 00F7 */  "\xC3"                  /*  ret                             */
  /************************************************************************/
  /* 00F8 */  "\xFF\x74\x24\x04"      /*  push dword ptr [esp+04h]        */
  /* 00FC */  "\xE8\xD6\xFF\xFF\xFF"  /*  call 000000D7h                  */
  /* 0101 */  "\xE8\xEA\xFF\xFF\xFF"  /*  call 000000F0h                  */
  /* 0106 */  "\xC3"                  /*  ret                             */
  /************************************************************************/
  /* 0107 */  "\x80\x39\x00"          /*  cmp byte ptr [ecx], 00h         */
  /* 010A */  "\x76\x05"              /*  jbe 00000111h                   */
  /* 010C */  "\xE8\x6A\xFF\xFF\xFF"  /*  call 0000007Bh                  */
  /* 0111 */  "\xC3"                  /*  ret                             */
  /************************************************************************/
  /* 0112 */  "\x60"                  /*  pushad                          */
  /* 0113 */  "\x0F\xB6\x41\x01"      /*  movzx eax, byte ptr [ecx+01h]   */
  /* 0117 */  "\x0F\xB6\x59\x02"      /*  movzx ebx, byte ptr [ecx+02h]   */
  /* 011B */  "\x0F\xB6\x51\x04"      /*  movzx edx, byte ptr [ecx+04h]   */
  /* 011F */  "\x02\x51\x03"          /*  add dl, byte ptr [ecx+03h]      */
  /* 0122 */  "\x02\x44\x0A\x06"      /*  add al, byte ptr [edx+ecx+06h]  */
  /* 0126 */  "\x02\x5C\x08\x06"      /*  add bl, byte ptr [eax+ecx+06h]  */
  /* 012A */  "\x8A\x44\x0B\x06"      /*  mov al, byte ptr [ebx+ecx+06h]  */
  /* 012E */  "\x88\x41\x04"          /*  mov byte ptr [ecx+04h], al      */
  /* 0131 */  "\x89\x44\x24\x1C"      /*  mov dword ptr [esp+1Ch], eax    */
  /* 0135 */  "\x61"                  /*  popad                           */
  /* 0136 */  "\xC3"                  /*  ret                             */
  /************************************************************************/
  /* 0137 */  "\xE8\xD8\xFE\xFF\xFF"  /*  call 00000014h                  */
  /* 013C */  "\xE8\xD1\xFF\xFF\xFF"  /*  call 00000112h                  */
  /* 0141 */  "\xC3"                  /*  ret                             */
  /************************************************************************/
  /* 0142 */  "\x60"                  /*  pushad                          */
  /* 0143 */  "\xE8\xBF\xFF\xFF\xFF"  /*  call 00000107h                  */
  /* 0148 */  "\x8B\x5C\x24\x24"      /*  mov ebx, dword ptr [esp+24h]    */
  /* 014C */  "\x8B\xFA"              /*  mov edi, edx                    */
  /* 014E */  "\xE8\xE4\xFF\xFF\xFF"  /*  call 00000137h                  */
  /* 0153 */  "\xAA"                  /*  stosb                           */
  /* 0154 */  "\x4B"                  /*  dec ebx                         */
  /* 0155 */  "\x75\xF7"              /*  jne 0000014Eh                   */
  /* 0157 */  "\x61"                  /*  popad                           */
  /* 0158 */  "\x58"                  /*  pop eax                         */
  /* 0159 */  "\xC3"                  /*  ret                             */
  /************************************************************************/
  /* 015A */  "\xE8\xA8\xFF\xFF\xFF"  /*  call 00000107h                  */
  /* 015F */  "\xE8\xD3\xFF\xFF\xFF"  /*  call 00000137h                  */
  /* 0164 */  "\xC3"                  /*  ret                             */
  /************************************************************************/
};
