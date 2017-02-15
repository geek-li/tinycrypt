

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "cubehash.h"

int main(int argc, char *argv[])
{
  cube_ctx  c;
  hashState s;
  uint8_t   out[128];
  int       i;
  
  if (argc!=2) {
    printf ("\nch <string>\n");
    return 0;
  }
  
  memset (out, 0, sizeof(out));
  
  cube_init(&c);
  cube_update(&c, argv[1], strlen(argv[1]));
  cube_final(out, &c);
  
  for (i=0; i<32; i++) {
    printf ("%02x", out[i]);
  }
  
  printf ("\n");
  
  Init(&s, 256);
  Update(&s, argv[1], strlen(argv[1]) * 8);
  Final(&s, out);
  
  for (i=0; i<32; i++) {
    printf ("%02x", out[i]);
  }
  return 0;
}
