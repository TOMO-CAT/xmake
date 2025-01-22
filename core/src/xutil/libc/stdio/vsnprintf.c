#include "xutil/libc/misc/ctype.h"
#include "xutil/libc/stdio/printf_object.h"
#include "xutil/libc/stdio/stdio.h"

/* *******************************************************
 * types
 */

// the printf type
typedef enum __xu_printf_type_t
{
    XU_PRINTF_TYPE_NONE         = 0,
    XU_PRINTF_TYPE_INT          = 1,
    XU_PRINTF_TYPE_CHAR         = 2,
    XU_PRINTF_TYPE_CHAR_PERCENT = 3,
    XU_PRINTF_TYPE_FLOAT        = 4,
    XU_PRINTF_TYPE_DOUBLE       = 5,
    XU_PRINTF_TYPE_STRING       = 6,
    XU_PRINTF_TYPE_WIDTH        = 7,
    XU_PRINTF_TYPE_PRECISION    = 8,
    XU_PRINTF_TYPE_OBJECT       = 9,
    XU_PRINTF_TYPE_INVALID      = 10

} xu_printf_type_t;

// the printf extra info
typedef enum __xu_printf_extra_t
{
    XU_PRINTF_EXTRA_NONE   = 0,
    XU_PRINTF_EXTRA_SIGNED = 1 // signed integer for %d %i
    ,
    XU_PRINTF_EXTRA_UPPER = 2 // upper case for %X %B
    ,
    XU_PRINTF_EXTRA_PERCENT = 4 // percent char: %
    ,
    XU_PRINTF_EXTRA_EXP = 8 // exponent form: [-]d.ddd e[+/-]ddd

} xu_printf_extra_t;

// printf length qualifier
typedef enum __xu_printf_qual_t
{
    XU_PRINTF_QUAL_NONE = 0,
    XU_PRINTF_QUAL_H    = 1,
    XU_PRINTF_QUAL_L    = 2,
    XU_PRINTF_QUAL_LL   = 3,
    XU_PRINTF_QUAL_I8   = 4,
    XU_PRINTF_QUAL_I16  = 5,
    XU_PRINTF_QUAL_I32  = 6,
    XU_PRINTF_QUAL_I64  = 7

} xu_printf_qual_t;

// printf flag type
typedef enum __xu_printf_flag_t
{
    XU_PRINTF_FLAG_NONE = 0,
    XU_PRINTF_FLAG_PLUS = 1 // +: denote the sign '+' or '-' of a number
    ,
    XU_PRINTF_FLAG_LEFT = 2 // -: left-justified
    ,
    XU_PRINTF_FLAG_ZERO = 4 // 0: fill 0 instead of spaces
    ,
    XU_PRINTF_FLAG_PFIX = 8 // #: add prefix

} xu_printf_flag_t;

// printf entry
typedef struct __xu_printf_entry_t
{
    // format type
    xu_uint8_t type;

    // extra info
    xu_uint8_t extra;

    // flag
    xu_uint8_t flags;

    // qualifier
    xu_uint8_t qual;

    // field width
    xu_int_t width;

    // precision
    xu_int_t precision;

    // base: 2 8 10 16
    xu_int_t base;

    // the object name
    xu_char_t object[XU_PRINTF_OBJECT_NAME_MAXN];

} xu_printf_entry_t;

/* *******************************************************
 * implementation
 */

