

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
  uint32_t x, y, z;
  
  if (argc!=3) {
    printf ("\npct <before> <after>\n");
    return 0;
  }
  
  x = atoi(argv[1]);
  y = atoi(argv[2]);
  
  printf ("\nreduction is %.f %%", (float)((x - y) * 100) / x);
  return 0;
}
