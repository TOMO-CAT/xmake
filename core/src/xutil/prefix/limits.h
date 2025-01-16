#pragma once

#include "xutil/config.h"

#define XU_MAXS8 ((xu_sint8_t)(0x7f))
#define XU_MINS8 ((xu_sint8_t)(0x81))
#define XU_MAXU8 ((xu_uint8_t)(0xff))
#define XU_MINU8 ((xu_uint8_t)(0))

#define XU_MAXS16 ((xu_sint16_t)(0x7fff))
#define XU_MINS16 ((xu_sint16_t)(0x8001))
#define XU_MAXU16 ((xu_uint16_t)(0xffff))
#define XU_MINU16 ((xu_uint16_t)(0))

#define XU_MAXS32 ((xu_sint32_t)(0x7fffffff))
#define XU_MINS32 ((xu_sint32_t)(0x80000001))
#define XU_MAXU32 ((xu_uint32_t)(0xffffffff))
#define XU_MINU32 ((xu_uint32_t)(0))

#define XU_MAXS64 ((xu_sint64_t)(0x7fffffffffffffffLL))
#define XU_MINS64 ((xu_sint64_t)(0x8000000000000001LL))
#define XU_MAXU64 ((xu_uint64_t)(0xffffffffffffffffULL))
#define XU_MINU64 ((xu_uint64_t)(0))

#define XU_NAN32 (0x80000000)
