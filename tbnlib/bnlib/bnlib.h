
// these defines should be in bnlib32.asm and bnlib64.asm before assembly
#define MAX_BITS 4096 + 1
#define MAX_BYTE MAX_BITS / 8
#define MAX_WORD MAX_BITS / 32

#ifdef __cplusplus
extern "C" {
#endif

void __fastcall bn_zero (void *a);
void __fastcall bn_one (void *a);
void __fastcall bn_copy (void *a, void *b);

int __fastcall bn_cmp (void *a, void *b);

size_t __fastcall bn_num_bits (void *a);
size_t __fastcall bn_num_bytes (void *a);
size_t __fastcall bn_num_words (void *a);

void __fastcall bn_sqr (void *r, void *a);
void __fastcall bn_add (void *r, void *a, void *b);
void __fastcall bn_sub (void *r, void *a, void *b);
void __fastcall bn_mul (void *r, void *a, void *b);
void __fastcall bn_mod (void *r, void *a, void *m);
void __fastcall bn_exp (void *r, void *a, void *b);
void __fastcall bn_div (void *a, void *b, void *q, void *r);

void __fastcall bn_mod_sqr (void *r, void *a, void *m);
void __fastcall bn_mod_inv (void *r, void *a, void *m);
void __fastcall bn_mod_add (void *r, void *a, void *b, void *m);
void __fastcall bn_mod_mul (void *r, void *a, void *b, void *m);

void __fastcall bn_mod_mul_quick (void *r, void *a, void *b, void *m);

void __fastcall bn_mod_sub (void *r, void *a, void *b, void *m);
void __fastcall bn_mod_exp (void *r, void *a, void *b, void *m);

void __fastcall bn_lshift (void *a, int x);
void __fastcall bn_lshift1 (void *a);

void __fastcall bn_rshift (void *a, int x);
void __fastcall bn_rshift1 (void *a);

void __fastcall bn_add_word (void *a, uint32_t b);
void __fastcall bn_sub_word (void *a, uint32_t b);
void __fastcall bn_mul_word (void *a, uint32_t b);

uint32_t __fastcall bn_mod_word (void *a, uint32_t b);
uint32_t __fastcall bn_div_word (void *a, uint32_t b);

void __fastcall bn_hex2bn (void *a, const char *s);
char* __fastcall bn_bn2hex (void *a);

void __fastcall bn_dec2bn (void *a, const char *s);
char* __fastcall bn_bn2dec (void *a);

void __fastcall bn_bn2bin (void *a, uint8_t *b);
void __fastcall bn_bin2bn (uint8_t *b, size_t len, void *a);

#ifdef __cplusplus
}
#endif
