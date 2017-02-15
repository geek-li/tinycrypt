

// SHA1 in C test unit
// Odzhan

#include "sha1.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
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

char *SHA1_dgst[] =
{ "da39a3ee5e6b4b0d3255bfef95601890afd80709",
  "86f7e437faa5a7fce15d1ddcb9eaeaea377667b8",
  "a9993e364706816aba3e25717850c26c9cd0d89d",
  "c12252ceda8be8994d5fa0290a47231c1d16aae3",
  "32d10c7b8cf96570ca04ce37f2a19d84240d3a89",
  "761c457bf73b14d27e9e9265c46f4b4dda11f940",
  "50abf5706a150990a08b2c5ea40fa0e585554732"
};

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

void SHA1_HMAC (void *text, size_t text_len, 
void *key, size_t key_len, void* dgst)
{
  SHA1_CTX ctx;
  uint8_t  k_ipad[SHA1_CBLOCK], k_opad[SHA1_CBLOCK], tk[SHA1_DIGEST_LENGTH];
  uint32_t i;
  uint8_t *k=(uint8_t*)key;

  if (key_len > SHA1_CBLOCK) {
    SHA1_Init (&ctx);
    SHA1_Update (&ctx, key, key_len);
    SHA1_Final (tk, &ctx);

    key = tk;
    key_len = SHA1_DIGEST_LENGTH;
  }

  memset (k_ipad, 0x36, sizeof (k_ipad));
  memset (k_opad, 0x5c, sizeof (k_opad));

  /** XOR key with ipad and opad values */
  for (i=0; i<key_len; i++) {
    k_ipad[i] ^= k[i];
    k_opad[i] ^= k[i];
  }
  /**
    * perform inner 
    */
  SHA1_Init (&ctx);                             /* init context for 1st pass */
  SHA1_Update (&ctx, k_ipad, SHA1_CBLOCK);     /* start with inner pad */
  SHA1_Update (&ctx, text, text_len);           /* then text of datagram */
  SHA1_Final (dgst, &ctx);                      /* finish up 1st pass */
  /**
    * perform outer
    */
  SHA1_Init (&ctx);                             /* init context for 2nd pass */
  SHA1_Update (&ctx, k_opad, SHA1_CBLOCK);     /* start with outer pad */
  SHA1_Update (&ctx, dgst, SHA1_DIGEST_LENGTH);      /* then results of 1st hash */
  SHA1_Final (dgst, &ctx);                      /* finish up 2nd pass */
}

/*
* Password-Based Key Derivation Function 2 (PKCS #5 v2.0).
* Code based on IEEE Std 802.11-2007, Annex H.4.2.
*/

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

int SHA1_PBKDF2 (void *pwd, size_t pwd_len, void *salt, size_t salt_len,
uint32_t rounds, void *key, size_t key_len)
{
  uint8_t *asalt, obuf[SHA1_DIGEST_LENGTH];
  uint8_t d1[SHA1_DIGEST_LENGTH], d2[SHA1_DIGEST_LENGTH];
  uint32_t i, j, count;
  size_t r;
  uint8_t *k=(uint8_t*)key;

  if ((asalt = (uint8_t*)malloc(salt_len + 4)) == NULL)
  return -1;

  memcpy(asalt, salt, salt_len);

  for (count = 1; key_len > 0; count++) {
    asalt[salt_len + 0] = (count >> 24) & 0xff;
    asalt[salt_len + 1] = (count >> 16) & 0xff;
    asalt[salt_len + 2] = (count >> 8) & 0xff;
    asalt[salt_len + 3] = count & 0xff;
    
    SHA1_HMAC(asalt, salt_len + 4, pwd, pwd_len, d1);
    memcpy(obuf, d1, sizeof(obuf));

    for (i = 1; i < rounds; i++) {
      SHA1_HMAC(d1, sizeof(d1), pwd, pwd_len, d2);
      memcpy(d1, d2, sizeof(d1));
      for (j = 0; j < sizeof(obuf); j++)
      obuf[j] ^= d1[j];
    }

    r = MIN(key_len, SHA1_DIGEST_LENGTH);
    memcpy(k, obuf, r);
    k += r;
    key_len -= r;
  };
  free(asalt);
  return 0;
}

