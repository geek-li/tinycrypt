

#include "binmath.h"

int main(int argc, char *argv[])
{
  uint64_t dd=dec2bin(argv[1]);
  uint64_t dr=dec2bin(argv[2]);
  uint64_t q, r;
  
  divide(&q, &r, dd, dr);
  
  printf ("\n%llu %llu", q, r);
  printf ("\n%llu %llu", dd/dr, dd%dr);
  return 0;
}
