

// Test unit for SHA-3 in C
// Odzhan

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "sha3.h"

char *text[] =
{ "",
  "a",
  "abc",
  "message digest",
  "abcdefghijklmnopqrstuvwxyz",
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
  "12345678901234567890123456789012345678901234567890123456789012345678901234567890", "1F42ADD25C0A80A4C82AAE3A0E302ABF9261DCA7E7884FD869D96ED4CE88AAAA25304D2D79E1FA5CC1FA2C95899229BC87431AD06DA524F2140E70BD0536E9685EE7808F598D8A9FE15D40A72AEFF431239292C5F64BDB7F620E5D160B329DEB58CF6D5C0665A3DED61AE4ADBCA94DC2B7B02CDF3992FDF79B3D93E546D5823C3A630923064ED24C3D974C4602A49DF75E49CF7BD51EDC7382214CBA850C4D3D11B40A70B1D926E3755EC79693620C242AB0F23EA206BA337A7EDC5421D63126CB6C7094F6BC1CF9943796BE2A0D9EB74FC726AA0C0D3B3D39039DEAD39A7169F8C3E2365DD349E358BF08C717D2E436D65172A76ED5E1F1E694A75C19280B15"
};

char *SHA3_dgst[] =
{ "a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a",
  "80084bf2fba02475726feb2cab2d8215eab14bc6bdd8bfb2c8151257032ecd8b",
  "3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532",
  "edcdb2069366e75243860c18c3a11465eca34bce6143d30c8665cefcfd32bffd",
  "7cab2dc765e21b241dbc1c255ce620b29f527c6d5e7f5f843e56288f0d707521",
  "a79d6a9da47f04a3b9a9323ec9991f2105d4c78a7bc7beeb103855a7a11dfb9f",
  "293e5ce4ce54ee71990ab06e511b7ccd62722b1beb414f5ff65c8274e0f5be1d",
  "eda9d864171fbba7de3808c1d441289c5c213843453cf3f931850fb3ca4a5de6"
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

int mdlen (int type)
{
  int len;
  
  switch (type)
  {
    case 0:
      len=SHA3_224;
      break;
    case 1:
      len=SHA3_256;
      break;
    case 2:
      len=SHA3_384;
      break;
    case 3:
      len=SHA3_512;
      break;
    default:
      len=SHA3_256;
      break;
  }
  return len;
}

// print digest
void SHA3_print (uint8_t dgst[], size_t len)
{
  size_t i;
  for (i=0; i<len; i++) {
    printf ("%02x", dgst[i]);
  }
  putchar ('\n');
}

int run_tests (void)
{
  uint8_t  dgst_out[256], buf[2048], dgst_in[256];
  int      i, fails=0, dgst_len, inlen;
  SHA3_CTX ctx;
  uint8_t *input;
  int tv_cnt=sizeof(text)/sizeof(char*);
  
  for (i=0; i<tv_cnt; i++)
  {
    // convert the digest to binary
    dgst_len=hex2bin (dgst_in, SHA3_dgst[i]);
    // if this is the last one, convert the string to binary
    if ((i+1)==tv_cnt) {
      memset (buf, sizeof (buf), 0);
      input=buf;
      inlen=hex2bin (buf, text[i]);
    } else {
      // just use string itself
      input=text[i];
      inlen=strlen(input);
    }
    // get hash
    SHA3_Init (&ctx, dgst_len);
    SHA3_Update (&ctx, input, inlen);
    SHA3_Final (dgst_out, &ctx);
    
    if (memcmp (dgst_in, dgst_out, ctx.olen) != 0) {
      printf ("\nFailed for string \"%s\"", text[i]);
      ++fails;
    }
  }
  return fails;
}

// generate SHA-3 hash of string
void SHA3_string (char *str, char *key, int type)
{
  SHA3_CTX ctx;
  uint8_t  dgst[256];
  char     *hdrs[]={ "SHA3-224", "SHA3-256", 
                     "SHA3-384", "SHA3-512" };

  printf ("\n%s(\"%s\")\n0x", hdrs[type], str);
  
  SHA3_Init (&ctx, mdlen(type));
  // prepend a key?
  if (key!=NULL) {
    SHA3_Update (&ctx, key, strlen(key));
  }
  SHA3_Update (&ctx, str, strlen (str));
  SHA3_Final (dgst, &ctx);
  
  SHA3_print (dgst, ctx.olen);
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
  // if both complete and total are equal, reset timer
  if (fs_complete==fs_total) {
    start=0;
  }
}

// generate SHA-3 hash of file
void SHA3_file (char fn[], char *key, int type)
{
  FILE     *fd;
  SHA3_CTX ctx;
  size_t   len;
  uint8_t  buf[4096+1], dgst[256];
  struct stat st;
  uint64_t cmp=0, total=0, pct=0, stats=0;
  
  fd = fopen (fn, "rb");
  
  if (fd!=NULL)
  {
    printf ("\n  [ SHA3-%d (%s) = ", mdlen(type)*8, fn);
    
    stat (fn, &st);
    total=st.st_size;
    
    // if file greater than 10 megs, we'll display stats
    if (total > (1024000*10)) {
      stats=1;
      // get 3% of the file size
      pct=(uint64_t)((double)total * 0.03);
    
      printf ("\npct is %llu", pct);
      
      // make sure it's congruent to buffer read
      pct &= -4096;
    }
    SHA3_Init (&ctx, mdlen(type));
    
    // prepend a key?
    if (key!=NULL) {
      SHA3_Update (&ctx, key, strlen(key));
    }
    while ((len = fread (buf, 1, 4096, fd))!=0) {
      if (stats) {
        printf ("\ndividing %llu by %llu", total, pct);
        cmp += len;
        if (((cmp % pct)==0) || cmp==total) {
          progress (cmp, total);
        }
      }
      SHA3_Update (&ctx, buf, len);
    }
    SHA3_Final (dgst, &ctx);

    fclose (fd);

    SHA3_print (dgst, ctx.olen);
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
  printf ("\n  usage: test -t <type> -f <file> -s <string>\n");
  printf ("\n  -t <type>   Type is 0=SHA3-224, 1=SHA3-256 (default), 2=SHA3-384, 3=SHA3-512");
  printf ("\n  -s <string> Derive SHA3 hash of <string>");
  printf ("\n  -f <file>   Derive SHA3 hash of <file>");
  printf ("\n  -k <key>    Create MAC using <key>");
  printf ("\n  -x          Run tests\n");
  exit (0);
}

int main (int argc, char *argv[])
{
  char opt;
  int i, test=0, type=1, wc=0;
  char *file=NULL, *str=NULL, *key=NULL;

  setbuf (stdout, NULL);
  
  if (argc==1) {
    if (!run_tests()) {
      printf ("\n  [ sha-3 self-test OK!\n");
    }
    return 0;
  }
    
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
        case 't':
          type=atoi(getparam (argc, argv, &i));
          break;
        case 'x':
          test=1;
          break;
        default:
          usage ();
          break;
      }
    } else {
      if (wc==0) {
        wc=i;
      }
    }
  }
  
  if (test) {
    if (!run_tests()) {
      printf ("\n  [ self-test OK!\n");
    }
  } else if (str!=NULL) {
    SHA3_string (str, key, type);
  } else if (file!=NULL || wc!=0) {
    if (wc!=0) {
      while (argv[wc]!=NULL) {
        SHA3_file (argv[wc++], key, type);
      }
    } else {
      SHA3_file (file, key, type);
    }
  } else {
    usage ();
  }
  return 0;
}