int run_tests (void)
{
  uint8_t dgst[SHA1_DIGEST_LENGTH+1], tv[SHA1_DIGEST_LENGTH+1];
  int     i, fails=0;
  SHA1_CTX ctx;
  
  for (i=0; i<sizeof(text)/sizeof(char*); i++)
  {
    SHA1_Init (&ctx);
    SHA1_Update (&ctx, text[i], strlen(text[i]));
    SHA1_Final (dgst, &ctx);
    
    hex2bin (tv, SHA1_dgst[i]);
    
    if (memcmp (dgst, tv, SHA1_DIGEST_LENGTH) != 0) {
      printf ("\nFailed for string : \"%s\"", text[i]);
      ++fails;
    }
  }
  return fails;
}

// print digest
void SHA1_print (uint8_t dgst[], size_t len)
{
  size_t i;
  for (i=0; i<len; i++) {
    printf ("%02x", dgst[i]);
  }
  putchar ('\n');
}

// generate SHA-3 hash of string
void SHA1_string (char *str)
{
  SHA1_CTX ctx;
  size_t   i;
  uint8_t  dgst[256];

  printf ("\nSHA1(\"%s\")\n0x", str);
  
  SHA1_Init (&ctx);
  SHA1_Update (&ctx, str, strlen (str));
  SHA1_Final (dgst, &ctx);
  
  SHA1_print (dgst, SHA1_DIGEST_LENGTH);
}

void progress (uint64_t fs_complete, uint64_t fs_total)
{
  uint32_t total, hours=0, minutes=0, seconds=0, speed=0, avg;
  uint64_t pct;
  static uint32_t start=0, current;
  
  if (start==0) {
    start=time(0);
    return;
  }
  
  pct = (100 * fs_complete) / (1 * fs_total);
  
  total = (time(0) - start);
  
  if (total != 0) {
    // (remaining data * time elapsed) / data completed
    avg = (total * (fs_total - fs_complete)) / fs_complete;
    speed = (fs_complete / total);
    
    minutes = (avg / 60);
    seconds = (avg % 60);
  }
  printf ("\rProcessed %llu MB out of %llu MB %lu MB/s : %llu%% complete. ETA: %02d:%02d   ",
    fs_complete/1000/1000, fs_total/1000/1000, speed/1000/1000, pct, minutes, seconds);
}

// generate SHA-3 hash of file
void SHA1_file (char fn[])
{
  FILE     *fd;
  SHA1_CTX ctx;
  size_t   len;
  uint8_t  buf[BUFSIZ], dgst[256];
  struct stat st;
  uint32_t cmp=0, total=0;
  
  fd = fopen (fn, "rb");
  
  if (fd!=NULL)
  {
    stat (fn, &st);
    total=st.st_size;
    
    SHA1_Init (&ctx);
    
    while (len = fread (buf, 1, BUFSIZ, fd)) {
      cmp += len;
      if (cmp > 10000000 && (cmp % 10000000)==0 || cmp==total) {
        progress (cmp, total);
      }
      SHA1_Update (&ctx, buf, len);
    }
    
    SHA1_Final (dgst, &ctx);

    fclose (fd);

    printf ("\n  [ SHA1 (%s) = ", fn);
    SHA1_print (dgst, SHA1_DIGEST_LENGTH);
  } else {
    printf ("  [ unable to open %s\n", fn);
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
  
  printf ("\n  usage: SHA1_test -t <type> -f <file> -s <string>\n");
  printf ("\n  -k <type>   Key for HMAC");
  printf ("\n  -s <string> Derive SHA1 hash of <string>");
  printf ("\n  -f <file>   Derive SHA1 hash of <file>");
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
    // if this is path, set wildcard to true
    if (wc==0) {
      wc=i;
    }
  }
  
  if (test) {
    if (!run_tests()) {
      printf ("\n  [ self-test OK!\n");
    }
  } else if (str!=NULL) {
    SHA1_string (str);
  } else if (file!=NULL || wc!=0) {
    if (wc!=0) {
      while (argv[wc]!=NULL) {
        SHA1_file (argv[wc++]);
      }
    } else {
      SHA1_file (file);
    }
  } else {
    usage ();
  }
  return 0;
}
