/*
 * PlatformDataTypes.h
 *
 *  Created on: 16 Sep 2009
 *      Author: Guy Burton
 */

#ifndef PLATFORMDATATYPES_H_
#define PLATFORMDATATYPES_H_

#ifdef linux

#include <stdint.h>

typedef unsigned short i_uint16;
typedef short i_sint16;
typedef uint32_t i_uint32; // unsigned long
typedef int32_t i_sint32; // signed long
typedef uint64_t i_uint64 ; // unsigned long long
typedef int64_t i_sint64; // signed long long

#else
typedef unsigned __int16 i_uint16;
typedef __int16 i_sint16;
typedef unsigned __int32 i_uint32;
typedef __int32 i_sint32;
typedef unsigned __int64 i_uint64;
typedef __int64 i_sint64;
#endif

#ifdef DEF_BIG_ENDIAN
#include <byteswap.h>
#define EndianSwap64(x) bswap_64(x)
#define EndianSwap32(x) bswap_32(x)
#define EndianSwap16(x) bswap_16(x)

#else
#define EndianSwap64(x) x
#define EndianSwap32(x) x
#define EndianSwap16(x) x
#endif

#endif /* PLATFORMDATATYPES_H_ */
