#include <stdio.h>
#include "belt.h"

int main() {

    uint8_t inenc[] =
    {
        0xB1,0x94,0xBA,0xC8,
        0x0A,0x08,0xF5,0x3B,
        0x36,0x6D,0x00,0x8E,
        0x58,0x4A,0x5D,0xE4,
    };

    uint8_t outenc[16] = {0};

    uint8_t keyenc[32] =
    {
        0xE9,0xDE,0xE7,0x2C,
        0x8F,0x0C,0x0F,0xA6,
        0x2D,0xDB,0x49,0xF4,
        0x6F,0x73,0x96,0x47,
        0x06,0x07,0x53,0x16,
        0xED,0x24,0x7A,0x37,
        0x39,0xCB,0xA3,0x83,
        0x03,0xA9,0x8B,0xF6,
    };

    uint8_t indec[16] =
    {
        0xE1,0x2B,0xDC,0x1A,
        0xE2,0x82,0x57,0xEC,
        0x70,0x3F,0xCC,0xF0,
        0x95,0xEE,0x8D,0xF1
    };

    uint8_t outdec[16] = {0};

    uint8_t keydec[32] =
    {
        0x92,0xBD,0x9B,0x1C,
        0xE5,0xD1,0x41,0x01,
        0x54,0x45,0xFB,0xC9,
        0x5E,0x4D,0x0E,0xF2,
        0x68,0x20,0x80,0xAA,
        0x22,0x7D,0x64,0x2F,
        0x26,0x87,0xF9,0x34,
        0x90,0x40,0x55,0x11
    };

    uint8_t ks[32] = {0};

    size_t i;

    //belt_init(ks, keyenc, 32);
    belt_encryptx( (uint32_t*)outenc, (uint32_t*)inenc, (uint32_t*)keyenc, 1);

    printf("Encryption:\n");
    printf("m: ");
    for (i = 0; i < 16; ++i) { printf("%02X ", inenc[i]); }
    printf("\nk: ");
    for (i = 0; i < 32; ++i) { printf("%02X ", ks[i]); }
    printf("\nc: ");
    for (i = 0; i < 16; ++i) { printf("%02X ", outenc[i]); }

    //belt_init(ks, keydec, 32);
    belt_encryptx((uint32_t*)outdec, (uint32_t*)indec, (uint32_t*)keydec, 0);

    printf("\nDecryption:\n");
    printf("c: ");
    for (i = 0; i < 16; ++i) { printf("%02X ", indec[i]); }
    printf("\nk: ");
    for (i = 0; i < 32; ++i) { printf("%02X ", ks[i]); }
    printf("\nm: ");
    for (i = 0; i < 16; ++i) { printf("%02X ", outdec[i]); }
    printf("\n");

    return 0;
}