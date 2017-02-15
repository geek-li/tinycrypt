

// BLAKE2b test unit in C
// Odzhan

#include "b2b.h"

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

char *BLAKE2B_dgst[] =
{ "786a02f742015903c6c6fd852552d272912f4740e15847618a86e217f71f5419d25e1031afee585313896444934eb04b903a685b1448b755d56f701afe9be2ce",
  "333fcb4ee1aa7c115355ec66ceac917c8bfd815bf7587d325aec1864edd24e34d5abe2c6b1b5ee3face62fed78dbef802f2a85cb91d455a8f5249d330853cb3c",
  "ba80a53f981c4d0d6a2797b69f12f6e94c212f14685ac4b74b12bb6fdbffa2d17d87c5392aab792dc252d5de4533cc9518d38aa8dbf1925ab92386edd4009923",
  "3c26ce487b1c0f062363afa3c675ebdbf5f4ef9bdc022cfbef91e3111cdc283840d8331fc30a8a0906cff4bcdbcd230c61aaec60fdfad457ed96b709a382359a",
  "c68ede143e416eb7b4aaae0d8e48e55dd529eafed10b1df1a61416953a2b0a5666c761e7d412e6709e31ffe221b7a7a73908cb95a4d120b8b090a87d1fbedb4c",
  "99964802e5c25e703722905d3fb80046b6bca698ca9e2cc7e49b4fe1fa087c2edf0312dfbb275cf250a1e542fd5dc2edd313f9c491127c2e8c0c9b24168e2d50",
  "686f41ec5afff6e87e1f076f542aa466466ff5fbde162c48481ba48a748d842799f5b30f5b67fc684771b33b994206d05cc310f31914edd7b97e41860d77d282"
};

char *BLAKE2B_tv[] =
{ "",
  "00", 
  "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f"
  "404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f"
  "808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebf"
  "c0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfe"};

char *BLAKE2B_key[] =
{ "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f",
  "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f",
  "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f"};

char *BLAKE2B_res[] =
{ "10ebb67700b1868efb4417987acf4690ae9d972fb7a590c2f02871799aaa4786b5e996e8f0f4eb981fc214b005f42d2ff4233499391653df7aefcbc13fc51568",
  "961f6dd1e4dd30f63901690c512e78e4b45e4742ed197c3c5e45c549fd25f2e4187b0bc9fe30492b16b0d0bc4ef9b0f34c7003fac09a5ef1532e69430234cebd",
  "142709d62e28fcccd0af97fad0f8465b971e82201dc51070faa0372aa43e92484be1c1e73ba10906d5d1853db6a4106e0a7bf9800d373d6dee2d46d62ef2a461"};

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

// unkeyed tests
int run_tests (void)
{
  uint8_t     dgst[BLAKE2b_DIGEST_LENGTH], tv[BLAKE2b_DIGEST_LENGTH];
  int         i, fails=0;
  b2b_ctx ctx;
  
  for (i=0; i<sizeof(text)/sizeof(char*); i++)
  {
    b2b_init (&ctx, BLAKE2b_DIGEST_LENGTH, NULL, 0, 0);
    b2b_update (&ctx, text[i], strlen(text[i]));
    b2b_final (dgst, &ctx);
    
    hex2bin (tv, BLAKE2B_dgst[i]);
    
    if (memcmp (dgst, tv, BLAKE2b_DIGEST_LENGTH) != 0) {
      printf ("\nFailed for string: %s", text[i]);
      ++fails;
    }
  }
  return fails;
}

// keyed tests
int run_xtests (void)
{
  uint8_t input[1024], key[64], res[64], dgst[64];
  uint32_t keylen, inlen, outlen;
  
  int i, fails=0;
  b2b_ctx ctx;
  
  for (i=0; i<sizeof(BLAKE2B_tv)/sizeof(char*); i++)
  {
    inlen  = hex2bin (input, BLAKE2B_tv[i]);
    keylen = hex2bin (key, BLAKE2B_key[i]);
    outlen = hex2bin (res, BLAKE2B_res[i]);
    
    b2b_init (&ctx, outlen, key, keylen, 0);
    b2b_update (&ctx, input, inlen);
    b2b_final (dgst, &ctx);
    
    if (memcmp (dgst, res, outlen) != 0) {
      printf ("\nFailed for string: %s", BLAKE2B_tv[i]);
      ++fails;
    }
  }
  return fails;
}

// print digest
void BLAKE2B_print (uint8_t dgst[], size_t len)
{
  size_t i;
  for (i=0; i<len; i++) {
    printf ("%02x", dgst[i]);
  }
  putchar ('\n');
}

// generate BLAKE2B hash of string
void BLAKE2B_string (char *str, char *key)
{
  b2b_ctx ctx;
  uint8_t dgst[256];

  printf ("\nBLAKE2B(\"%s\")\n0x", str);
  
  b2b_init (&ctx, BLAKE2b_DIGEST_LENGTH, key, key!=NULL ? strlen (key) : 0, 0);
  b2b_update (&ctx, str, strlen (str));
  b2b_final (dgst, &ctx);
  
  BLAKE2B_print (dgst, BLAKE2b_DIGEST_LENGTH);
  
  printf ("\nBLAKE2B(\"%s\")\n0x", str);
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

// generate BLAKE2B hash of file
void BLAKE2B_file (char fn[], char *key)
{
  FILE        *fd;
  b2b_ctx ctx;
  size_t      len;
  uint8_t     buf[BUFSIZ], dgst[256];
  struct stat st;
  uint32_t    cmp=0, total=0;
  
  fd = fopen (fn, "rb");
  
  if (fd!=NULL)
  {
    stat (fn, &st);
    total=st.st_size;
    
    b2b_init (&ctx, BLAKE2b_DIGEST_LENGTH, key, key!=NULL ? strlen (key) : 0, 0);
    
    while (len = fread (buf, 1, BUFSIZ, fd)) {
      cmp += len;
      if ((cmp > 10000000) && ((cmp % 10000000)==0 || cmp==total)) {
        progress (cmp, total);
      }
      b2b_update (&ctx, buf, len);
    }
    b2b_final (dgst, &ctx);

    fclose (fd);

    printf ("\n  [ BLAKE2B (%s) = ", fn);
    BLAKE2B_print (dgst, BLAKE2b_DIGEST_LENGTH);
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
  printf ("\n  usage: b2b_test -t <type> -f <file> -s <string>\n");
  printf ("\n  -k <type>   Key for MAC");
  printf ("\n  -s <string> Derive BLAKE2B hash of <string>");
  printf ("\n  -f <file>   Derive BLAKE2B hash of <file>");
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
    BLAKE2B_string (str, key);
  } else if (file!=NULL || wc!=0) {
    if (wc!=0) {
      while (argv[wc]!=NULL) {
        BLAKE2B_file (argv[wc++], key);
      }
    } else {
      BLAKE2B_file (file, key);
    }
  } else {
    usage ();
  }
  return 0;
}
