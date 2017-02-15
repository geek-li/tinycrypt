/************************************************************************************/
/* Noekeontest.c
 *
 * Last Modified: 00/09/26             Created: 00/08/30
 *
 * Project    : Nessie Proposal: NOEKEON
 *
 * Authors    : Joan Daemen, Michael Peeters, Vincent Rijmen, Gilles Van Assche
 *
 * Written by : Michael Peeters
 *
 * References : [NESSIE] see http://cryptonessie.org/ for information about 
 *                       interface conventions and definition of portable C.
 *
 * Description: Generate test vector for block cipher NOEKEON
 *
 ***********************************************************************************/

#include <stdio.h>
#include "NessieInterfaces.h"
#include "Nessie.h"

char *printVector8 (char * const s, u8 const * const v)
{
  sprintf (s,"%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x",
           v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],v[11],v[12],v[13],v[14],v[15]);
  return s;
}

char *printVector32 (char * const s, u32 const * const v)
{
  sprintf (s,"%08x %08x %08x %08x",v[0],v[1],v[2],v[3]);
  return s;
}

int main (int argc, char *argv[])
{
  u8 *k;
  u8 k0[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  u8 k1[16] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
  u8 k2sched [16] = {0xba,0x69,0x33,0x81,0x92,0x99,0xc7,0x16,0x99,0xa9,0x9f,0x08,0xf6,0x78,0x17,0x8b};
  u8 k2direct[16] = {0xb1,0x65,0x68,0x51,0x69,0x9e,0x29,0xfa,0x24,0xb7,0x01,0x48,0x50,0x3d,0x2d,0xfc};
  u8 *a;
  u8 a0[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  u8 a1[16] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
  u8 a2sched [16] = {0x52,0xf8,0x8a,0x7b,0x28,0x3c,0x1f,0x7b,0xdf,0x7b,0x6f,0xaa,0x50,0x11,0xc7,0xd8};
  u8 a2direct[16] = {0x2a,0x78,0x42,0x1b,0x87,0xc7,0xd0,0x92,0x4f,0x26,0x11,0x3f,0x1d,0x13,0x49,0xb2};
  u8 b[16];
  u8 c[16];
  struct NESSIEstruct keystruct;
  char outs[200];

  a=a0; k=k0;
  printf ("                     k = %s\n",printVector8(outs,k));
  printf ("                     a = %s\n",printVector8(outs,a));
  NESSIEkeysetup (k,&keystruct);
  NESSIEencrypt (&keystruct,a,b);
  printf ("after NESSIEencrypt, b = %s\n",printVector8(outs,b));
  NESSIEdecrypt (&keystruct,b,c);
  printf ("after NESSIEdecrypt, a?= %s\n",printVector8(outs,c));
  printf ("\n");

  a=a1; k=k1;
  printf ("                     k = %s\n",printVector8(outs,k));
  printf ("                     a = %s\n",printVector8(outs,a));
  NESSIEkeysetup (k,&keystruct);
  NESSIEencrypt (&keystruct,a,b);
  printf ("after NESSIEencrypt, b = %s\n",printVector8(outs,b));
  NESSIEdecrypt (&keystruct,b,c);
  printf ("after NESSIEdecrypt, a?= %s\n",printVector8(outs,c));
  printf ("\n");

  /* select either a2sched/k2sched or a2direct/k2direct */
  a=a2direct; k=k2direct;
  printf ("                     k = %s\n",printVector8(outs,k));
  printf ("                     a = %s\n",printVector8(outs,a));
  NESSIEkeysetup (k,&keystruct);
  NESSIEencrypt (&keystruct,a,b);
  printf ("after NESSIEencrypt, b = %s\n",printVector8(outs,b));
  NESSIEdecrypt (&keystruct,b,c);
  printf ("after NESSIEdecrypt, a?= %s\n",printVector8(outs,c));
  printf ("\n");

  return 0;
}
