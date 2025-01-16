#include "xutil/utils/url.h"
#include "xutil/libc/misc/ctype.h"
#include "xutil/libc/stdlib/stdlib.h"

xu_size_t xu_url_encode(xu_char_t const* ib, xu_size_t in, xu_char_t* ob, xu_size_t on)
{
    // init
    xu_char_t const* ip   = ib;
    xu_char_t*       op   = ob;
    xu_char_t const* ie   = ib + in;
    xu_char_t const* oe   = ob + on;
    static xu_char_t ht[] = "0123456789ABCDEF";

    // done
    while (ip < ie && op < oe)
    {
        // character
        xu_byte_t c = *ip++;

        // space?
        if (c == ' ') *op++ = '+';
        // %xx?
        else if ((c < '0' && c != '-' && c != '.') || (c < 'A' && c > '9') || (c > 'Z' && c < 'a' && c != '_') ||
                 (c > 'z'))
        {
            op[0] = '%';
            op[1] = ht[c >> 4];
            op[2] = ht[c & 15];
            op += 3;
        }
        else
            *op++ = c;
    }

    // end
    *op = '\0';

    // ok
    return op - ob;
}
xu_size_t xu_url_decode(xu_char_t const* ib, xu_size_t in, xu_char_t* ob, xu_size_t on)
{
    // init
    xu_char_t const* ip = ib;
    xu_char_t*       op = ob;
    xu_char_t const* ie = ib + in;
    xu_char_t const* oe = ob + on;

    // done
    xu_char_t ch[3] = {0};
    while (ip < ie && op < oe)
    {
        // space?
        if (*ip == '+') *op = ' ';
        // %xx?
        else if (*ip == '%' && ip + 2 < ie && xu_isdigit16(ip[1]) && xu_isdigit16(ip[2]))
        {
            ch[0] = ip[1];
            ch[1] = ip[2];
            *op   = (xu_char_t)xu_s16tou32(ch);
            ip += 2;
        }
        else
            *op = *ip;

        // next
        ip++;
        op++;
    }

    // end
    *op = '\0';

    // ok
    return op - ob;
}
xu_size_t xu_url_encode2(xu_char_t const* ib, xu_size_t in, xu_char_t* ob, xu_size_t on)
{
    // init
    xu_char_t const* ip   = ib;
    xu_char_t*       op   = ob;
    xu_char_t const* ie   = ib + in;
    xu_char_t const* oe   = ob + on;
    static xu_char_t ht[] = "0123456789ABCDEF";

    // done
    while (ip < ie && op < oe)
    {
        // character
        xu_byte_t c = *ip++;

        // %xx?
        if ((c < '0' && c != '-' && c != '.' && c != '&' && c != '!' && c != '#' && c != '$' && c != '\'' && c != '(' &&
             c != ')' && c != '+' && c != ',' && c != '*' && c != '/') ||
            (c < 'A' && c > '9' && c != '@' && c != '?' && c != '=' && c != ';' && c != ':') ||
            (c > 'Z' && c < 'a' && c != '_') || (c > 'z' && c != '~'))
        {
            op[0] = '%';
            op[1] = ht[c >> 4];
            op[2] = ht[c & 15];
            op += 3;
        }
        else
            *op++ = c;
    }

    // end
    *op = '\0';

    // ok
    return op - ob;
}
xu_size_t xu_url_decode2(xu_char_t const* ib, xu_size_t in, xu_char_t* ob, xu_size_t on)
{
    // init
    xu_char_t const* ip = ib;
    xu_char_t*       op = ob;
    xu_char_t const* ie = ib + in;
    xu_char_t const* oe = ob + on;

    // done
    xu_char_t ch[3] = {0};
    while (ip < ie && op < oe)
    {
        // %xx?
        if (*ip == '%' && ip + 2 < ie && xu_isdigit16(ip[1]) && xu_isdigit16(ip[2]))
        {
            ch[0] = ip[1];
            ch[1] = ip[2];
            *op   = (xu_char_t)xu_s16tou32(ch);
            ip += 2;
        }
        else
            *op = *ip;

        // next
        ip++;
        op++;
    }

    // end
    *op = '\0';

    // ok
    return op - ob;
}
xu_size_t xu_url_encode_args(xu_char_t const* ib, xu_size_t in, xu_char_t* ob, xu_size_t on)
{
    // init
    xu_char_t const* ip   = ib;
    xu_char_t*       op   = ob;
    xu_char_t const* ie   = ib + in;
    xu_char_t const* oe   = ob + on;
    static xu_char_t ht[] = "0123456789ABCDEF";

    // done
    while (ip < ie && op < oe)
    {
        // character
        xu_byte_t c = *ip++;

        // %xx?
        if ((c < '0' && c != '-' && c != '.' && c != '!' && c != '(' && c != ')' && c != '*' && c != '\'') ||
            (c < 'A' && c > '9') || (c > 'Z' && c < 'a' && c != '_') || (c > 'z' && c != '~'))
        {
            op[0] = '%';
            op[1] = ht[c >> 4];
            op[2] = ht[c & 15];
            op += 3;
        }
        else
            *op++ = c;
    }

    // end
    *op = '\0';

    // ok
    return op - ob;
}
xu_size_t xu_url_decode_args(xu_char_t const* ib, xu_size_t in, xu_char_t* ob, xu_size_t on)
{
    return xu_url_decode2(ib, in, ob, on);
}
