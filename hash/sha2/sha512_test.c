

// SHA-512 in C test unit
// Odzhan

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <time.h>

#include "sha2.h"

char *text[] =
{ "",
  "a",
  "abc",
  "message digest",
  "abcdefghijklmnopqrstuvwxyz",
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
  "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
};

char *SHA512_dgst[] =
{ "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e",
  "1f40fc92da241694750979ee6cf582f2d5d7d28e18335de05abc54d0560e0f5302860c652bf08d560252aa5e74210546f369fbbbce8c12cfc7957b2652fe9a75",
  "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f",
  "107dbf389d9e9f71a3a95f6c055b9251bc5268c2be16d6c13492ea45b0199f3309e16455ab1e96118e8a905d5597b72038ddb372a89826046de66687bb420e7c",
  "4dbff86cc2ca1bae1e16468a05cb9881c97f1753bce3619034898faa1aabe429955a1bf8ec483d7421fe3c1646613a59ed5441fb0f321389f77f48a879c7b1f1",
  "1e07be23c26a86ea37ea810c8ec7809352515a970e9253c26f536cfc7a9996c45c8370583e0a78fa4a90041d71a4ceab7423f19c71b9d5a3e01249f0bebd5894",
  "72ec1ef1124a45b047e8b7c75a932195135bb61de24ec0d1914042246e0aec3a2354e093d76f3048b456764346900cb130d2a4fd5dd16abb5e30bcb850dee843"
};

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

int run_tests (void)
{
  uint8_t    dgst[SHA512_DIGEST_LENGTH], tv[SHA512_DIGEST_LENGTH];
  int        i, fails=0;
  SHA512_CTX ctx;
  
  for (i=0; i<sizeof(text)/sizeof(char*); i++)
  {
    SHA512_Init (&ctx);
    SHA512_Update (&ctx, text[i], strlen(text[i]));
    SHA512_Final (dgst, &ctx);
    
    hex2bin (tv, SHA512_dgst[i]);
    
    if (memcmp (dgst, tv, SHA512_DIGEST_LENGTH) != 0) {
      printf ("\nFailed for string: %s", text[i]);
      ++fails;
    }
  }
  return fails;
}

