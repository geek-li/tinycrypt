

// BLAKE2s test unit in C
// Odzhan

#include "b2s.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

char *text[] =
{ "",
  "a",
  "abc",
  "message digest",
  "abcdefghijklmnopqrstuvwxyz",
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
  "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
};

char *BLAKE2s_dgst[] =
{ "69217a3079908094e11121d042354a7c1f55b6482ca1a51e1b250dfd1ed0eef9",
  "4a0d129873403037c2cd9b9048203687f6233fb6738956e0349bd4320fec3e90",
  "508c5e8c327c14e2e1a72ba34eeb452f37458b209ed63a294d999b4c86675982",
  "fa10ab775acf89b7d3c8a6e823d586f6b67bdbac4ce207fe145b7d3ac25cd28c",
  "bdf88eb1f86a0cdf0e840ba88fa118508369df186c7355b4b16cf79fa2710a12",
  "c75439ea17e1de6fa4510c335dc3d3f343e6f9e1ce2773e25b4174f1df8b119b",
  "fdaedb290a0d5af9870864fec2e090200989dc9cd53a3c092129e8535e8b4f66"
};

char *BLAKE2s_tv[] =
{ "",
  "00",
  "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2021222324252627"
  "28292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f"
  "505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f7071727374757677"
  "78797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9f"
  "a0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7"
  "c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeef"
  "f0f1f2f3f4f5f6f7f8f9fafbfcfdfe"};

char *BLAKE2s_key[] =
{ "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f",
  "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f",
  "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"};

char *BLAKE2s_res[] =
{ "48a8997da407876b3d79c0d92325ad3b89cbb754d86ab71aee047ad345fd2c49",
  "40d15fee7c328830166ac3f918650f807e7e01e177258cdc0a39b11f598066f1",
  "3fb735061abc519dfe979e54c1ee5bfad0a9d858b3315bad34bde999efd724ddok"};

