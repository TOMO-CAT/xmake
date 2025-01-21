#pragma once

#include "xutil/prefix.h"

// is
#define xu_isspace(x) (((x) == 0x20) || ((x) > 0x8 && (x) < 0xe))
#define xu_isgraph(x) ((x) > 0x1f && (x) < 0x7f)
#define xu_isalpha(x) (((x) > 0x40 && (x) < 0x5b) || ((x) > 0x60 && (x) < 0x7b))
#define xu_isupper(x) ((x) > 0x40 && (x) < 0x5b)
#define xu_islower(x) ((x) > 0x60 && (x) < 0x7b)
#define xu_isascii(x) ((x) >= 0x0 && (x) < 0x80)
#define xu_isdigit(x) ((x) > 0x2f && (x) < 0x3a)
#define xu_isdigit2(x) ((x) == '0' || (x) == '1')
#define xu_isdigit8(x) (((x) > 0x2f && (x) < 0x38))
#define xu_isdigit10(x) (xu_isdigit(x))
#define xu_isdigit16(x) (((x) > 0x2f && (x) < 0x3a) || ((x) > 0x40 && (x) < 0x47) || ((x) > 0x60 && (x) < 0x67))

// to lower & upper
#define xu_tolower(x) (xu_isupper(x) ? (x) + 0x20 : (x))
#define xu_toupper(x) (xu_islower(x) ? (x)-0x20 : (x))
