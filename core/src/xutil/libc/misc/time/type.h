#pragma once

#include "xutil/macros/type.h"
#include "xutil/prefix.h"

/* *******************************************************
 *  types
 */

// the tm type
typedef struct __xu_tm_t
{
    xu_long_t second;
    xu_long_t minute;
    xu_long_t hour;
    xu_long_t mday;
    xu_long_t month;
    xu_long_t year;
    xu_long_t week;
    xu_long_t yday;
    xu_long_t isdst;

} xu_tm_t;
