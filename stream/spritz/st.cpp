

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <windows.h>

char *keys[] = { "ABC", "spam", "arcfour" };

void dump_bytes (uint8_t b[], size_t len)
{
    for (size_t i=0; i<len; i++)
    {
        printf ("%02x ", b[i]);
    }
    putchar ('\n');
}

enum {
  spritz_N = 256
};

typedef struct
{
  uint8_t a, i, j, k, z, w;
  uint8_t S[spritz_N];
} spritz_state;

/*******************************************************************************/
/* the spritz primitives */
extern "C" void    __fastcall spritz_init            (spritz_state *s);
extern "C" void    __fastcall spritz_shuffle_squeeze (spritz_state *s);
extern "C" void    __fastcall spritz_update          (spritz_state *s);
extern "C" void    __fastcall spritz_whip            (spritz_state *s, uint_fast16_t r);
extern "C" void    __fastcall spritz_crush           (spritz_state *s);
extern "C" void    __fastcall spritz_shuffle         (spritz_state *s);
extern "C" void    __fastcall spritz_absorb          (spritz_state *s, const void *I, size_t I_len);
extern "C" void    __fastcall spritz_absorb_byte     (spritz_state *s, uint32_t b);
extern "C" void    __fastcall spritz_absorb_stop     (spritz_state *s);
extern "C" void    __fastcall spritz_absorb_and_stop (spritz_state *s, const void *I, size_t I_len); /* commonly used helper function */
extern "C" uint8_t __fastcall spritz_output          (spritz_state *s);
extern "C" void    __fastcall spritz_squeeze         (spritz_state *s, void *P, size_t P_len);
extern "C" uint8_t __fastcall spritz_drip            (spritz_state *s);
extern "C" uint8_t __fastcall spritz_drip_nosqueeze  (spritz_state *s);

/* the spritz hash inline functions */

static void
spritz_hash_init (spritz_state *s)
{
  spritz_init (s);
}

static void
spritz_hash_add (spritz_state *s, const void *M, size_t M_len)
{
  spritz_absorb (s, M, M_len);
}

void
spritz_hash_finish (spritz_state *s, void *H, size_t H_len)
{
  spritz_absorb_stop (s);
  spritz_absorb_byte (s, H_len);
  spritz_squeeze (s, H, H_len);
}

void
spritz_cipher_init (spritz_state *s, const void *K, size_t K_len, const void *IV, size_t IV_len)
{
  spritz_init (s);
  spritz_absorb (s, K, K_len);

  if (IV)
    {
      spritz_absorb_stop (s);
      spritz_absorb (s, IV, IV_len);
    }

  spritz_shuffle_squeeze (s);
}

void
spritz_cipher_encrypt (spritz_state *s, const void *I, void *O, size_t len)
{
  const uint8_t *i = (const uint8_t *)I;
        uint8_t *o = (      uint8_t *)O;

  while (len--)
    *o++ = *i++ + spritz_drip_nosqueeze (s);
}

void
spritz_cipher_decrypt (spritz_state *s, const void *I, void *O, size_t len)
{
  const uint8_t *i = (const uint8_t *)I;
        uint8_t *o = (      uint8_t *)O;

  while (len--)
    *o++ = *i++ - spritz_drip_nosqueeze (s);
}

/*****************************************************************************/

void
spritz_cipher_xor_crypt (spritz_state *s, const void *I, void *O, size_t len)
{
  const uint8_t *i = (const uint8_t *)I;
        uint8_t *o = (      uint8_t *)O;

  while (len--)
    *o++ = *i++ ^ spritz_drip_nosqueeze (s);
}

void dump_hash (void)
{
    uint8_t      out[32];
    spritz_state s;
    
    for (int i=0; i<sizeof(keys) / sizeof(char*); i++)
    {
        memset (out, 0, sizeof (out));
        spritz_hash_init (&s);
        spritz_hash_add (&s, keys[i], strlen(keys[i]));
        spritz_hash_finish (&s, out, sizeof (out));
        
        dump_bytes (out, sizeof (out));
    }
}

int main (int argc, char *argv[])
{
    spritz_state s;
    uint8_t      ct[128]={0};
    uint8_t      ptx[128]={0};
    char         pt[]="The Spritz's recipe is shrouded in mystery";
    const        char key[]="secret password";
    size_t       keylen = strlen (key);
    size_t       ptlen = strlen (pt);
    
    dump_hash();
    exit(0);
    
    dump_bytes ((uint8_t*)pt, ptlen);
        
    spritz_cipher_init (&s, key, keylen, 0, 0);
    spritz_cipher_encrypt (&s, pt, ct, ptlen);
    dump_bytes (ct, ptlen);
    
    spritz_cipher_init (&s, key, keylen, 0, 0);
    spritz_cipher_decrypt (&s, ct, ptx, ptlen);
    dump_bytes (ptx, ptlen);
    
    return 0;
}
