#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 * macros
 */
#define xu_round(x)                 ((x) > 0? (xu_int32_t)((x) + 0.5) : (xu_int32_t)((x) - 0.5))
