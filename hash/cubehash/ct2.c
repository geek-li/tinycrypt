

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "ch.h"

int main(int argc, char *argv[])
{
  cube_ctx  c;
  uint8_t   out[128];
  int       i, r, b, h;
  char      *s;
  
  if (argc!=5) {
    printf ("\nch <r> <b> <h> <string>\n");
    return 0;
  }
  
  memset (out, 0, sizeof(out));
  
  r=atoi(argv[1]);
  b=atoi(argv[2]);
  h=atoi(argv[3]);
  s=argv[4];
  
  cube_init(&c, r, b, h);
  cube_update(&c, s, strlen(s));
  cube_final(out, &c);
  
  for (i=0; i<c.h/8; i++) {
    printf ("%02x", out[i]);
  }
  return 0;
}
