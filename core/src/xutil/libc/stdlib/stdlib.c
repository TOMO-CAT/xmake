#include "xutil/libc/stdlib/stdlib.h"

xu_uint64_t xu_s2tou64(xu_char_t const* s)
{
    // check
    xu_assert_and_check_return_val(s, 0);

    // skip space
    while (xu_isspace(*s))
        s++;

    // has sign?
    xu_int_t sign = 0;
    if (*s == '-')
    {
        sign = 1;
        s++;
    }
    // skip '+'
    else if (*s == '+')
        s++;

    // skip "0b"
    if (s[0] == '0' && (s[1] == 'b' || s[1] == 'B')) s += 2;

    // skip '0'
    while ((*s) == '0')
        s++;

    // compute number
    xu_uint64_t val = 0;
    while (*s)
    {
        xu_char_t ch = *s;
        if (xu_isdigit2(ch))
            val = (val << 1) + (ch - '0');
        else
            break;

        s++;
    }

    // is negative number?
    if (sign) val = ~val + 1;

    // the value
    return val;
}
xu_uint64_t xu_s8tou64(xu_char_t const* s)
{
    // check
    xu_assert_and_check_return_val(s, 0);

    // skip space
    while (xu_isspace(*s))
        s++;

    // has sign?
    xu_int_t sign = 0;
    if (*s == '-')
    {
        sign = 1;
        s++;
    }
    // skip '+'
    else if (*s == '+')
        s++;

    // skip '0'
    while ((*s) == '0')
        s++;

    // compute number
    xu_uint64_t val = 0;
    while (*s)
    {
        xu_char_t ch = *s;
        if (xu_isdigit8(ch))
            val = (val << 3) + (ch - '0');
        else
            break;

        s++;
    }

    // is negative number?
    if (sign) val = ~val + 1;

    // the value
    return val;
}
xu_uint64_t xu_s10tou64(xu_char_t const* s)
{
    // check
    xu_assert_and_check_return_val(s, 0);

    // skip space
    while (xu_isspace(*s))
        s++;

    // has sign?
    xu_int_t sign = 0;
    if (*s == '-')
    {
        sign = 1;
        s++;
    }
    // skip '+'
    else if (*s == '+')
        s++;

    // skip '0'
    while ((*s) == '0')
        s++;

    // compute number
    xu_uint64_t val = 0;
    while (*s)
    {
        xu_char_t ch = *s;
        if (xu_isdigit10(ch))
            val = val * 10 + (ch - '0');
        else
            break;

        s++;
    }

    // is negative number?
    if (sign) val = ~val + 1;

    // the value
    return val;
}
xu_uint64_t xu_s16tou64(xu_char_t const* s)
{
    // check
    xu_assert_and_check_return_val(s, 0);

    // skip space
    while (xu_isspace(*s))
        s++;

    // has sign?
    xu_int_t sign = 0;
    if (*s == '-')
    {
        sign = 1;
        s++;
    }
    // skip '+'
    else if (*s == '+')
        s++;

    // skip "0x"
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) s += 2;

    // skip '0'
    while ((*s) == '0')
        s++;

    // compute number
    xu_uint64_t val = 0;
    while (*s)
    {
        xu_char_t ch = *s;
        if (xu_isdigit10(ch))
            val = (val << 4) + (ch - '0');
        else if (ch > ('a' - 1) && ch < ('f' + 1))
            val = (val << 4) + (ch - 'a') + 10;
        else if (ch > ('A' - 1) && ch < ('F' + 1))
            val = (val << 4) + (ch - 'A') + 10;
        else
            break;

        s++;
    }

    // is negative number?
    if (sign) val = ~val + 1;

    // the value
    return val;
}
xu_uint64_t xu_stou64(xu_char_t const* s)
{
    // check
    xu_assert_and_check_return_val(s, 0);

    // skip space
    xu_char_t const* p = s;
    while (xu_isspace(*p))
        p++;

    // has sign?
    if (*p == '-' || *p == '+') p++;

    // is hex?
    if (*p++ == '0')
    {
        if (*p == 'x' || *p == 'X')
            return xu_s16tou64(s);
        else if (*p == 'b' || *p == 'B')
            return xu_s2tou64(s);
        else
            return xu_s8tou64(s);
    }
    else
        return xu_s10tou64(s);
}
xu_uint64_t xu_sbtou64(xu_char_t const* s, xu_int_t base)
{
    // check
    xu_assert_and_check_return_val(s, 0);

    // the convect functions
    static xu_uint64_t (*s_conv[])(xu_char_t const*) = {xu_null, xu_null, xu_s2tou64, xu_null, xu_null,     xu_null,
                                                        xu_null, xu_null, xu_s8tou64, xu_null, xu_s10tou64, xu_null,
                                                        xu_null, xu_null, xu_null,    xu_null, xu_s16tou64};
    xu_assert_and_check_return_val(base < xu_arrayn(s_conv) && s_conv[base], 0);

    // convect it
    return s_conv[base](s);
}
#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
xu_double_t xu_s2tod(xu_char_t const* s)
{
    // check
    xu_assert_and_check_return_val(s, 0);

    // skip space
    while (xu_isspace(*s))
        s++;

    // has sign?
    xu_int_t sign = 0;
    if (*s == '-')
    {
        sign = 1;
        s++;
    }
    // skip '+'
    else if (*s == '+')
        s++;

    // nan?
    if (s[0] == 'n' && s[1] == 'a' && s[2] == 'n') return XU_NAN;

    // inf or -inf?
    if (s[0] == 'i' && s[1] == 'n' && s[2] == 'f') return sign ? -XU_INF : XU_INF;

    // skip "0b"
    if (s[0] == '0' && (s[1] == 'b' || s[1] == 'B')) s += 2;

    // compute double: lhs.rhs
    xu_int_t    dec   = 0;
    xu_uint64_t lhs   = 0;
    xu_double_t rhs   = 0.;
    xu_int_t    zeros = 0;
    xu_int8_t   decimals[256];
    xu_int8_t*  d = decimals;
    xu_int8_t*  e = decimals + 256;
    while (*s)
    {
        xu_char_t ch = *s;

        // is the part of decimal?
        if (ch == '.')
        {
            if (!dec)
            {
                dec = 1;
                s++;
                continue;
            }
            else
                break;
        }

        // parse integer & decimal
        if (xu_isdigit2(ch))
        {
            // save decimals
            if (dec)
            {
                if (d < e)
                {
                    if (ch != '0')
                    {
                        // fill '0'
                        while (zeros--)
                            *d++ = 0;
                        zeros = 0;

                        // save decimal
                        *d++ = ch - '0';
                    }
                    else
                        zeros++;
                }
            }
            else
                lhs = (lhs << 1) + (ch - '0');
        }
        else
            break;

        s++;
    }

    xu_assert(d <= decimals + 256);

    // compute decimal
    while (d-- > decimals)
        rhs = (rhs + *d) / 2;

    // merge
    return (sign ? ((xu_double_t)lhs + rhs) * -1. : ((xu_double_t)lhs + rhs));
}
xu_double_t xu_s8tod(xu_char_t const* s)
{
    // check
    xu_assert_and_check_return_val(s, 0);

    // skip space
    while (xu_isspace(*s))
        s++;

    // has sign?
    xu_int_t sign = 0;
    if (*s == '-')
    {
        sign = 1;
        s++;
    }
    // skip '+'
    else if (*s == '+')
        s++;

    // nan?
    if (s[0] == 'n' && s[1] == 'a' && s[2] == 'n') return XU_NAN;

    // inf or -inf?
    if (s[0] == 'i' && s[1] == 'n' && s[2] == 'f') return sign ? -XU_INF : XU_INF;

    // skip '0'
    while ((*s) == '0')
        s++;

    // compute double: lhs.rhs
    xu_int_t    dec   = 0;
    xu_uint64_t lhs   = 0;
    xu_double_t rhs   = 0.;
    xu_int_t    zeros = 0;
    xu_int8_t   decimals[256];
    xu_int8_t*  d = decimals;
    xu_int8_t*  e = decimals + 256;
    while (*s)
    {
        xu_char_t ch = *s;

        // is the part of decimal?
        if (ch == '.')
        {
            if (!dec)
            {
                dec = 1;
                s++;
                continue;
            }
            else
                break;
        }

        // parse integer & decimal
        if (xu_isdigit8(ch))
        {
            // save decimals
            if (dec)
            {
                if (d < e)
                {
                    if (ch != '0')
                    {
                        // fill '0'
                        while (zeros--)
                            *d++ = 0;
                        zeros = 0;

                        // save decimal
                        *d++ = ch - '0';
                    }
                    else
                        zeros++;
                }
            }
            else
                lhs = (lhs << 3) + (ch - '0');
        }
        else
            break;

        s++;
    }

    // check
    xu_assert_and_check_return_val(d <= decimals + 256, 0);

    // compute decimal
    while (d-- > decimals)
        rhs = (rhs + *d) / 8;

    // merge
    return (sign ? ((xu_double_t)lhs + rhs) * -1. : ((xu_double_t)lhs + rhs));
}
xu_double_t xu_s10tod(xu_char_t const* s)
{
    // check
    xu_assert_and_check_return_val(s, 0);

    // skip space
    while (xu_isspace(*s))
        s++;

    // has sign?
    xu_int_t sign = 0;
    if (*s == '-')
    {
        sign = 1;
        s++;
    }
    // skip '+'
    else if (*s == '+')
        s++;

    // nan?
    if (s[0] == 'n' && s[1] == 'a' && s[2] == 'n') return XU_NAN;

    // inf or -inf?
    if (s[0] == 'i' && s[1] == 'n' && s[2] == 'f') return sign ? -XU_INF : XU_INF;

    // skip '0'
    while ((*s) == '0')
        s++;

    // compute double: lhs.rhs
    xu_int_t    dec   = 0;
    xu_uint64_t lhs   = 0;
    xu_double_t rhs   = 0.;
    xu_int_t    zeros = 0;
    xu_int8_t   decimals[256];
    xu_int8_t*  d = decimals;
    xu_int8_t*  e = decimals + 256;
    while (*s)
    {
        xu_char_t ch = *s;

        // is the part of decimal?
        if (ch == '.')
        {
            if (!dec)
            {
                dec = 1;
                s++;
                continue;
            }
            else
                break;
        }

        // parse integer & decimal
        if (xu_isdigit10(ch))
        {
            // save decimals
            if (dec)
            {
                if (d < e)
                {
                    if (ch != '0')
                    {
                        // fill '0'
                        while (zeros--)
                            *d++ = 0;
                        zeros = 0;

                        // save decimal
                        *d++ = ch - '0';
                    }
                    else
                        zeros++;
                }
            }
            else
                lhs = lhs * 10 + (ch - '0');
        }
        else
            break;

        s++;
    }

    // check
    xu_assert_and_check_return_val(d <= decimals + 256, 0);

    // compute decimal
    while (d-- > decimals)
        rhs = (rhs + *d) / 10;

    // merge
    return (sign ? ((xu_double_t)lhs + rhs) * -1. : ((xu_double_t)lhs + rhs));
}
xu_double_t xu_s16tod(xu_char_t const* s)
{
    // check
    xu_assert_and_check_return_val(s, 0);

    // skip space
    while (xu_isspace(*s))
        s++;

    // has sign?
    xu_int_t sign = 0;
    if (*s == '-')
    {
        sign = 1;
        s++;
    }
    // skip '+'
    else if (*s == '+')
        s++;

    // nan?
    if (s[0] == 'n' && s[1] == 'a' && s[2] == 'n') return XU_NAN;

    // inf or -inf?
    if (s[0] == 'i' && s[1] == 'n' && s[2] == 'f') return sign ? -XU_INF : XU_INF;

    // skip "0x"
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) s += 2;

    // compute double: lhs.rhs
    xu_int_t    dec   = 0;
    xu_uint64_t lhs   = 0;
    xu_double_t rhs   = 0.;
    xu_int_t    zeros = 0;
    xu_int8_t   decimals[256];
    xu_int8_t*  d = decimals;
    xu_int8_t*  e = decimals + 256;
    while (*s)
    {
        xu_char_t ch = *s;

        // is the part of decimal?
        if (ch == '.')
        {
            if (!dec)
            {
                dec = 1;
                s++;
                continue;
            }
            else
                break;
        }

        // parse integer & decimal
        if (xu_isdigit10(ch))
        {
            // save decimals
            if (dec)
            {
                if (d < e)
                {
                    if (ch != '0')
                    {
                        // fill '0'
                        while (zeros--)
                            *d++ = 0;
                        zeros = 0;

                        // save decimal
                        *d++ = ch - '0';
                    }
                    else
                        zeros++;
                }
            }
            else
                lhs = (lhs << 4) + (ch - '0');
        }
        else if (ch > ('a' - 1) && ch < ('f' + 1))
        {
            // save decimals
            if (dec)
            {
                if (d < e)
                {
                    if (ch != '0')
                    {
                        // fill '0'
                        while (zeros--)
                            *d++ = 0;
                        zeros = 0;

                        // save decimal
                        *d++ = (ch - 'a') + 10;
                    }
                    else
                        zeros++;
                }
            }
            else
                lhs = (lhs << 4) + (ch - 'a') + 10;
        }
        else if (ch > ('A' - 1) && ch < ('F' + 1))
        {
            // save decimals
            if (dec)
            {
                if (d < e)
                {
                    if (ch != '0')
                    {
                        // fill '0'
                        while (zeros--)
                            *d++ = 0;
                        zeros = 0;

                        // save decimal
                        *d++ = (ch - 'A') + 10;
                    }
                    else
                        zeros++;
                }
            }
            else
                lhs = (lhs << 4) + (ch - 'A') + 10;
        }
        else
            break;

        s++;
    }

    // check
    xu_assert_and_check_return_val(d <= decimals + 256, 0);

    // compute decimal
    while (d-- > decimals)
        rhs = (rhs + *d) / 16;

    // merge
    return (sign ? ((xu_double_t)lhs + rhs) * -1. : ((xu_double_t)lhs + rhs));
}
xu_double_t xu_stod(xu_char_t const* s)
{
    // check
    xu_assert_and_check_return_val(s, 0);

    // skip space
    xu_char_t const* p = s;
    while (xu_isspace(*p))
        p++;

    // has sign?
    if (*p == '-' || *p == '+') p++;

    // is hex?
    if (*p++ == '0')
    {
        if (*p == 'x' || *p == 'X')
            return xu_s16tod(s);
        else if (*p == 'b' || *p == 'B')
            return xu_s2tod(s);
        else
            return xu_s8tod(s);
    }
    else
        return xu_s10tod(s);
}
xu_double_t xu_sbtod(xu_char_t const* s, xu_int_t base)
{
    // check
    xu_assert_and_check_return_val(s, 0);

    // the convect functions
    static xu_double_t (*s_conv[])(xu_char_t const*) = {xu_null, xu_null, xu_s2tod, xu_null, xu_null,   xu_null,
                                                        xu_null, xu_null, xu_s8tod, xu_null, xu_s10tod, xu_null,
                                                        xu_null, xu_null, xu_null,  xu_null, xu_s16tod};
    xu_assert_and_check_return_val(base < xu_arrayn(s_conv) && s_conv[base], 0);

    // convect it
    return s_conv[base](s);
}
#endif