static xu_int_t xu_skip_atoi(xu_char_t const** s)
{
    xu_int_t i = 0;
    while (xu_isdigit(**s))
        i = i * 10 + *((*s)++) - '0';
    return i;
}
static xu_char_t* xu_printf_object(xu_char_t* pb, xu_char_t* pe, xu_printf_entry_t e, xu_cpointer_t object)
{
#ifndef XU_CONFIG_MICRO_ENABLE
    // find the object func
    xu_printf_object_func_t func = xu_printf_object_find(e.object);
    if (func)
    {
        // printf it
        xu_long_t size = func(object, pb, pe - pb);
        if (size >= 0)
            pb += size;
        else
        {
            // invalid
            if (pb < pe) *pb++ = 'i';
            if (pb < pe) *pb++ = 'n';
            if (pb < pe) *pb++ = 'v';
            if (pb < pe) *pb++ = 'a';
            if (pb < pe) *pb++ = 'l';
            if (pb < pe) *pb++ = 'i';
            if (pb < pe) *pb++ = 'd';
        }
    }
    else
    {
        // null
        if (pb < pe) *pb++ = 'n';
        if (pb < pe) *pb++ = 'u';
        if (pb < pe) *pb++ = 'l';
        if (pb < pe) *pb++ = 'l';
    }
#else
    if (pb < pe) *pb++ = '%';
    if (pb < pe) *pb++ = '{';
#endif

    // ok?
    return pb;
}
static xu_char_t* xu_printf_string(xu_char_t* pb, xu_char_t* pe, xu_printf_entry_t e, xu_char_t const* s)
{
    // done
    if (s)
    {
        // get length
        xu_long_t n = xu_strnlen(s, e.precision);

        // fill space at left side, e.g. "   abcd"
        if (!(e.flags & XU_PRINTF_FLAG_LEFT))
        {
            while (n < e.width--)
                if (pb < pe) *pb++ = ' ';
        }

        // copy string
        xu_int_t i = 0;
        for (i = 0; i < n; ++i)
            if (pb < pe) *pb++ = *s++;

        // fill space at right side, e.g. "abcd    "
        while (n < e.width--)
            if (pb < pe) *pb++ = ' ';
    }
    else
    {
        // null
        if (pb < pe) *pb++ = 'n';
        if (pb < pe) *pb++ = 'u';
        if (pb < pe) *pb++ = 'l';
        if (pb < pe) *pb++ = 'l';
    }

    return pb;
}
static xu_char_t* xu_printf_int64(xu_char_t* pb, xu_char_t* pe, xu_printf_entry_t e, xu_uint64_t num)
{
    // digits table
    static xu_char_t const* digits_table = "0123456789ABCDEF";

    // max: 64-bit binary decimal
    xu_char_t digits[64] = {0};
    xu_int_t  digit_i    = 0;

    // lowercase mask, e.g. 'F' | 0x20 => 'f'
    xu_int_t lomask = (e.extra & XU_PRINTF_EXTRA_UPPER) ? 0x0 : 0x20;

    // sign: + -
    xu_char_t sign = 0;
    if (e.extra & XU_PRINTF_EXTRA_SIGNED)
    {
        if ((xu_int64_t)num < 0)
        {
            sign = '-';
            --e.width;

            num = (xu_uint64_t)(-(xu_int64_t)num);
        }
        else if (e.flags & XU_PRINTF_FLAG_PLUS)
        {
            sign = '+';
            --e.width;
        }
    }

    // convert num => digits string in reverse order
    if (num == 0)
        digits[digit_i++] = '0';
    else
    {
#if 0
        do
        {
            digits[digit_i++] = digits_table[num % e.base] | lomask;
            num /= e.base;
        }
        while (num) {}
#else
        if (e.base != 10)
        {
            xu_int_t shift_bits = 4;
            if (e.base == 8)
                shift_bits--;
            else if (e.base == 2)
                shift_bits -= 3;
            do
            {
                digits[digit_i++] = digits_table[(xu_uint8_t)num & (e.base - 1)] | lomask;
                num >>= shift_bits;
            } while (num);
        }
        else
        {
            do
            {
                digits[digit_i++] = digits_table[num % e.base] | lomask;
                num /= e.base;
            } while (num);
        }
#endif
    }

    // adjust precision
    if (digit_i > e.precision) e.precision = digit_i;

    // fill spaces at left side, e.g. "   0x0"
    e.width -= e.precision;
    if (!(e.flags & (XU_PRINTF_FLAG_LEFT + XU_PRINTF_FLAG_ZERO)))
    {
        while (--e.width >= 0)
            if (pb < pe) *pb++ = ' ';
    }

    // append sign: + / -
    if (sign && (pb < pe)) *pb++ = sign;

    // append prefix: 0x..., 0X..., 0b..., 0B...
    if (e.flags & XU_PRINTF_FLAG_PFIX)
    {
        switch (e.base)
        {
        case 16:
        {
            if (pb + 1 < pe)
            {
                *pb++ = '0';
                *pb++ = 'X' | lomask;
                e.width -= 2;
            }
            break;
        }
        case 8:
        {
            if (pb < pe)
            {
                *pb++ = '0';
                --e.width;
            }
            break;
        }
        case 2:
        {
            if (pb + 1 < pe)
            {
                *pb++ = '0';
                *pb++ = 'B' | lomask;
                e.width -= 2;
            }
            break;
        }
        default: break;
        }
    }

    // fill 0 or spaces, e.g. "0x   ff"
    if (!(e.flags & XU_PRINTF_FLAG_LEFT))
    {
        xu_char_t c = (e.flags & XU_PRINTF_FLAG_ZERO) ? '0' : ' ';
        while (--e.width >= 0)
            if (pb < pe) *pb++ = c;
    }

    // fill 0 if precision is larger, e.g. "0x000ff"
    while (digit_i <= --e.precision)
        if (pb < pe) *pb++ = '0';

    // append digits
    while (--digit_i >= 0)
        if (pb < pe) *pb++ = digits[digit_i];

    // trailing space padding for left-justified flags, e.g. "0xff   "
    while (--e.width >= 0)
        if (pb < pe) *pb++ = ' ';

    return pb;
}
static xu_char_t* xu_printf_int32(xu_char_t* pb, xu_char_t* pe, xu_printf_entry_t e, xu_uint32_t num)
{
    // digits table
    static xu_char_t const* digits_table = "0123456789ABCDEF";

    // max: 64-bit binary decimal
    xu_char_t digits[64] = {0};
    xu_int_t  digit_i    = 0;

    // lowercase mask, e.g. 'F' | 0x20 => 'f'
    xu_int_t lomask = (e.extra & XU_PRINTF_EXTRA_UPPER) ? 0x0 : 0x20;

    // sign: + -
    xu_char_t sign = 0;
    if (e.extra & XU_PRINTF_EXTRA_SIGNED)
    {
        if ((xu_int32_t)num < 0)
        {
            sign = '-';
            --e.width;
            num = (xu_uint32_t)(-(xu_int32_t)num);
        }
        else if (e.flags & XU_PRINTF_FLAG_PLUS)
        {
            sign = '+';
            --e.width;
        }
    }

    // convert num => digits string in reverse order
    if (num == 0)
        digits[digit_i++] = '0';
    else
    {
#if 0
        do
        {
            digits[digit_i++] = digits_table[num % e.base] | lomask;
            num /= e.base;
        }
        while (num) {}
#else
        if (e.base != 10)
        {
            xu_int_t shift_bits = 4;
            if (e.base == 8)
                shift_bits--;
            else if (e.base == 2)
                shift_bits -= 3;
            do
            {
                digits[digit_i++] = digits_table[(xu_uint8_t)num & (e.base - 1)] | lomask;
                num >>= shift_bits;
            } while (num);
        }
        else
        {
            do
            {
                digits[digit_i++] = digits_table[num % e.base] | lomask;
                num /= e.base;
            } while (num);
        }
#endif
    }

    // adjust precision
    if (digit_i > e.precision) e.precision = digit_i;

    // fill spaces at left side, e.g. "   0x0"
    e.width -= e.precision;
    if (!(e.flags & (XU_PRINTF_FLAG_LEFT + XU_PRINTF_FLAG_ZERO)))
    {
        while (--e.width >= 0)
            if (pb < pe) *pb++ = ' ';
    }

    // append sign: + / -
    if (sign && (pb < pe)) *pb++ = sign;

    // append prefix: 0x..., 0X..., 0b..., 0B...
    if (e.flags & XU_PRINTF_FLAG_PFIX)
    {
        switch (e.base)
        {
        case 16:
        {
            if (pb + 1 < pe)
            {
                *pb++ = '0';
                *pb++ = 'X' | lomask;
                e.width -= 2;
            }
            break;
        }
        case 8:
        {
            if (pb < pe)
            {
                *pb++ = '0';
                --e.width;
            }
            break;
        }
        case 2:
        {
            if (pb + 1 < pe)
            {
                *pb++ = '0';
                *pb++ = 'B' | lomask;
                e.width -= 2;
            }
            break;
        }
        default: break;
        }
    }

    // fill 0 or spaces, e.g. "0x   ff"
    if (!(e.flags & XU_PRINTF_FLAG_LEFT))
    {
        xu_char_t c = (e.flags & XU_PRINTF_FLAG_ZERO) ? '0' : ' ';
        while (--e.width >= 0)
            if (pb < pe) *pb++ = c;
    }

    // fill 0 if precision is larger, e.g. "0x000ff"
    while (digit_i <= --e.precision)
        if (pb < pe) *pb++ = '0';

    // append digits
    while (--digit_i >= 0)
        if (pb < pe) *pb++ = digits[digit_i];

    // trailing space padding for left-justified flags, e.g. "0xff   "
    while (--e.width >= 0)
        if (pb < pe) *pb++ = ' ';

    return pb;
}
#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
static xu_char_t* xu_printf_float(xu_char_t* pb, xu_char_t* pe, xu_printf_entry_t e, xu_float_t num)
{
    // digits
    xu_char_t ints[32] = {0};
    xu_char_t decs[32] = {0};
    xu_int_t  ints_i = 0, decs_i = 0;

    // for inf nan
    if (xu_isinff(num))
    {
        if (pb < pe && num < 0) *pb++ = '-';
        if (pb < pe) *pb++ = (e.extra & XU_PRINTF_EXTRA_UPPER) ? 'I' : 'i';
        if (pb < pe) *pb++ = (e.extra & XU_PRINTF_EXTRA_UPPER) ? 'N' : 'n';
        if (pb < pe) *pb++ = (e.extra & XU_PRINTF_EXTRA_UPPER) ? 'F' : 'f';
        return pb;
    }
    else if (xu_isnanf(num))
    {
        if (pb < pe) *pb++ = (e.extra & XU_PRINTF_EXTRA_UPPER) ? 'N' : 'n';
        if (pb < pe) *pb++ = (e.extra & XU_PRINTF_EXTRA_UPPER) ? 'A' : 'a';
        if (pb < pe) *pb++ = (e.extra & XU_PRINTF_EXTRA_UPPER) ? 'N' : 'n';
        return pb;
    }

    // sign: + -
    xu_char_t sign = 0;
    if (e.extra & XU_PRINTF_EXTRA_SIGNED)
    {
        if (num < 0)
        {
            sign = '-';
            --e.width;
        }
        else if (e.flags & XU_PRINTF_FLAG_PLUS)
        {
            sign = '+';
            --e.width;
        }
    }

    // adjust sign
    if (num < 0) num = -num;

    // default precision: 6
    if (e.precision <= 0) e.precision = 6;

    // round? i.dddddddd5 => i.ddddddde
    xu_uint32_t p = 1;
    xu_size_t   n = e.precision;
    while (n--)
        p *= 10;
    if (((xu_uint32_t)(num * p * 10) % 10) > 4) num += 1.0f / (xu_float_t)p;

    // get integer & decimal
    xu_int32_t integer = (xu_int32_t)num;
    xu_float_t decimal = num - integer;

    // convert integer => digits string in reverse order
    if (integer == 0)
        ints[ints_i++] = '0';
    else
    {
        if (integer < 0) integer = -integer;
        do
        {
            ints[ints_i++] = (integer % 10) + '0';
            integer /= 10;
        } while (integer);
    }

    // convert decimal => digits string in positive order
    if (decimal == 0)
        decs[decs_i++] = '0';
    else
    {
        xu_long_t d = (xu_long_t)(decimal * 10);
        do
        {
            decs[decs_i++] = (xu_char_t)(d + (xu_long_t)'0');
            decimal        = decimal * 10 - d;
            d              = (xu_long_t)(decimal * 10);
        } while (decs_i < e.precision);
    }

    // adjust precision
    if (decs_i > e.precision) decs_i = e.precision;

    // fill spaces at left side, e.g. "   0.31415926"
    e.width -= (xu_int_t)(ints_i + 1 + e.precision);
    if (!(e.flags & (XU_PRINTF_FLAG_LEFT + XU_PRINTF_FLAG_ZERO)))
    {
        while (--e.width >= 0)
            if (pb < pe) *pb++ = ' ';
    }

    // append sign: + / -
    if (sign && (pb < pe)) *pb++ = sign;

    // fill 0 or spaces, e.g. "00003.1415926"
    if (!(e.flags & XU_PRINTF_FLAG_LEFT))
    {
        xu_char_t c = (e.flags & XU_PRINTF_FLAG_ZERO) ? '0' : ' ';
        while (--e.width >= 0)
            if (pb < pe) *pb++ = c;
    }

    // append integer
    while (--ints_i >= 0)
        if (pb < pe) *pb++ = ints[ints_i];

    // append .
    if (pb < pe) *pb++ = '.';

    // append decimal
    xu_int_t decs_n = decs_i;
    while (--decs_i >= 0)
        if (pb < pe) *pb++ = decs[decs_n - decs_i - 1];

    // fill 0 if precision is larger, e.g. "0.3140000"
    while (decs_n <= --e.precision)
        if (pb < pe) *pb++ = '0';

    // trailing space padding for left-justified flags, e.g. "0.31415926   "
    while (--e.width >= 0)
        if (pb < pe) *pb++ = ' ';

    return pb;
}
static xu_char_t* xu_printf_double(xu_char_t* pb, xu_char_t* pe, xu_printf_entry_t e, xu_double_t num)
{
    // digits
    xu_char_t ints[64] = {0};
    xu_char_t decs[64] = {0};
    xu_int_t  ints_i = 0, decs_i = 0;

    // for inf nan
    if (xu_isinf(num))
    {
        if (pb < pe && num < 0) *pb++ = '-';
        if (pb < pe) *pb++ = (e.extra & XU_PRINTF_EXTRA_UPPER) ? 'I' : 'i';
        if (pb < pe) *pb++ = (e.extra & XU_PRINTF_EXTRA_UPPER) ? 'N' : 'n';
        if (pb < pe) *pb++ = (e.extra & XU_PRINTF_EXTRA_UPPER) ? 'F' : 'f';
        return pb;
    }
    else if (xu_isnan(num))
    {
        if (pb < pe) *pb++ = (e.extra & XU_PRINTF_EXTRA_UPPER) ? 'N' : 'n';
        if (pb < pe) *pb++ = (e.extra & XU_PRINTF_EXTRA_UPPER) ? 'A' : 'a';
        if (pb < pe) *pb++ = (e.extra & XU_PRINTF_EXTRA_UPPER) ? 'N' : 'n';
        return pb;
    }

    // sign: + -
    xu_char_t sign = 0;
    if (e.extra & XU_PRINTF_EXTRA_SIGNED)
    {
        if (num < 0)
        {
            sign = '-';
            --e.width;
        }
        else if (e.flags & XU_PRINTF_FLAG_PLUS)
        {
            sign = '+';
            --e.width;
        }
    }

    // adjust sign
    if (num < 0) num = -num;

    // default precision: 6
    if (e.precision <= 0) e.precision = 6;

    // round? i.dddddddd5 => i.ddddddde
    xu_uint64_t p = 1;
    xu_size_t   n = e.precision;
    while (n--)
        p *= 10;
    if (((xu_uint64_t)(num * p * 10) % 10) > 4) num += 1. / (xu_double_t)p;

    // get integer & decimal
    xu_int64_t  integer = (xu_int64_t)num;
    xu_double_t decimal = num - integer;

    // convert integer => digits string in reverse order
    if (integer == 0)
        ints[ints_i++] = '0';
    else
    {
        if (integer < 0) integer = -integer;
        do
        {
            ints[ints_i++] = (integer % 10) + '0';
            integer /= 10;
        } while (integer);
    }

    // convert decimal => digits string in positive order
    if (decimal == 0)
        decs[decs_i++] = '0';
    else
    {
        xu_long_t d = (xu_long_t)(decimal * 10);
        do
        {
            decs[decs_i++] = (xu_char_t)(d + (xu_long_t)'0');
            decimal        = decimal * 10 - d;
            d              = (xu_long_t)(decimal * 10);
        } while (decs_i < e.precision);
    }

    // adjust precision
    if (decs_i > e.precision) decs_i = e.precision;

    // fill spaces at left side, e.g. "   0.31415926"
    e.width -= (xu_int_t)(ints_i + 1 + e.precision);
    if (!(e.flags & (XU_PRINTF_FLAG_LEFT + XU_PRINTF_FLAG_ZERO)))
    {
        while (--e.width >= 0)
            if (pb < pe) *pb++ = ' ';
    }

    // append sign: + / -
    if (sign && (pb < pe)) *pb++ = sign;

    // fill 0 or spaces, e.g. "00003.1415926"
    if (!(e.flags & XU_PRINTF_FLAG_LEFT))
    {
        xu_char_t c = (e.flags & XU_PRINTF_FLAG_ZERO) ? '0' : ' ';
        while (--e.width >= 0)
            if (pb < pe) *pb++ = c;
    }

    // append integer
    while (--ints_i >= 0)
        if (pb < pe) *pb++ = ints[ints_i];

    // append .
    if (pb < pe) *pb++ = '.';

    // append decimal
    xu_int_t decs_n = decs_i;
    while (--decs_i >= 0)
        if (pb < pe) *pb++ = decs[decs_n - decs_i - 1];

    // fill 0 if precision is larger, e.g. "0.3140000"
    while (decs_n <= --e.precision)
        if (pb < pe) *pb++ = '0';

    // trailing space padding for left-justified flags, e.g. "0.31415926   "
    while (--e.width >= 0)
        if (pb < pe) *pb++ = ' ';

    return pb;
}
#endif
// get a printf format entry
static xu_int_t xu_printf_entry(xu_char_t const* fmt, xu_printf_entry_t* e)
{
    xu_char_t const* p = fmt;

    // get field width for *
    if (e->type == XU_PRINTF_TYPE_WIDTH)
    {
        if (e->width < 0)
        {
            e->width = -e->width;
            e->flags |= XU_PRINTF_FLAG_LEFT;
        }
        e->type = XU_PRINTF_TYPE_NONE;
        goto get_precision;
    }

    // get precision for *
    if (e->type == XU_PRINTF_TYPE_PRECISION)
    {
        if (e->precision < 0) e->precision = 0;
        e->type = XU_PRINTF_TYPE_NONE;
        goto get_qualifier;
    }

    // default type
    e->type = XU_PRINTF_TYPE_NONE;

    // goto %
    for (; *p; ++p)
        if (*p == '%') break;

    // return non-format string
    if (p != fmt || !*p) return (xu_int_t)(p - fmt);

    // skip %
    ++p;

    // get flags
    e->flags = XU_PRINTF_FLAG_NONE;
    while (1)
    {
        xu_bool_t is_found = xu_true;
        switch (*p)
        {
        case '+': e->flags |= XU_PRINTF_FLAG_PLUS; break;
        case '-': e->flags |= XU_PRINTF_FLAG_LEFT; break;
        case '0': e->flags |= XU_PRINTF_FLAG_ZERO; break;
        case '#': e->flags |= XU_PRINTF_FLAG_PFIX; break;
        default: is_found = xu_false; break;
        }
        if (is_found == xu_false)
            break;
        else
            ++p;
    }

    // get field width
    e->width = -1;
    if (xu_isdigit(*p))
        e->width = xu_skip_atoi(&p);
    else if (*p == '*')
    {
        // it's the next argument
        e->type = XU_PRINTF_TYPE_WIDTH;
        return (xu_int_t)(++p - fmt);
    }

get_precision:
    // get precision
    e->precision = -1;
    if (*p == '.')
    {
        ++p;
        if (xu_isdigit(*p))
        {
            e->precision = xu_skip_atoi(&p);
            if (e->precision < 0) e->precision = 0;
        }
        else if (*p == '*')
        {
            // it's the next argument
            e->type = XU_PRINTF_TYPE_PRECISION;
            return (xu_int_t)(++p - fmt);
        }
    }

get_qualifier:
    // get length qualifier
    e->qual = XU_PRINTF_QUAL_NONE;
    switch (*p)
    {
        // short & long => int
    case 'h':
        e->qual = XU_PRINTF_QUAL_H;
        ++p;
        break;
    case 'l':
        e->qual = XU_PRINTF_QUAL_L;
        ++p;
        if (*p == 'l')
        {
            e->qual = XU_PRINTF_QUAL_LL;
            ++p;
        }
        break;
    case 'I':
    {
        ++p;
        xu_int_t n = xu_skip_atoi(&p);
        switch (n)
        {
        case 8: e->qual = XU_PRINTF_QUAL_I8; break;
        case 16: e->qual = XU_PRINTF_QUAL_I16; break;
        case 32: e->qual = XU_PRINTF_QUAL_I32; break;
        case 64: e->qual = XU_PRINTF_QUAL_I64; break;
        default: break;
        }
        break;
    }
    case 'z':
        switch (sizeof(xu_size_t))
        {
        case 1: e->qual = XU_PRINTF_QUAL_I8; break;
        case 2: e->qual = XU_PRINTF_QUAL_I16; break;
        case 4: e->qual = XU_PRINTF_QUAL_I32; break;
        case 8: e->qual = XU_PRINTF_QUAL_I64; break;
        default: break;
        }
        ++p;
        break;
    default: e->qual = XU_PRINTF_QUAL_NONE; break;
    }

    // get base & type
    e->base  = -1;
    e->type  = XU_PRINTF_TYPE_INVALID;
    e->extra = XU_PRINTF_EXTRA_NONE;
    switch (*p)
    {
    case 's': e->type = XU_PRINTF_TYPE_STRING; return (xu_int_t)(++p - fmt);
    case '%': e->extra |= XU_PRINTF_EXTRA_PERCENT;
    case 'c': e->type = XU_PRINTF_TYPE_CHAR; return (xu_int_t)(++p - fmt);
    case 'd':
    case 'i': e->extra |= XU_PRINTF_EXTRA_SIGNED;
    case 'u':
        e->base = 10;
        e->type = XU_PRINTF_TYPE_INT;
        break;
    case 'X': e->extra |= XU_PRINTF_EXTRA_UPPER;
    case 'x':
        e->base = 16;
        e->type = XU_PRINTF_TYPE_INT;
        break;
    case 'P': e->extra |= XU_PRINTF_EXTRA_UPPER;
    case 'p':
        e->base = 16;
        e->type = XU_PRINTF_TYPE_INT;
        e->flags |= XU_PRINTF_FLAG_PFIX;
#if XU_CPU_BITSIZE == 64
        e->qual = XU_PRINTF_QUAL_I64;
#endif
        break;
    case 'o':
        e->base = 8;
        e->type = XU_PRINTF_TYPE_INT;
        break;
    case 'B': e->extra |= XU_PRINTF_EXTRA_UPPER;
    case 'b':
        e->base = 2;
        e->type = XU_PRINTF_TYPE_INT;
        break;
#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
    case 'F': e->extra |= XU_PRINTF_EXTRA_UPPER;
    case 'f':
        e->type = XU_PRINTF_TYPE_FLOAT;
        e->extra |= XU_PRINTF_EXTRA_SIGNED;
        break;
    case 'E': e->extra |= XU_PRINTF_EXTRA_UPPER;
    case 'e':
        e->type = XU_PRINTF_TYPE_FLOAT;
        e->extra |= XU_PRINTF_EXTRA_SIGNED;
        e->extra |= XU_PRINTF_EXTRA_EXP;
        break;
#endif
    case '{':
    {
#ifdef XU_CONFIG_MICRO_ENABLE
        e->type = XU_PRINTF_TYPE_OBJECT;
#else
        // get the object name
        ++p;
        xu_size_t indx = 0;
        xu_size_t maxn = xu_arrayn(e->object);
        while (*p && *p != '}' && indx < maxn - 1)
            e->object[indx++] = *p++;
        e->object[indx] = '\0';

        // save the object type
        e->type = *p == '}' ? XU_PRINTF_TYPE_OBJECT : XU_PRINTF_TYPE_INVALID;
#endif
    }
    break;
    default: e->type = XU_PRINTF_TYPE_INVALID; return (xu_int_t)(p - fmt);
    }

    return (xu_int_t)(++p - fmt);
}

