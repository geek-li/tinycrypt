

// Test unit for CubeHash-256 in C with parameters of r=1, b=1
// Odzhan

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "cubehash.h"

#ifdef TEST
#define cube_ctx hashState
#define cube_init(x) Init(x, 256)
#define cube_update(x, y, z) Update(x, y, z*8)
#define cube_final(x, y) Final(y, x)
#endif

char *text[] =
{ "",
  "a",
  "abc",
  "message digest",
  "abcdefghijklmnopqrstuvwxyz",
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
  "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
};

char *Cube_dgst[] =
{ "80f72e07d04ddadb44a78823e0af2ea9f72ef3bf366fd773aa1fa33fc030e5cb",
  "d1bcae7578b5a345683a72be9816d0511629d5d7759051a1048018766c8da89e",
  "6d63ca66e0a5124797bf840718002740727b8662848963a73cf49b21cfa8f6e6",
  "cf8df1b73014fc4fea6fce64f84029ff0385e5b08b8df5e83323fba86ec69fe1",
  "2973ece34a2540789a02ea44218e209ca73f184bfaddf18366f8e7c6ca00a783",
  "f2d1e7b8827943b724f15a9c634ca2adf1e769db411670747ded3a562b69f4fa",
  "f52728f93799a87a7fd215f04024258843b9d031848f93ebc32dec2c1af81d53"
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

// print digest
void Cube_print (uint8_t dgst[], size_t len)
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
  cube_ctx ctx;
  uint8_t *input;
  int tv_cnt=sizeof(text)/sizeof(char*);
  
  for (i=0; i<tv_cnt; i++)
  {
    // convert the digest to binary
    dgst_len=hex2bin (dgst_in, Cube_dgst[i]);
    
    input=text[i];
    inlen=strlen(input);
    
    // get hash
    cube_init (&ctx);
    cube_update (&ctx, input, inlen);
    cube_final (dgst_out, &ctx);
    
    if (memcmp (dgst_in, dgst_out, 32) != 0) {
      printf ("\nFailed for string \"%s\"", text[i]);
      Cube_print (dgst_out, 32);
      ++fails;
    }
  }
  return fails;
}

// generate SHA-3 hash of string
void Cube_string (char *str, char *key, int type)
{
  cube_ctx ctx;
  uint8_t  dgst[32];
  
  cube_init (&ctx);
  // prepend a key?
  if (key!=NULL) {
    cube_update (&ctx, key, strlen(key));
  }
  cube_update (&ctx, str, strlen (str));
  cube_final (dgst, &ctx);
  
  Cube_print (dgst, 32);
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
void Cube_file (char fn[], char *key)
{
  FILE     *fd;
  cube_ctx ctx;
  size_t   len;
  uint8_t  buf[4096+1], dgst[32];
  struct stat st;
  uint64_t cmp=0, total=0, pct=0, stats=0;
  
  fd = fopen (fn, "rb");
  
  if (fd!=NULL)
  {
    printf ("\n  [ Cube-%d (%s) = ", 256, fn);
    
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
    cube_init (&ctx);
    
    // prepend a key?
    if (key!=NULL) {
      cube_update (&ctx, key, strlen(key));
    }
    while ((len = fread (buf, 1, 4096, fd))!=0) {
      if (stats) {
        //printf ("\ndividing %llu by %llu", total, pct);
        cmp += len;
        if (((cmp % pct)==0) || cmp==total) {
          progress (cmp, total);
        }
      }
      cube_update (&ctx, buf, len);
    }
    cube_final (dgst, &ctx);

    fclose (fd);

    Cube_print (dgst, 32);
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
  printf ("\n  usage: test -f <file> -s <string>\n");
  printf ("\n  -s <string> Derive Cube hash of <string>");
  printf ("\n  -f <file>   Derive Cube hash of <file>");
  printf ("\n  -k <key>    Create MAC using <key>");
  printf ("\n  -x          Run tests\n");
  exit (0);
}

int main (int argc, char *argv[])
{
  char opt;
  int i, test=0, type=1, wc=0;
  char *file=NULL, *str=NULL, *key=NULL;

  int j;
  setbuf (stdout, NULL);
  
  if (argc==1) {
    if (!run_tests()) {
      printf ("\n  [ cube1/1-256 self-test OK!\n");
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
    Cube_string (str, key, type);
  } else if (file!=NULL || wc!=0) {
    if (wc!=0) {
      while (argv[wc]!=NULL) {
        Cube_file (argv[wc++], key);
      }
    } else {
      Cube_file (file, key);
    }
  } else {
    usage ();
  }
  return 0;
}
