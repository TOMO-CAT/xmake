#pragma once

#include "xutil/macros/assert.h"
#include "xutil/prefix.h"
#include "xutil/stream/static_stream.h"

/* *******************************************************
 * macros
 */

// the protocol header
#define XU_DNS_HEADER_SIZE (12)
#define XU_DNS_HEADER_MAGIC (0xbeef)

// the protocol port
#define XU_DNS_HOST_PORT (53)

// the name maximum size
#define XU_DNS_NAME_MAXN (256)

// the rpkt maximum size
#define XU_DNS_RPKT_MAXN (XU_DNS_HEADER_SIZE + XU_DNS_NAME_MAXN + 256)

/* *******************************************************
 * types
 */

// the dns header type
typedef struct __xu_dns_header_t
{
    xu_uint16_t id; // identification number

    xu_uint16_t qr : 1;     // query/response flag
    xu_uint16_t opcode : 4; // purpose of message
    xu_uint16_t aa : 1;     // authoritive answer
    xu_uint16_t tc : 1;     // truncated message
    xu_uint16_t rd : 1;     // recursion desired

    xu_uint16_t ra : 1;    // recursion available
    xu_uint16_t z : 1;     // its z! reserved
    xu_uint16_t ad : 1;    // authenticated data
    xu_uint16_t cd : 1;    // checking disabled
    xu_uint16_t rcode : 4; // response code

    xu_uint16_t question;  // number of question entries
    xu_uint16_t answer;    // number of answer entries
    xu_uint16_t authority; // number of authority entries
    xu_uint16_t resource;  // number of resource entries

} xu_dns_header_t;

// the dns question type
typedef struct __xu_dns_question_t
{
    xu_uint16_t type;
    xu_uint16_t class_;

} xu_dns_question_t;

// the dns resource type
typedef struct __xu_dns_resource_t
{
    xu_uint16_t type;
    xu_uint16_t class_;
    xu_uint32_t ttl;
    xu_uint16_t size;

} xu_dns_resource_t;

// the dns answer type
typedef struct __xu_dns_answer_t
{
    xu_char_t         name[XU_DNS_NAME_MAXN];
    xu_dns_resource_t res;
    xu_byte_t const*  rdata;

} xu_dns_answer_t;

/* *******************************************************
 * inlines
 */

// size + data, e.g. .www.google.com => 3www6google3com
static __xu_inline__ xu_char_t const* xu_dns_encode_name(xu_char_t* name)
{
    xu_assert_and_check_return_val(name && name[0] == '.', xu_null);

    // encode
    xu_byte_t  n = 0;
    xu_char_t* b = name;
    xu_char_t* p = name + 1;
    while (*p)
    {
        if (*p == '.')
        {
            //*b = '0' + n;
            *b = (xu_char_t)n;
            n  = 0;
            b  = p;
        }
        else
            n++;
        p++;
    }
    //*b = '0' + n;
    *b = n;

    // ok
    return name;
}
static __xu_inline__ xu_char_t const* xu_dns_decode_name_impl(xu_char_t const* sb, xu_char_t const* se,
                                                              xu_char_t const* ps, xu_char_t** pd)
{
    xu_char_t const* p = ps;
    xu_char_t*       q = *pd;
    while (p < se)
    {
        xu_byte_t c = *p++;
        if (!c) break;
        // is pointer? 11xxxxxx xxxxxxxx
        else if (c >= 0xc0)
        {
            xu_uint16_t pos = c;
            pos &= ~0xc0;
            pos <<= 8;
            pos |= *p++;
            xu_dns_decode_name_impl(sb, se, sb + pos, &q);
            break;
        }
        // is ascii? 00xxxxxx
        else
        {
            while (c--)
                *q++ = *p++;
            *q++ = '.';
        }
    }
    *pd = q;
    return p;
}
static __xu_inline__ xu_char_t const* xu_dns_decode_name(xu_static_stream_ref_t sstream, xu_char_t* name)
{
    xu_char_t* q = name;
    xu_char_t* p = (xu_char_t*)xu_dns_decode_name_impl((xu_char_t const*)xu_static_stream_beg(sstream),
                                                       (xu_char_t const*)xu_static_stream_end(sstream),
                                                       (xu_char_t const*)xu_static_stream_pos(sstream), &q);
    if (p)
    {
        xu_assert(q - name < XU_DNS_NAME_MAXN);
        if (q > name && *(q - 1) == '.') *--q = '\0';
        xu_static_stream_goto(sstream, (xu_byte_t*)p);
        return name;
    }
    else
        return xu_null;
}