/************************************************
*
* HMAC
*
************************************************/
void SHA512_HMAC (void *text, size_t text_len, 
  void *key, size_t key_len, void* dgst)
{
  SHA512_CTX ctx;
  uint8_t    k_ipad[SHA512_CBLOCK], k_opad[SHA512_CBLOCK], tk[SHA512_DIGEST_LENGTH];
  size_t     i;
  uint8_t    *k=(uint8_t*)key;

  if (key_len > SHA512_CBLOCK) {
    SHA512_Init (&ctx);
    SHA512_Update (&ctx, key, key_len);
    SHA512_Final (tk, &ctx);

    key = tk;
    key_len = SHA512_DIGEST_LENGTH;
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
  SHA512_Init (&ctx);                   /* init context for 1st pass */
  SHA512_Update (&ctx, k_ipad, SHA512_CBLOCK);     /* start with inner pad */
  SHA512_Update (&ctx, text, text_len); /* then text of datagram */
  SHA512_Final (dgst, &ctx);            /* finish up 1st pass */
  /**
  * perform outer
  */
  SHA512_Init (&ctx);                   /* init context for 2nd pass */
  SHA512_Update (&ctx, k_opad, SHA512_CBLOCK);     /* start with outer pad */
  SHA512_Update (&ctx, dgst, SHA512_DIGEST_LENGTH); /* then results of 1st hash */
  SHA512_Final (dgst, &ctx);            /* finish up 2nd pass */
}

/**
* Password-Based Key Derivation Function 2 (PKCS #5 v2.0).
* Code based on IEEE Std 802.11-2007, Annex H.4.2.
*/

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

int SHA512_PBKDF2 (void *pwd, size_t pwd_len, void *salt, size_t salt_len,
uint32_t rounds, void *key, size_t key_len)
{
  uint8_t *asalt, obuf[SHA512_DIGEST_LENGTH];
  uint8_t *k=(uint8_t*)key;
  uint8_t d1[SHA512_DIGEST_LENGTH], d2[SHA512_DIGEST_LENGTH];
  size_t i, j, count, r;

  if ((asalt = (uint8_t*)malloc(salt_len + 4)) == 0)
  return -1;

  memcpy(asalt, salt, salt_len);

  for (count = 1; key_len > 0; count++) {
    asalt[salt_len + 0] = (count >> 24) & 0xff;
    asalt[salt_len + 1] = (count >> 16) & 0xff;
    asalt[salt_len + 2] = (count >>  8) & 0xff;
    asalt[salt_len + 3] = count & 0xff;
    SHA512_HMAC(asalt, salt_len + 4, pwd, pwd_len, d1);
    memcpy(obuf, d1, sizeof(obuf));

    for (i = 1; i < rounds; i++) {
      SHA512_HMAC(d1, sizeof(d1), pwd, pwd_len, d2);
      memcpy(d1, d2, sizeof(d1));
      for (j = 0; j < sizeof(obuf); j++)
      obuf[j] ^= d1[j];
    }

    r = MIN(key_len, SHA512_DIGEST_LENGTH);
    memcpy(k, obuf, r);
    k += r;
    key_len -= r;
  };
  free (asalt);
  return 0;
}

// print digest
void SHA512_print (uint8_t dgst[], size_t len)
{
  size_t i;
  for (i=0; i<len; i++) {
    printf ("%02x", dgst[i]);
  }
  putchar ('\n');
}

// generate SHA-3 hash of string
void SHA512_string (char *str)
{
  SHA512_CTX ctx;
  uint8_t  dgst[256];
  
  memset (dgst, 0, sizeof (dgst));
  printf ("\nSHA2-512(\"%s\")\n0x", str);
  
  SHA512_Init (&ctx);
  SHA512_Update (&ctx, str, strlen (str));
  SHA512_Final (dgst, &ctx);
  
  SHA512_print (dgst, SHA512_DIGEST_LENGTH);
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
  printf ("\rProcessed %llu MB out of %llu MB %llu MB/s : %llu%% complete. ETA: %03d:%02d:%02d:%02d    ",
    fs_complete/1000/1000, fs_total/1000/1000, speed/1000/1000, pct, days, hours, minutes, (int)seconds);
  }
}

// generate SHA-3 hash of file
void SHA512_file (char fn[])
{
  FILE     *fd;
  SHA512_CTX ctx;
  size_t   len;
  uint8_t  buf[BUFSIZ], dgst[256];
  struct stat st;
  uint32_t cmp=0, total=0;
  
  fd = fopen (fn, "rb");
  
  if (fd!=NULL)
  {
    stat (fn, &st);
    total=st.st_size;
    
    SHA512_Init (&ctx);
    
    while (len = fread (buf, 1, BUFSIZ, fd)) {
      cmp += len;
      if (total > 10000000 && (cmp % 10000000)==0 || cmp==total) {
        progress (cmp, total);
      }
      SHA512_Update (&ctx, buf, len);
    }
    SHA512_Final (dgst, &ctx);

    fclose (fd);

    printf ("\n  [ SHA2-512 (%s) = ", fn);
    SHA512_print (dgst, SHA512_DIGEST_LENGTH);
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
  printf ("\n  usage: SHA512_test -t <type> -f <file> -s <string>\n");
  printf ("\n  -k <type>   Key for HMAC");
  printf ("\n  -s <string> Derive SHA2-256 hash of <string>");
  printf ("\n  -f <file>   Derive SHA2-256 hash of <file>");
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
    SHA512_string (str);
  } else if (file!=NULL || wc!=0) {
    if (wc!=0) {
      while (argv[wc]!=NULL) {
        SHA512_file (argv[wc++]);
      }
    } else {
      SHA512_file (file);
    }
  } else {
    usage ();
  }
  return 0;
}
