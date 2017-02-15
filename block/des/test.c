

// DES in C
// Odzhan

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

char *test_keys[] = 
{ "8000000000000000",
  "0001020304050607",
  "2BD6459F82C5B300",
  "A6A6A6A6A6A6A6A6",
  "9292929292929292" };

char *test_pt[] =
{ "0000000000000000",
  "41AD068548809D02",
  "B10F843097A0F932",
  "11DE2BCE0CB1765A",
  "9292929292929292" };
            
char *test_ct[] =
{ "95A8D72813DAA94D",
  "0011223344556677",
  "EA024714AD5C4D84",
  "A6A6A6A6A6A6A6A6",
  "5B365F2FB2CD7F32" };
  
#ifndef OSSL
#include "des.h"
#else
#include <openssl/des.h>

#pragma comment (lib, "user32.lib")
#pragma comment (lib, "advapi32.lib")

void des_enc (uint8_t out[], uint8_t in[], uint8_t key[])
{
  DES_key_schedule ks;
  
  DES_set_key((const_DES_cblock*)key, &ks);
  DES_ecb_encrypt((const_DES_cblock*) in, (DES_cblock*)out, &ks, DES_ENCRYPT);
}

void des_dec (uint8_t out[], uint8_t in[], uint8_t key[])
{
  DES_key_schedule ks;
  
  DES_set_key((const_DES_cblock*)key, &ks);
  DES_ecb_encrypt((const_DES_cblock*) in, (DES_cblock*)out, &ks, DES_DECRYPT);
}
#endif

// generate Lanman hash
void lanman (uint8_t *lmhash, uint8_t *pwd)
{
  uint8_t lmpwd[32];
  uint8_t i;
  des_blk key1, key2;
  size_t  len=strlen(pwd);
  des_ctx ctx;
  
  memset (lmhash, 0, 16);
  
  // MS Lanman passwords don't exceed 14 characters
  len=(len>14) ? 14 : len;
  
  for (i=0; i<14; i++) {
    lmpwd[i]=(i<len) ? toupper (pwd[i]) : 0;
  }

  des_str2key (&lmpwd[0], &key1);
  des_str2key (&lmpwd[7], &key2);

  des_setkey (&ctx, &key1);
  des_enc (&ctx, "KGS!@#$%", (des_blk*)&lmhash[0], DES_ENCRYPT);
  
  des_setkey (&ctx, &key2);
  des_enc (&ctx, "KGS!@#$%", (des_blk*)&lmhash[8], DES_ENCRYPT);
}
  
size_t hex2bin (void *bin, char hex[]) {
  size_t len, i;
  int x;
  uint8_t *p=(uint8_t*)bin;
  
  len = strlen (hex);
  
  if ((len & 1) != 0) {
    return 0; 
  }
  
  for (i=0; i<len; i++) {
    if (isxdigit((int)hex[i]) == 0) {
      return 0; 
    }
  }
  
  for (i=0; i<len / 2; i++) {
    sscanf (&hex[i * 2], "%2x", &x);
    p[i] = (uint8_t)x;
  } 
  return len / 2;
} 

int run_tests (void)
{
  int i, j, plen, clen, klen, fails=0;

  des_blk ct1, ct2, pt1, pt2, key;
  des_ctx ctx;
  
  for (i=0; i<sizeof (test_keys)/sizeof(char*); i++)
  { 
    klen=hex2bin (key.v8, test_keys[i]);
    clen=hex2bin (ct1.v8, test_ct[i]);
    plen=hex2bin (pt1.v8, test_pt[i]);
    
    //des_enc (ct2.v8, pt1.v8, key.v8);
    des_setkey(&ctx, key.v8);
    /*for (j=0; j<4; j++)
    {
      printf ("\n%08X %08X", ctx.keys[i].v32[0], ctx.keys[i].v32[1]);
    }*/
    
    des_enc (&ctx, pt1.v8, ct2.v8, DES_ENCRYPT);
    //des_enc (&ctx, ct2.v8, pt2.v8, DES_DECRYPT);
    
    if (memcmp (ct1.v8, ct2.v8, clen)==0) {
      printf ("\nPassed Encryption/Decryption test #%i %08X %08X", 
        (i+1), ct1.v32[0], ct2.v32[0]);
    } else {
      fails++;
      printf ("\nFailed test #%i : "
          "Got %08X %08X instead of %08X %08X for %08X %08X", (i+1), 
          ct2.v32[0], ct2.v32[1], ct1.v32[0], ct1.v32[1],
          pt1.v32[0], pt1.v32[1]);
          
    }
  }
  return fails;
}

void progress (uint64_t fs_complete, uint64_t fs_total)
{
  int           days=0, hours=0, minutes=0;
  uint64_t      t, pct, speed, seconds=0;
  static time_t start=0;
  
  if (start==0) {
    start=time(0);
    return;
  }
  
  pct = (100*fs_complete)/fs_total;
  
  t = (time(0) - start);
  
  if (t != 0) {
    seconds = (fs_total - fs_complete) / (fs_complete / t);
    speed   = (fs_complete / t);
    
    days=0;
    hours=0;
    minutes=0;
    
    if (seconds>=60) {
      minutes = (seconds / 60);
      seconds %= 60;
      if (minutes>=60) {
        hours = minutes / 60;
        minutes %= 60;
        if (hours>=24) {
          days = hours/24;
          hours %= 24;
        }
      }
    }
  printf ("\rProcessed %llu MB out of %llu MB at %llu MB/s : %llu%% complete. ETA: %03d:%02d:%02d:%02d",
    fs_complete/1000/1000, fs_total/1000/1000, speed/1000/1000, pct, days, hours, minutes, (int)seconds);
  }
}

