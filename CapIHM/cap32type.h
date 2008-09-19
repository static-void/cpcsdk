//
// Caprice32 emulator predefined types
//

#ifndef _CAP32TYPES_H_
#define _CAP32TYPES_H_
#include <stdlib.h>

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;
/*
#ifdef _MSC_VER
typedef _int64 int64_t;
#else
typedef long long int64_t;
#endif
*/
typedef union {
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
   struct { byte l, h, h2, h3; } b;
   struct { word l, h; } w;
#else
   struct { byte h3, h2, h, l; } b;
   struct { word h, l; } w;
#endif
   dword d;
}  reg_pair;

#define MAX_LINE_LEN 256

#endif