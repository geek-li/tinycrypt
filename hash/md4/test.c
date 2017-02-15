

// Test unit for MD4 in C
// Odzhan

#include "md4.h"

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

char *md4_dgst[] =
{ "31d6cfe0d16ae931b73c59d7e0c089c0",
  "bde52cb31de33e46245e05fbdbd6fb24",
  "a448017aaf21d8525fc10ae87aa6729d",
  "d9130a8164549fe818874806e1c7014b",
  "d79e1c308aa5bbcdeea8ed63df412da9",
  "043f8582f241db351ce627e153e7f0e4",
  "e33b4ddc9c38f2199c3e7b164fcc0536"
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

/************************************************
*
* HMAC
*
************************************************/
void MD4_HMAC (void *text, size_t text_len, 
void *key, size_t key_len, void *dgst)
{
  MD4_CTX ctx;
  uint8_t k_ipad[64], k_opad[64], tk[MD4_DIGEST_LENGTH];
  size_t  i;
  uint8_t *k=(uint8_t*)key;

  if (key_len > 64) {
    MD4_Init (&ctx);
    MD4_Update (&ctx, key, key_len);
    MD4_Final (tk, &ctx);

    k = tk;
    key_len = MD4_DIGEST_LENGTH;
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
  MD4_Init (&ctx);                       /* init context for 1st pass */
  MD4_Update (&ctx, k_ipad, 64);         /* start with inner pad */
  MD4_Update (&ctx, text, text_len);     /* then text of datagram */
  MD4_Final (dgst, &ctx);                /* finish up 1st pass */
  /**
  * perform outer
  */
  MD4_Init (&ctx);                       /* init context for 2nd pass */
  MD4_Update (&ctx, k_opad, 64);         /* start with outer pad */
  MD4_Update (&ctx, dgst, MD4_DIGEST_LENGTH); /* then results of 1st hash */
  MD4_Final (dgst, &ctx);                /* finish up 2nd pass */
}

void MD4 (void *in, size_t len, void *out)
{
  MD4_CTX ctx;
  
  MD4_Init (&ctx);
  MD4_Update (&ctx, in, len);
  MD4_Final (out, &ctx);
}

int run_tests (void)
{
  uint8_t dgst[MD4_DIGEST_LENGTH], tv[MD4_DIGEST_LENGTH];
  int     i, fails=0;
  MD4_CTX ctx;
  
  for (i=0; i<sizeof(text)/sizeof(char*); i++)
  {
    MD4_Init (&ctx);
    MD4_Update (&ctx, text[i], strlen(text[i]));
    MD4_Final (dgst, &ctx);
    
    hex2bin (tv, md4_dgst[i]);
    
    if (memcmp (dgst, tv, MD4_DIGEST_LENGTH) != 0) {
      printf ("\nFailed for \"%s\"", text[i]);
      ++fails;
    }
  }
  return fails;
}

// print digest
void MD4_print (uint8_t dgst[], size_t len)
{
  size_t i;
  for (i=0; i<len; i++) {
    printf ("%02x", dgst[i]);
  }
  putchar ('\n');
}

// generate SHA-3 hash of string
void MD4_string (char *str)
{
  MD4_CTX ctx;
  size_t  i;
  uint8_t dgst[256];

  printf ("\nMD4(\"%s\")\n0x", str);
  
  MD4_Init (&ctx);
  MD4_Update (&ctx, str, strlen (str));
  MD4_Final (dgst, &ctx);
  
  MD4_print (dgst, MD4_DIGEST_LENGTH);
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
  printf ("\rProcessed %llu MB out of %llu MB %lu MB/s : %llu%% complete. ETA: %02d:%02d     ",
    fs_complete/1000/1000, fs_total/1000/1000, speed/1000/1000, pct, minutes, seconds);
}

// generate SHA-3 hash of file
void MD4_file (char fn[])
{
  FILE     *fd;
  MD4_CTX ctx;
  size_t   len;
  uint8_t  buf[BUFSIZ], dgst[256];
  struct stat st;
  uint32_t cmp=0, total=0;
  
  fd = fopen (fn, "rb");
  if (fd!=NULL)
  {
    stat (fn, &st);
    total=st.st_size;
    
    MD4_Init (&ctx);
    
    while (len = fread (buf, 1, BUFSIZ, fd)) {
      cmp += len;
      if (cmp > 10000000 && (cmp % 10000000)==0 || cmp==total) {
        progress (cmp, total);
      }
      MD4_Update (&ctx, buf, len);
    }
    MD4_Final (dgst, &ctx);

    fclose (fd);

    printf ("\n  [ MD4 (%s) = ", fn);
    MD4_print (dgst, MD4_DIGEST_LENGTH);
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
  
  printf ("\n  usage: MD4_test -t <type> -f <file> -s <string>\n");
  printf ("\n  -k <type>   Key for HMAC");
  printf ("\n  -s <string> Derive MD4 hash of <string>");
  printf ("\n  -f <file>   Derive MD4 hash of <file>");
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
      printf ("\n  [ self-test OK!\n");
    }
  } else if (str!=NULL) {
    MD4_string (str);
  } else if (file!=NULL || wc!=0) {
    if (wc!=0) {
      while (argv[wc]!=NULL) {
        MD4_file (argv[wc++]);
      }
    } else {
      MD4_file (file);
    }
  } else {
    usage ();
  }
  return 0;
}
