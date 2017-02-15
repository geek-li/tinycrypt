
/**

1) server sends public key to client
2) client generates session key and encrypts using public key
3) client sends session key to server
4) server decrypts session key

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/rsa.h>

BIGNUM *e;
BN_CTX *ctx;
RSA   *rsa;
  
typedef struct _asc_t {
  uint32_t p;
  int      s;
  int      secure;
  union {
    uint8_t v8[16];
    uint32_t v32[4];
  } len;
  uint8_t  buf[BUFSIZ+64];
  uint8_t  mac[32];
  uint8_t  key[32];
  uint8_t  rnd[128];
} asc;

asc x;

// receive packet and decrypt if required
int recv_pkt (void)
{
  int len;
  len=recv (x.s, (char*)x.len.v8, 16, 0);
  if (x.secure) decrypt(x.len, 16);
  if (x.len.v32[0]>sizeof(x.buf)) return -1;
  len=recv (x.s, (char*)x.buf, x.len.v8, 0);
  if (x.secure) decrypt(x.buf, x.len);
}

// send packet encrypted if required
int send_pkt (void)
{
  int len;
  if (x.secure) encrypt (x.len, 16);
  send (x.s, x.len, 16, 0);
  if (x.secure) encrypt (x.buf, x.len);
  send (x.s, x.buf, x.len, 0);
  return len;
}

int init_session (void)
{
  HCRYPTPROV hProvider;
  int r;
  
  CryptAcquireContext (&hProvider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT);
  CryptGenRandom (hProvider, x.rnd, sizeof (x.rnd));
  CryptReleaseContext (hProvider, 0);
  return r;
}

void key_xchg (void)
{
  if (init_session())  // initialize session key
  {
    if (recv_pkt())    // receive public key
    {      
      modexp();        // encrypt the session key
      send_pkt();      // send session key
    }
  }
}

void start_crypt(void)
{
  ctx = BN_CTX_new();
  BN_CTX_init(ctx);

  rsa = RSA_new();
  
  e = BN_new();
  BN_dec2bn(&e,"65537");
  
  RSA_generate_key_ex(rsa, 2048, e, NULL);
}

void end_crypt (void)
{
  RSA_free(rsa);
  BN_free(e);
  BN_CTX_end(ctx);
  BN_CTX_free(ctx);
}

void key_xchg (void)
{
  uint8_t buf[2048];
  uint32_t keylen=0;
  
  // send public
  send_pkt (rsa->n->d, rsa->n->dmax);
  // wait for encrypted session key
  keylen=recv_pkt (buf, sizeof(buf));
  RSA_private_decrypt(keylen, session_key, buf, rsa, RSA_NO_PADDING);
}

int main(int argc, char *argv[])
{
  WSADATA wsa;
  struct sockaddr_in sin;
  int r, t;
  
  WSAStartup(MAKEWORD(2,2), &wsa);
  
  s=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (s != -1)
  {
    t=1;
    setsockopt (s, SOL_SOCKET, SO_REUSEADDR, (char*)&t, sizeof (t));
    
    if (listen(s, 1) != -1)
    {
      if (bind(s, &sin, sizeof(sin)) != -1)
      {
        r=accept (s, &sin, sizeof(sin));
        if (r != -1)
        {
          
        }
      }
    }
    close (s);
  }
  WSACleanup();
  return 0;
}