void DES_genkey (void *out, char *str) 
{
  //des_str2key (str, out);
}

// generate DES hash of file
void DES_file (char infile[], char outfile[], char *key, int crypt)
{
  FILE        *in, *out;
  des_blk     ctx;
  size_t      len;
  uint8_t     buf_in[DES_BLK_LEN*32+64];
  uint8_t     buf_out[DES_BLK_LEN*32+64];
  uint8_t     k[DES_BLK_LEN], iv[DES_BLK_LEN];
  struct stat st;
  uint32_t    cmp=0, total=0, dec_len=0, pad_len, last=0;
  
  in = fopen (infile, "rb");
  
  if (in!=NULL)
  {
    out = fopen (outfile, "wb");
    
    if (out!=NULL)
    {
      stat (infile, &st);
      total=st.st_size;
    
      memset (iv,      0, sizeof (iv));
      memset (buf_in,  0, sizeof (buf_in));
      memset (buf_out, 0, sizeof (buf_out));
      
      DES_genkey (&ctx, key);

      while (len = fread (buf_in, 1, DES_BLK_LEN*32, in)) {
        cmp += len;
        if (cmp > 1000000 && (cmp % 1000000)==0 || cmp==total) {
          progress (cmp, total);
        }//
        // if encrypting, pad
        if (len < DES_BLK_LEN*32) 
        {
          last=1;
          if (crypt==DES_ENCRYPT) 
          {
            memset (&buf_in[len], 0, DES_BLK_LEN*32 - len);
            pad_len=(len % DES_BLK_LEN);
            if (pad_len != 0) 
            {
              dec_len=DES_BLK_LEN - pad_len;

              while (pad_len++ < DES_BLK_LEN) {
                printf ("\npadding");
                buf_in[len++] = dec_len;
              }
            }
          }
        }
        if (crypt==DES_ENCRYPT) {
          //des_cbc_enc (&ctx, buf_in, buf_out, len, iv);
        } else {
          //des_cbc_dec (&ctx, buf_in, buf_out, len, iv);
          if (last && buf_out[len-1] < DES_BLK_LEN) {
            printf ("\nundoing");
            dec_len=(uint8_t)buf_out[len-1];
            len-= dec_len;
          }
        }
        fwrite (buf_out, 1, len, out);
      }
      fclose (out);
    } else {
      perror ("fopen()");
    }
    fclose (in);
  } else {
    perror ("fopen()");
  }
}

char* getparam (int argc, char *argv[], int *i)
{
  int n=*i;
  if (argv[n][2] != 0) {
    return &argv[n][2];
  }
  if ((n+1) < argc) {
    *i=n+1;
    return argv[n+1];
  }
  printf ("  [ %c%c requires parameter\n", argv[n][0], argv[n][1]);
  exit (0);
}

void usage (void)
{
  int i;
  
  printf ("\n  usage: DES_test -k <key> -i <file> -o <file>\n");
  printf ("\n  -k <key>    Key");
  printf ("\n  -i <file>   Input file");
  printf ("\n  -o <file>   Output file");
  printf ("\n  -e          Encrypt");
  printf ("\n  -d          Decrypt");
  printf ("\n  -x          Run tests");
  printf ("\n  -l <pwd>    Create Lanman hash\n");
  exit (0);
}

int main (int argc, char *argv[])
{
  char opt;
  int i, test=0, crypt=DES_ENCRYPT;
  char *in=NULL, *out=NULL, *pwd="password";
  uint8_t lm[32];
  
  // for each argument
  for (i=1; i<argc; i++)
  {
    // is this option?
    if (argv[i][0]=='-' || argv[i][1]=='/')
    {
      // get option value
      opt=argv[i][1];
      switch (opt)
      {
        case 'i':
          in=getparam (argc, argv, &i);
          break;
        case 'd':
          crypt=DES_DECRYPT;
          break;
        case 'e':
          crypt=DES_ENCRYPT;
          break;
        case 'o':
          out=getparam (argc, argv, &i);
          break;
        case 'k':
          pwd=getparam (argc, argv, &i);
          break;
        case 'x':
          test=1;
          break;
        case 'l': {
          pwd=getparam(argc, argv, &i);
          lanman (lm, pwd);
          printf ("\nMS Lanman = ");
          for (i=0; i<16; i++) {
            printf ("%02X", lm[i]);
          }
          return 0;
        }
        default:
          usage ();
          break;
      }
    }
  }
  
  if (test) {
    if (!run_tests()) {
      printf ("\n  [ self-test OK!\n");
    }
  } else if (in!=NULL && out!=NULL && pwd!=NULL) {
    DES_file (in, out, pwd, crypt);
  } else {
    usage ();
  }
  return 0;
}
