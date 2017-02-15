
// example using base64 decoder

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "b64.h"

int main (int argc, char *argv[])
{
  char   *s, *out;
  size_t len;
  
  if (argc != 2) {
    printf ("\nusage: b64d <string>\n");
    return 0;
  }
  
  s=argv[1];
  len=strlen(s);
  out=malloc (len+4*4);
  
  b64decode (out, s);
  printf ("\nBase64(\"%s\") = %s", s, out);
  
  free (out);
  return 0;
}


