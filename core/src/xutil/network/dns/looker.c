/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "dns_looker"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/network/dns/looker.h"
#include "xutil/network/dns/cache.h"
#include "xutil/network/dns/prefix.h"
#include "xutil/network/dns/server.h"
#include "xutil/platform/socket.h"
#include "xutil/string/static_string.h"
#include "xutil/utils/used.h"

/* *******************************************************
 * macros
 */

// the dns looker timeout
#define XU_DNS_LOOKER_TIMEOUT (5000)

/* *******************************************************
 * implementation
 */

// the dns looker step enum
typedef enum __xu_dns_looker_step_e
{
    XU_DNS_LOOKER_STEP_NONE = 0,
    XU_DNS_LOOKER_STEP_REQT = 1,
    XU_DNS_LOOKER_STEP_RESP = 2,
    XU_DNS_LOOKER_STEP_NEVT = 4

} xu_dns_looker_step_e;

// the dns looker type
typedef struct __xu_dns_looker_t
{
    // the name
    xu_static_string_t name;

    // the request and response packet
    xu_static_buffer_t rpkt;

    // the size for recv and send packet
    xu_size_t size;

    // the iterator
    xu_size_t itor;

    // the step
    xu_size_t step;

    // the tryn
    xu_size_t tryn;

    // the socket
    xu_socket_ref_t sock;

    // the socket family
    xu_uint8_t family;

    // the server list
    xu_ipaddr_t list[2];

    // the server maxn
    xu_size_t maxn;

    // the data
    xu_byte_t data[XU_DNS_NAME_MAXN + XU_DNS_RPKT_MAXN];

} xu_dns_looker_t;

/* *******************************************************
 * implementation
 */
