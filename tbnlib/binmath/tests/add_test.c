

#include "binmath.h"

int main(int argc, char *argv[])
{
  uint64_t dd=dec2bin(argv[1]);
  uint64_t dr=dec2bin(argv[2]);
  uint64_t q, r;
  
  r=add(dd, dr);
  
  printf ("\n%llu", r);
  return 0;
}
