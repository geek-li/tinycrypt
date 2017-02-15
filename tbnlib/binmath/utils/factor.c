

// Factor a number using Pollard Rho algorithm
// Odzhan

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "binmath.h"

void factor (uint64_t n)
{
  uint64_t divisor;
  if (n == 1) return;
  if (fermat (n, 20) && miller (n, 20)) {
    printf ("\n%llu", n);
    return;
  }
  divisor=rho (n);
  factor (divisor);
  factor (n/divisor);
}

int main (int argc, char *argv[])
{
  uint64_t n;
  
  if (argc != 2) {
    printf ("\n  usage: factor <number>\n");
    return 0;
  }
  n=dec2bin (argv[1]);
  factor (n);
  return 0;
}
