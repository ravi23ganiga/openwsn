//---------------------------------------------------------------------------


#pragma hdrstop

#include "rtl_crc.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

/*
 * Reference
 * http://www.geocities.com/malbrain/crc_c.html
 */

/*
 *        calculate ccitt cyclic redundancy codes
 *
 *        actual Crc16 = x ^ 16 + x ^ 12 + x ^ 5 + 1
 *
 *        inverse Crc32 = x ^ 32 + x ^ 31 + x ^ 30 +
 *              x ^ 28 + x ^ 27 + x ^ 25 + x ^ 24 +
 *              x ^ 22 + x ^ 21 + x ^ 20 + x ^ 16 +
 *              x ^ 10 + x ^ 9 + x ^ 6 + 1
 *
 *        n.b. standard ANSI X3.66 crc-32 polynomial:
 *        x^0 + x^1 + x^2 + x^4 + x^5 + x^7 + x^8 + x^10 +
 *        x^11 + x^12 + x^16 + x^22 + x^23 + x^26 + x^32
 *
 *        Tables constructed so that entry 0x80 = inverse polynomial
 */

unsigned short Crc16[] = {
0x0000, 0x1081, 0x2102, 0x3183, 0x4204, 0x5285, 0x6306, 0x7387,
0x8408, 0x9489, 0xa50a, 0xb58b, 0xc60c, 0xd68d, 0xe70e, 0xf78f
};

unsigned long Crc32[] = {
0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
};

unsigned short crc16_calc (unsigned char *ptr, unsigned cnt, unsigned short crc)
{
    while( cnt-- ) {
        crc = ( crc >> 4 ) ^ Crc16[(crc & 0xf) ^ (*ptr & 0xf)];
        crc = ( crc >> 4 ) ^ Crc16[(crc & 0xf) ^ (*ptr++ >> 4)];
    }

    return crc;
}

unsigned long crc32_calc (unsigned char *ptr, unsigned cnt, unsigned long crc)
{
    while( cnt-- ) {
        crc = ( crc >> 4 ) ^ Crc32[(crc & 0xf) ^ (*ptr & 0xf)];
        crc = ( crc >> 4 ) ^ Crc32[(crc & 0xf) ^ (*ptr++ >> 4)];
    }

    return crc;
}

#ifdef STANDALONE
#include <stdio.h>

int main (int argc, char **argv)
{
int len = strlen(argv[1]), idx;
unsigned char msg[128];
unsigned short v16;
unsigned long v32;

    if( len > sizeof(msg) - sizeof(unsigned long) )
        len = sizeof(msg) - sizeof(unsigned);

    memcpy (msg, argv[1], len);

    //  CRC32 test:

    v32 = ~crc32_calc (msg, len, ~0U);
    printf("32 bit CRC: %08lx", v32);

    //  append the inverted the crc and recompute

    for( idx = 0; idx < sizeof(unsigned long); idx++ )
        msg[len + idx] = v32 >> idx * 8;

    v32 = crc32_calc (msg, len + sizeof(unsigned), ~0UL);

    //  this should yield the well known CRC32 residue value:
    //  0xdebb20e3

    printf("   RESIDUE: %08lx\n", v32);

    //  CRC16 test:

    v16 = ~crc16_calc (msg, len, ~0U);
    printf("16 bit CRC: %04lx    ", v16);

    for( idx = 0; idx < sizeof(unsigned short); idx++ )
        msg[len + idx] = v16 >> idx * 8;

    v16 = crc16_calc (msg, len + sizeof(unsigned short), 0xffff);

    //  this should yield the well known CRC16 residue value:
    //  0xf0b8

    printf("   RESIDUE: %04lx\n", v16);
}
#endif


