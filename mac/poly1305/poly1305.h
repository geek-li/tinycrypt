
#ifndef POLY1305_H
#define POLY1305_H

#ifdef __cplusplus
extern "C" {
#endif

void poly1305_key_gen(void*, const void*, const void*);
void poly1305_mac (uint8_t*, const uint8_t*, uint32_t, const uint8_t*);
    
#ifdef __cplusplus
}
#endif

#endif