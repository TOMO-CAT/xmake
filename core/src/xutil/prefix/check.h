#pragma once

#include "xutil/config.h"

/* *******************************************************
 *  the trace prefix
 * *******************************************************
 */
#define xu_check_return(x)                                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(x)) return;                                                                                              \
    } while (0)
#define xu_check_return_val(x, v)                                                                                      \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(x)) return (v);                                                                                          \
    } while (0)
#define xu_check_goto(x, b)                                                                                            \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(x)) goto b;                                                                                              \
    } while (0)
#define xu_check_break(x)                                                                                              \
    {                                                                                                                  \
        if (!(x)) break;                                                                                               \
    }
#define xu_check_abort(x)                                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(x)) xu_abort();                                                                                          \
    } while (0)
#define xu_check_continue(x)                                                                                           \
    {                                                                                                                  \
        if (!(x)) continue;                                                                                            \
    }
#define xu_check_break_state(x, s, v)                                                                                  \
    {                                                                                                                  \
        if (!(x))                                                                                                      \
        {                                                                                                              \
            (s) = (v);                                                                                                 \
            break;                                                                                                     \
        }                                                                                                              \
    }