static xu_long_t xu_dns_looker_reqt(xu_dns_looker_t* looker)
{
    // check
    xu_check_return_val(!(looker->step & XU_DNS_LOOKER_STEP_REQT), 1);

    // format it first if the request is null
    if (!xu_static_buffer_size(&looker->rpkt))
    {
        // check size
        xu_assert_and_check_return_val(!looker->size, -1);

        // format query
        xu_static_stream_t stream;
        xu_byte_t          rpkt[XU_DNS_RPKT_MAXN];
        xu_size_t          size = 0;
        xu_byte_t*         p    = xu_null;
        xu_static_stream_init(&stream, rpkt, XU_DNS_RPKT_MAXN);

        // identification number
        xu_static_stream_writ_u16_be(&stream, XU_DNS_HEADER_MAGIC);

        /* 0x2104: 0 0000 001 0000 0000
         *
         * xu_uint16_t qr     :1;       // query/response flag
         * xu_uint16_t opcode :4;       // purpose of message
         * xu_uint16_t aa     :1;       // authoritive answer
         * xu_uint16_t tc     :1;       // truncated message
         * xu_uint16_t rd     :1;       // recursion desired

         * xu_uint16_t ra     :1;       // recursion available
         * xu_uint16_t z      :1;       // its z! reserved
         * xu_uint16_t ad     :1;       // authenticated data
         * xu_uint16_t cd     :1;       // checking disabled
         * xu_uint16_t rcode  :4;       // response code
         *
         * this is a query
         * this is a standard query
         * not authoritive answer
         * not truncated
         * recursion desired
         *
         * recursion not available! hey we dont have it (lol)
         *
         */
#if 1
        xu_static_stream_writ_u16_be(&stream, 0x0100);
#else
        xu_static_stream_writ_u1(&stream, 0);         // this is a query
        xu_static_stream_writ_ubits32(&stream, 0, 4); // this is a standard query
        xu_static_stream_writ_u1(&stream, 0);         // not authoritive answer
        xu_static_stream_writ_u1(&stream, 0);         // not truncated
        xu_static_stream_writ_u1(&stream, 1);         // recursion desired

        xu_static_stream_writ_u1(&stream, 0); // recursion not available! hey we dont have it (lol)
        xu_static_stream_writ_u1(&stream, 0);
        xu_static_stream_writ_u1(&stream, 0);
        xu_static_stream_writ_u1(&stream, 0);
        xu_static_stream_writ_ubits32(&stream, 0, 4);
#endif

        /* we have only one question
         *
         * xu_uint16_t question;        // number of question entries
         * xu_uint16_t answer;          // number of answer entries
         * xu_uint16_t authority;       // number of authority entries
         * xu_uint16_t resource;        // number of resource entries
         *
         */
        xu_static_stream_writ_u16_be(&stream, 1);
        xu_static_stream_writ_u16_be(&stream, 0);
        xu_static_stream_writ_u16_be(&stream, 0);
        xu_static_stream_writ_u16_be(&stream, 0);

        // set questions, see as xu_dns_question_t
        // name + question1 + question2 + ...
        xu_static_stream_writ_u8(&stream, '.');
        p = (xu_byte_t*)xu_static_stream_writ_cstr(&stream, xu_static_string_cstr(&looker->name));

        // only one question now.
        xu_static_stream_writ_u16_be(&stream, 1); // we are requesting the ipv4 address
        xu_static_stream_writ_u16_be(&stream, 1); // it's internet (lol)

        // encode dns name
        if (!p || !xu_dns_encode_name((xu_char_t*)p - 1)) return -1;

        // size
        size = xu_static_stream_offset(&stream);
        xu_assert_and_check_return_val(size, -1);

        // copy
        xu_static_buffer_memncpy(&looker->rpkt, rpkt, size);
    }

    // data && size
    xu_byte_t const* data = xu_static_buffer_data(&looker->rpkt);
    xu_size_t        size = xu_static_buffer_size(&looker->rpkt);

    // check
    xu_assert_and_check_return_val(data && size && looker->size < size, -1);

    // try get addr from the dns list
    xu_ipaddr_ref_t addr = xu_null;
    if (looker->maxn && looker->itor && looker->itor <= looker->maxn) addr = &looker->list[looker->itor - 1];

    // check
    xu_assert_and_check_return_val(addr && !xu_ipaddr_is_empty(addr), -1);

    // family have been changed? reinit socket
    if (xu_ipaddr_family(addr) != looker->family)
    {
        // exit the previous socket
        if (looker->sock) xu_socket_exit(looker->sock);

        // init a new socket for the family
        looker->sock = xu_socket_init(XU_SOCKET_TYPE_UDP, xu_ipaddr_family(addr));
        xu_assert_and_check_return_val(looker->sock, -1);

        // update the new family
        looker->family = (xu_uint8_t)xu_ipaddr_family(addr);
    }

    // need wait if no data
    looker->step &= ~XU_DNS_LOOKER_STEP_NEVT;

    // trace
    xu_trace_d("request: try %{ipaddr}", addr);

    // send request
    while (looker->size < size)
    {
        // writ data
        xu_long_t writ = xu_socket_usend(looker->sock, addr, data + looker->size, size - looker->size);
        xu_assert_and_check_return_val(writ >= 0, -1);

        // no data?
        if (!writ)
        {
            // abort?
            xu_check_return_val(!looker->size && !looker->tryn, -1);

            // tryn++
            looker->tryn++;

            // continue
            return 0;
        }
        else
            looker->tryn = 0;

        // update size
        looker->size += writ;
    }

    // finish it
    looker->step |= XU_DNS_LOOKER_STEP_REQT;
    looker->tryn = 0;

    // reset rpkt
    looker->size = 0;
    xu_static_buffer_clear(&looker->rpkt);

    // ok
    xu_trace_d("request: ok");
    return 1;
}
static xu_bool_t xu_dns_looker_resp_done(xu_dns_looker_t* looker, xu_ipaddr_ref_t addr)
{
    // the rpkt and size
    xu_byte_t const* rpkt = xu_static_buffer_data(&looker->rpkt);
    xu_size_t        size = xu_static_buffer_size(&looker->rpkt);
    xu_assert_and_check_return_val(rpkt && size >= XU_DNS_HEADER_SIZE, xu_false);

    // init stream
    xu_static_stream_t stream;
    xu_static_stream_init(&stream, (xu_byte_t*)rpkt, size);

    // init header
    xu_dns_header_t header;
    header.id = xu_static_stream_read_u16_be(&stream);
    xu_static_stream_skip(&stream, 2);
    header.question  = xu_static_stream_read_u16_be(&stream);
    header.answer    = xu_static_stream_read_u16_be(&stream);
    header.authority = xu_static_stream_read_u16_be(&stream);
    header.resource  = xu_static_stream_read_u16_be(&stream);

    // trace
    xu_trace_d("response: size: %u", size);
    xu_trace_d("response: id: 0x%04x", header.id);
    xu_trace_d("response: question: %d", header.question);
    xu_trace_d("response: answer: %d", header.answer);
    xu_trace_d("response: authority: %d", header.authority);
    xu_trace_d("response: resource: %d", header.resource);
    xu_trace_d("");

    // check header
    xu_assert_and_check_return_val(header.id == XU_DNS_HEADER_MAGIC, xu_false);

    // skip questions, only one question now.
    // name + question1 + question2 + ...
    xu_assert_and_check_return_val(header.question == 1, xu_false);
#if 1
    xu_static_stream_skip_cstr(&stream);
    xu_static_stream_skip(&stream, 4);
#else
    xu_char_t* name = xu_static_stream_read_cstr(&stream);
    // name = xu_dns_decode_name(name);
    xu_assert_and_check_return_val(name, xu_false);
    xu_static_stream_skip(&stream, 4);
    xu_trace_d("response: name: %s", name);
#endif

    // decode answers
    xu_size_t i     = 0;
    xu_size_t found = 0;
    for (i = 0; i < header.answer; i++)
    {
        // decode answer
        xu_dns_answer_t answer;
        xu_trace_d("response: answer: %d", i);

        // decode dns name
        xu_char_t const* name = xu_dns_decode_name(&stream, answer.name);
        xu_used(name);
        xu_trace_d("response: name: %s", name);

        // decode resource
        answer.res.type   = xu_static_stream_read_u16_be(&stream);
        answer.res.class_ = xu_static_stream_read_u16_be(&stream);
        answer.res.ttl    = xu_static_stream_read_u32_be(&stream);
        answer.res.size   = xu_static_stream_read_u16_be(&stream);
        xu_trace_d("response: type: %d", answer.res.type);
        xu_trace_d("response: class: %d", answer.res.class_);
        xu_trace_d("response: ttl: %d", answer.res.ttl);
        xu_trace_d("response: size: %d", answer.res.size);

        // is ipv4?
        if (answer.res.type == 1)
        {
            // get ipv4
            xu_byte_t b1 = xu_static_stream_read_u8(&stream);
            xu_byte_t b2 = xu_static_stream_read_u8(&stream);
            xu_byte_t b3 = xu_static_stream_read_u8(&stream);
            xu_byte_t b4 = xu_static_stream_read_u8(&stream);

            // trace
            xu_trace_d("response: ipv4: %u.%u.%u.%u", b1, b2, b3, b4);

            // save the first ip
            if (!found)
            {
                // save it
                if (addr)
                {
                    // init ipv4
                    xu_ipv4_t ipv4;
                    ipv4.u8[0] = b1;
                    ipv4.u8[1] = b2;
                    ipv4.u8[2] = b3;
                    ipv4.u8[3] = b4;

                    // save ipv4
                    xu_ipaddr_ipv4_set(addr, &ipv4);
                }

                // found it
                found = 1;

                // trace
                xu_trace_d("response: ");
                break;
            }
        }
        else
        {
            // decode rdata
            answer.rdata = (xu_byte_t*)xu_dns_decode_name(&stream, answer.name);

            // trace
            xu_trace_d("response: alias: %s", answer.rdata ? (xu_char_t const*)answer.rdata : "");
        }

        // trace
        xu_trace_d("response: ");
    }

    // found it?
    xu_check_return_val(found, xu_false);

#if 0
    // decode authorities
    for (i = 0; i < header.authority; i++)
    {
        // decode answer
        xu_dns_answer_t answer;
        xu_trace_d("response: authority: %d", i);

        // decode dns name
        xu_char_t* name = xu_dns_decode_name(&stream, answer.name);
        xu_trace_d("response: name: %s", name? name : "");

        // decode resource
        answer.res.type =   xu_static_stream_read_u16_be(&stream);
        answer.res.class_ = xu_static_stream_read_u16_be(&stream);
        answer.res.ttl =    xu_static_stream_read_u32_be(&stream);
        answer.res.size =   xu_static_stream_read_u16_be(&stream);
        xu_trace_d("response: type: %d",    answer.res.type);
        xu_trace_d("response: class: %d",   answer.res.class_);
        xu_trace_d("response: ttl: %d",     answer.res.ttl);
        xu_trace_d("response: size: %d",    answer.res.size);

        // is ipv4?
        if (answer.res.type == 1)
        {
            xu_byte_t b1 = xu_static_stream_read_u8(&stream);
            xu_byte_t b2 = xu_static_stream_read_u8(&stream);
            xu_byte_t b3 = xu_static_stream_read_u8(&stream);
            xu_byte_t b4 = xu_static_stream_read_u8(&stream);
            xu_trace_d("response: ipv4: %u.%u.%u.%u", b1, b2, b3, b4);
        }
        else
        {
            // decode data
            answer.rdata = xu_dns_decode_name(&stream, answer.name);
            xu_trace_d("response: server: %s", answer.rdata? answer.rdata : "");
        }
        xu_trace_d("response: ");
    }

    for (i = 0; i < header.resource; i++)
    {
        // decode answer
        xu_dns_answer_t answer;
        xu_trace_d("response: resource: %d", i);

        // decode dns name
        xu_char_t* name = xu_dns_decode_name(&stream, answer.name);
        xu_trace_d("response: name: %s", name? name : "");

        // decode resource
        answer.res.type =   xu_static_stream_read_u16_be(&stream);
        answer.res.class_ = xu_static_stream_read_u16_be(&stream);
        answer.res.ttl =    xu_static_stream_read_u32_be(&stream);
        answer.res.size =   xu_static_stream_read_u16_be(&stream);
        xu_trace_d("response: type: %d",    answer.res.type);
        xu_trace_d("response: class: %d",   answer.res.class_);
        xu_trace_d("response: ttl: %d",     answer.res.ttl);
        xu_trace_d("response: size: %d",    answer.res.size);

        // is ipv4?
        if (answer.res.type == 1)
        {
            xu_byte_t b1 = xu_static_stream_read_u8(&stream);
            xu_byte_t b2 = xu_static_stream_read_u8(&stream);
            xu_byte_t b3 = xu_static_stream_read_u8(&stream);
            xu_byte_t b4 = xu_static_stream_read_u8(&stream);
            xu_trace_d("response: ipv4: %u.%u.%u.%u", b1, b2, b3, b4);
        }
        else
        {
            // decode data
            answer.rdata = xu_dns_decode_name(&stream, answer.name);
            xu_trace_d("response: alias: %s", answer.rdata? answer.rdata : "");
        }
        xu_trace_d("response: ");
    }
#endif

    // ok
    return xu_true;
}
static xu_long_t xu_dns_looker_resp(xu_dns_looker_t* looker, xu_ipaddr_ref_t addr)
{
    // check
    xu_check_return_val(!(looker->step & XU_DNS_LOOKER_STEP_RESP), 1);

    // need wait if no data
    looker->step &= ~XU_DNS_LOOKER_STEP_NEVT;

    // recv response data
    xu_size_t  size = xu_static_buffer_size(&looker->rpkt);
    xu_size_t  maxn = xu_static_buffer_maxn(&looker->rpkt);
    xu_byte_t* data = xu_static_buffer_data(&looker->rpkt);
    while (size < maxn)
    {
        // read data
        xu_long_t read = xu_socket_urecv(looker->sock, xu_null, data + size, maxn - size);
        xu_assert_and_check_return_val(read >= 0, -1);

        // no data?
        if (!read)
        {
            // end? read x, read 0
            xu_check_break(!xu_static_buffer_size(&looker->rpkt));

            // abort? read 0, read 0
            xu_check_return_val(!looker->tryn, -1);

            // tryn++
            looker->tryn++;

            // continue
            return 0;
        }
        else
            looker->tryn = 0;

        // update buffer size
        xu_static_buffer_resize(&looker->rpkt, size + read);
        size = xu_static_buffer_size(&looker->rpkt);
    }

    // done
    if (!xu_dns_looker_resp_done(looker, addr)) return -1;

    // check
    xu_assert_and_check_return_val(xu_static_string_size(&looker->name) && !xu_ipaddr_ip_is_empty(addr), -1);

    // save address to cache
    xu_dns_cache_set(xu_static_string_cstr(&looker->name), addr);

    // finish it
    looker->step |= XU_DNS_LOOKER_STEP_RESP;
    looker->tryn = 0;

    // reset rpkt
    looker->size = 0;
    xu_static_buffer_clear(&looker->rpkt);

    // ok
    xu_trace_d("response: ok");
    return 1;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
xu_dns_looker_ref_t xu_dns_looker_init(xu_char_t const* name)
{
    // check
    xu_assert_and_check_return_val(name, xu_null);

    // must be not address
    xu_assert(!xu_ipaddr_ip_cstr_set(xu_null, name, XU_IPADDR_FAMILY_NONE));

    // done
    xu_bool_t        ok     = xu_false;
    xu_dns_looker_t* looker = xu_null;
    do
    {
        // make looker
        looker = xu_malloc0_type(xu_dns_looker_t);
        xu_assert_and_check_return_val(looker, xu_null);

        // dump server
        //      xu_dns_server_dump();

        // get the dns server list
        looker->maxn = xu_dns_server_get(looker->list);
        xu_check_break(looker->maxn && looker->maxn <= xu_arrayn(looker->list));

        // init name
        if (!xu_static_string_init(&looker->name, (xu_char_t*)looker->data, XU_DNS_NAME_MAXN)) break;
        xu_static_string_cstrcpy(&looker->name, name);

        // init rpkt
        if (!xu_static_buffer_init(&looker->rpkt, looker->data + XU_DNS_NAME_MAXN, XU_DNS_RPKT_MAXN)) break;

        // init family
        looker->family = XU_IPADDR_FAMILY_IPV4;

        // init sock
        looker->sock = xu_socket_init(XU_SOCKET_TYPE_UDP, looker->family);
        xu_assert_and_check_break(looker->sock);

        // init itor
        looker->itor = 1;

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (looker) xu_dns_looker_exit((xu_dns_looker_ref_t)looker);
        looker = xu_null;
    }

    // ok?
    return (xu_dns_looker_ref_t)looker;
}
xu_long_t xu_dns_looker_spak(xu_dns_looker_ref_t self, xu_ipaddr_ref_t addr)
{
    // check
    xu_dns_looker_t* looker = (xu_dns_looker_t*)self;
    xu_assert_and_check_return_val(looker && addr, -1);

    // init
    xu_long_t r = -1;
    do
    {
        // request
        r = xu_dns_looker_reqt(looker);
        xu_check_break(r > 0);

        // response
        r = xu_dns_looker_resp(looker, addr);
        xu_check_break(r > 0);

    } while (0);

    // failed?
    if (r < 0)
    {
        // next
        if (looker->itor + 1 <= looker->maxn)
            looker->itor++;
        else
            looker->itor = 0;

        // has next?
        if (looker->itor)
        {
            // reset step, no event now, need not wait
            looker->step = XU_DNS_LOOKER_STEP_NONE | XU_DNS_LOOKER_STEP_NEVT;

            // reset rpkt
            looker->size = 0;
            xu_static_buffer_clear(&looker->rpkt);

            // continue
            r = 0;
        }
    }

    // ok?
    return r;
}
xu_long_t xu_dns_looker_wait(xu_dns_looker_ref_t self, xu_long_t timeout)
{
    // check
    xu_dns_looker_t* looker = (xu_dns_looker_t*)self;
    xu_assert_and_check_return_val(looker && looker->sock, -1);

    // has io event?
    xu_size_t e = XU_SOCKET_EVENT_NONE;
    if (!(looker->step & XU_DNS_LOOKER_STEP_NEVT))
    {
        if (!(looker->step & XU_DNS_LOOKER_STEP_REQT))
            e = XU_SOCKET_EVENT_SEND;
        else if (!(looker->step & XU_DNS_LOOKER_STEP_RESP))
            e = XU_SOCKET_EVENT_RECV;
    }

    // need wait?
    xu_long_t r = 0;
    if (e)
    {
        // trace
        xu_trace_d("waiting %p ..", looker->sock);

        // wait
        r = xu_socket_wait(looker->sock, e, timeout);

        // fail or timeout?
        xu_check_return_val(r > 0, r);
    }

    // ok?
    return r;
}
xu_void_t xu_dns_looker_exit(xu_dns_looker_ref_t self)
{
    // the looker
    xu_dns_looker_t* looker = (xu_dns_looker_t*)self;
    if (looker)
    {
        // exit sock
        if (looker->sock) xu_socket_exit(looker->sock);
        looker->sock = xu_null;

        // exit it
        xu_free(looker);
    }
}
xu_bool_t xu_dns_looker_done(xu_char_t const* name, xu_ipaddr_ref_t addr)
{
    // check
    xu_assert_and_check_return_val(name && addr, xu_false);

    // try to lookup it from cache first
    if (xu_dns_cache_get(name, addr)) return xu_true;

    // init looker
    xu_dns_looker_ref_t looker = xu_dns_looker_init(name);
    xu_check_return_val(looker, xu_false);

    // spak
    xu_long_t r = -1;
    while (!(r = xu_dns_looker_spak(looker, addr)))
    {
        // wait
        r = xu_dns_looker_wait(looker, XU_DNS_LOOKER_TIMEOUT);
        xu_assert_and_check_break(r >= 0);
    }

    // exit
    xu_dns_looker_exit(looker);

    // ok
    return r > 0 ? xu_true : xu_false;
}