/* *******************************************************
 * interfaces
 */

xu_long_t xu_vsnprintf(xu_char_t* s, xu_size_t n, xu_char_t const* fmt, xu_va_list_t args)
{
    // check
    if (!n || !s || !fmt) return 0;

    // init start and end pointer
    xu_char_t* pb = s;
    xu_char_t* pe = s + n - 1;

#if 0
    // pe must be larger than pb
    if (pe < pb)
    {
        pe = ((xu_char_t*)-1);
        n = (xu_size_t)(pe - pb);
    }
#endif

    // parse format
    xu_printf_entry_t e  = {0};
    xu_int_t          en = 0;
    while (*fmt)
    {
        xu_char_t const* ofmt = fmt;

        // get an entry
        en = xu_printf_entry(fmt, &e);
        fmt += en;

        switch (e.type)
        {
            // copy it if none type
        case XU_PRINTF_TYPE_NONE:
        {
            xu_int_t copy_n = en;
            if (pb < pe)
            {
                if (copy_n > pe - pb) copy_n = (xu_int_t)(pe - pb);
                xu_memcpy(pb, ofmt, copy_n);
                pb += copy_n;
            }
            break;
        }
            // get a character for %c
        case XU_PRINTF_TYPE_CHAR:
        {
            // char: %
            if (e.extra & XU_PRINTF_EXTRA_PERCENT)
            {
                if (pb < pe) *pb++ = '%';
            }
            // char: %c
            else
            {
                // fill space at left side, e.g. "   a"
                if (!(e.flags & XU_PRINTF_FLAG_LEFT))
                {
                    while (--e.width > 0)
                    {
                        if (pb < pe) *pb++ = ' ';
                    }
                }

                if (pb < pe) *pb++ = (xu_char_t)xu_va_arg(args, xu_int_t);

                // fill space at right side, e.g. "a   "
                while (--e.width > 0)
                {
                    if (pb < pe) *pb++ = ' ';
                }
            }
            break;
        }
            // get field width for *
        case XU_PRINTF_TYPE_WIDTH:
            e.width = xu_va_arg(args, xu_int_t);
            break;
            // get precision for *
        case XU_PRINTF_TYPE_PRECISION:
            e.precision = xu_va_arg(args, xu_int_t);
            break;
            // get string for %s
        case XU_PRINTF_TYPE_STRING:
        {
            pb = xu_printf_string(pb, pe, e, xu_va_arg(args, xu_char_t const*));
            break;
        }
            // get an integer for %d %u %x ...
        case XU_PRINTF_TYPE_INT:
        {
            if (e.qual == XU_PRINTF_QUAL_I64
#if XU_CPU_BIT64
                || e.qual == XU_PRINTF_QUAL_L
#endif
                || e.qual == XU_PRINTF_QUAL_LL)
                pb = xu_printf_int64(pb, pe, e, xu_va_arg(args, xu_uint64_t));
            else
            {
                xu_uint32_t num = 0;
                if (e.extra & XU_PRINTF_EXTRA_SIGNED)
                {
                    switch (e.qual)
                    {
                    case XU_PRINTF_QUAL_I8: num = (xu_int8_t)xu_va_arg(args, xu_int_t); break;
                    case XU_PRINTF_QUAL_I16: num = (xu_int16_t)xu_va_arg(args, xu_int_t); break;
                    case XU_PRINTF_QUAL_I32: num = (xu_int32_t)xu_va_arg(args, xu_int32_t); break;
                    default: num = xu_va_arg(args, xu_int_t); break;
                    }
                }
                else
                {
                    switch (e.qual)
                    {
                    case XU_PRINTF_QUAL_I8: num = (xu_uint8_t)xu_va_arg(args, xu_uint_t); break;
                    case XU_PRINTF_QUAL_I16: num = (xu_uint16_t)xu_va_arg(args, xu_uint_t); break;
                    case XU_PRINTF_QUAL_I32: num = xu_va_arg(args, xu_uint32_t); break;
                    default: num = xu_va_arg(args, xu_uint_t); break;
                    }
                }
                pb = xu_printf_int32(pb, pe, e, num);
            }
            break;
        }
#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
        case XU_PRINTF_TYPE_FLOAT:
        {
            // double?
            if (e.qual == XU_PRINTF_QUAL_L)
            {
                xu_double_t num = xu_va_arg(args, xu_double_t);
                pb              = xu_printf_double(pb, pe, e, num);
            }
            // float?
            else
            {
                xu_float_t num = (xu_float_t)xu_va_arg(args, xu_double_t);
                pb             = xu_printf_float(pb, pe, e, num);
            }
            break;
        }
#endif
            // get object for %{object_name}
        case XU_PRINTF_TYPE_OBJECT:
        {
            pb = xu_printf_object(pb, pe, e, xu_va_arg(args, xu_cpointer_t));
            break;
        }
        case XU_PRINTF_TYPE_INVALID:
        {
            if (pb < pe) *pb++ = '%';
            break;
        }
        default: break;
        }
    }

    // end
    if (pb < s + n) *pb = '\0';

    // the trailing null byte doesn't count towards the total
    return (pb - s);
}