size_t hex2bin (void *bin, char hex[]) {
  size_t  len, i;
  int     x;
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

uint32_t run_tests (void)
{
  uint8_t     dgst[BLAKE2s_DIGEST_LENGTH+1], tv[BLAKE2s_DIGEST_LENGTH+1];
  uint32_t    i, fails=0;
  b2s_ctx     ctx;
  
  for (i=0; i<sizeof(text)/sizeof(char*); i++)
  {
    b2s_init (&ctx, BLAKE2s_DIGEST_LENGTH, NULL, 0, 0);
    b2s_update (&ctx, text[i], strlen(text[i]));
    b2s_final (dgst, &ctx);
    
    hex2bin (tv, BLAKE2s_dgst[i]);
    
    if (memcmp (dgst, tv, BLAKE2s_DIGEST_LENGTH) != 0) {
      printf ("\nFailed for string len %u: %s", 
        strlen(text[i]), text[i]);
      ++fails;
    }
  }
  return fails;
}

uint32_t run_xtests (void)
{
  uint8_t input[1024], key[32], res[32], dgst[32];
  uint32_t keylen, inlen, outlen;
  
  uint32_t i, fails=0;
  b2s_ctx  ctx;
  
  for (i=0; i<sizeof(BLAKE2s_tv)/sizeof(char*); i++)
  {
    inlen  = hex2bin (input, BLAKE2s_tv[i]);
    keylen = hex2bin (key, BLAKE2s_key[i]);
    outlen = hex2bin (res, BLAKE2s_res[i]);
    
    b2s_init (&ctx, outlen, key, keylen, 0);
    b2s_update (&ctx, input, inlen);
    b2s_final (dgst, &ctx);
    
    if (memcmp (dgst, res, outlen) != 0) {
      printf ("\nFailed for string: %s", BLAKE2s_tv[i]);
      ++fails;
    }
  }
  return fails;
}

// print digest
void BLAKE2s_print (uint8_t dgst[], size_t len)
{
  size_t i;
  for (i=0; i<len; i++) {
    printf ("%02x", dgst[i]);
  }
  putchar ('\n');
}

// generate BLAKE2s hash of string
void BLAKE2s_string (char *str, char *key)
{
  b2s_ctx ctx;
  uint8_t     dgst[256];

  printf ("\nBLAKE2s(\"%s\")\n0x", str);
  
  b2s_init (&ctx, BLAKE2s_DIGEST_LENGTH, key, key!=NULL ? strlen (key) : 0, 0);
  b2s_update (&ctx, str, strlen (str));
  b2s_final (dgst, &ctx);
  
  BLAKE2s_print (dgst, BLAKE2s_DIGEST_LENGTH);
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

// generate BLAKE2s hash of file
void BLAKE2s_file (char fn[], char *key)
{
  FILE        *fd;
  b2s_ctx ctx;
  size_t      len;
  uint8_t     buf[BUFSIZ], dgst[256];
  struct stat st;
  uint32_t    cmp=0, total=0;
  
  fd = fopen (fn, "rb");
  
  if (fd!=NULL)
  {
    stat (fn, &st);
    total=st.st_size;
    
    b2s_init (&ctx, BLAKE2s_DIGEST_LENGTH, key, key!=NULL ? strlen (key) : 0, 0);
    
    while ((len = fread (buf, 1, BUFSIZ, fd))) {
      cmp += len;
      if ((cmp > 10000000) && ((cmp % 10000000)==0 || cmp==total)) {
        progress (cmp, total);
      }
      b2s_update (&ctx, buf, len);
    }
    b2s_final (dgst, &ctx);

    fclose (fd);

    printf ("\n  [ BLAKE2s (%s) = ", fn);
    BLAKE2s_print (dgst, BLAKE2s_DIGEST_LENGTH);
  } else {
    printf ("  [ unable to open %s\n", fn);
  }
}

void b2s_hash (void *input, uint32_t inlen, 
	void *output, uint32_t outlen)
{
	b2s_ctx ctx;
	uint8_t dgst[BLAKE2s_DIGEST_LENGTH];
	
	outlen=(outlen==0) ? 32 : outlen;
	
	b2s_init (&ctx, BLAKE2s_DIGEST_LENGTH, NULL, 0, 0);
	b2s_update (&ctx, input, inlen);
	b2s_final (dgst, &ctx);
	
	memcpy (output, dgst, outlen);
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
  printf ("\n  usage: b2s_test -t <type> -f <file> -s <string>\n");
  printf ("\n  -k <type>   Key for MAC");
  printf ("\n  -s <string> Derive BLAKE2s hash of <string>");
  printf ("\n  -f <file>   Derive BLAKE2s hash of <file>");
  printf ("\n  -x          Run tests\n");
  exit (0);
}

int main (int argc, char *argv[])
{
  char opt;
  int i, test=0, wc=0;
  char *file=NULL, *str=NULL, *key=NULL;
  
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
        case 's':
          str=getparam (argc, argv, &i);
          break;
        case 'f':
          file=getparam (argc, argv, &i);
          break;
        case 'k':
          key=getparam (argc, argv, &i);
          break;
        case 'x':
          test=1;
          break;
        default:
          usage ();
          break;
      }
    }
    if (wc==0) {
      wc=i;
    }
  }
  
  if (test) {
    if (!run_tests()) {
      printf ("\n  [ unkeyed self-test OK!\n");
    }
    if (!run_xtests()) {
      printf ("\n  [ keyed self-test OK!\n");
    }
  } else if (str!=NULL) {
    BLAKE2s_string (str, key);
  } else if (file!=NULL || wc!=0) {
    if (wc!=0) {
      while (argv[wc]!=NULL) {
        BLAKE2s_file (argv[wc++], key);
      }
    } else {
      BLAKE2s_file (file, key);
    }
  } else {
    usage ();
  }
  return 0;
}